#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Apr 24 22:08:27 2019

@author: aiden
"""

import requests
import json
import stopwatch
from usefultools import split
import multiprocessing as mp
import gspread
from oauth2client.client import SignedJwtAssertionCredentials
import time


class getData:
    def __init__(self):
        self.sku = ""
        
        self.sheet_name = ""
        
        self.matchesWonUrl = 'https://api.vexdb.io/v1/get_matches?round=2'
        self.skillsUrl = 'https://api.vexdb.io/v1/get_skills?'
        self.pointsUrl = 'https://api.vexdb.io/v1/get_rankings?sku=' + self.sku

        self.teams = []
        self.allTeams = []
        self.ranges ={}

        self.matchesWonData = {}
        self.matchesWonOverallData = {}
        self.combinedSkillsScoreData = {}
        self.driverSkillsScoreData = {}
        self.pointsData = {}
        self.rankingData = {}

        self.NUM_PROCESSES = 20
        self.TEAM_NUM_COL = 2

        self.sheet = None


    def __calcWin(self, team, data):
        """
        calculates if a match was won or not by finding what
        color a team was and the score of the match
        it is a win if the teams color score more points
        """
        colors = ['red1', 'red2', 'blue1', 'blue2']
        teamColor = ''
        for color in colors:
            if data.get(color) == team:
                c = color.split('1')[0]
                c = c.split('2')[0]
                teamColor = c
                break

        redScore = data.get('redscore')
        blueScore = data.get('bluescore')

        if redScore != blueScore:
            if redScore > blueScore:
                winner = 'red'
            else:
                winner = 'blue'

            if teamColor == 'red' and winner == 'red':
                return 1
            elif teamColor == 'blue' and winner == 'blue':
                return 1
            else:
                return 0

        elif redScore == blueScore and (redScore != 0 and blueScore != 0):
            return -1

        else:
            return None




    def __getMatchesWon(self, teams, q):
        """
        gets win/loss/tie data for a team
        """
        for team in teams:
            team = team.split('\n')[0]
            url = self.matchesWonUrl + '&team=' + team + '&season=Tower Takeover'

            data = requests.get(url)
            if data.status_code == 200 and team != '':

                winStruct = {'wins':0,
                             'losses':0,
                             'ties':0
                        }

                data = json.loads(data.content.decode('utf-8'))
                data = data.get('result')

                for entry in data:
#                    print(team, entry)
                    x = self.__calcWin(team, entry)
                    if x == 1:
                        winStruct['wins'] = winStruct.get('wins') + 1
                    elif x == 0:
                        winStruct['losses'] = winStruct.get('losses') + 1
                    elif x == -1:
                        winStruct['ties'] = winStruct.get('ties') + 1
                    else:
                        pass
                q.put({team: winStruct})



    def __getDriverSkillsScore(self, teams, q):
        """
        gets driver skills score for a team
        """
        for team in teams:
            team = team.split('\n')[0]
            url = self.skillsUrl + 'team=' + team + '&season=Tower Takeover&type=0'

            data = requests.get(url)
            if data.status_code == 200:

                data = json.loads(data.content.decode('utf-8'))
                data = data.get('result')
                highest = 0

                for item in data:
                    value = item.get('score')
                    if value > highest:
                        highest = value

                q.put({team: highest})


    def __getCombinedSkillsScore(self, teams, q):
        """
        gets the combined skills score for a team
        (driver and programming)
        """
        for team in teams:
            team = team.split('\n')[0]
            url2 = self.skillsUrl + 'team=' + team + '&season=Tower Takeover&type=2'

            data = requests.get(url2)
            if data.status_code == 200:


                data = json.loads(data.content.decode('utf-8'))
                data = data.get('result')
                highest = 0

                for item in data:
                    value = item.get('score')
                    if value > highest:
                        highest = value
                q.put({team: highest})


    def __getPoints(self, teams, q):
        """
        gets win points/auton points/strength points/calculated 
        contribution to win margin data for a team
        """
        for team in teams:
            team = team.split('\n')[0]
            url = self.pointsUrl + '&team=' + team + '&season=Tower Takeover&type=2'

            data = requests.get(url)
            if data.status_code == 200:
                try:
                    data = json.loads(data.content.decode('utf-8'))
                    data = data.get('result')[0]
                    pointsStruct = {
                            'wp':data.get('wp'),
                            'ap':data.get('ap'),
                            'sp':data.get('sp'),
                            'ccwm':data.get('ccwm')
                            }
                    q.put({team: pointsStruct})

                except IndexError:
                    q.put({team: 'N/A'})

    def __getRanking(self, teams, q):
        """
        gets the ranking of a team at an event
        """
        for team in teams:
            team = team.split('\n')[0]
            url = self.pointsUrl + '&team=' + team + '&season=Tower Takeover&type=2'
            #print(url)

            data = requests.get(url)
            if data.status_code == 200:

                data = json.loads(data.content.decode('utf-8'))
                try:
                    data = data.get('result')[0]
                    q.put({team: data.get('rank')})

                except IndexError:
                    q.put({team: 'N/A'})


    def __parralellise(self, func, returnDict):
        """
        processes the data in parrallel so that if there is a lot of
        teams the operation can be performed quickly
        """
        queues = []
        processes = []
        for i in range(len(self.teams)):
            queues.append(mp.Queue())
            p = mp.Process(target=func, args=(self.teams[i], queues[i],))
            processes.append(p)
            p.daemon = True
            p.start()
        for process in processes:
            process.join()

        for q in queues:
            while not q.empty():
                returnDict.update(q.get(timeout=.1))





    def openSheet(self):
        """
        opens the sheet
        need to change workbook to the sheet that will be edited
        and the sheet name to the sheet that corresponds with the
        data that will be collected
        """
        json_key = json.load(open('/home/aiden/Documents/google_credentials/creds.json'))
        scope = ['https://spreadsheets.google.com/feeds',
        'https://www.googleapis.com/auth/drive']

        credentials = SignedJwtAssertionCredentials(json_key['client_email'], json_key['private_key'].encode(), scope)

        file = gspread.authorize(credentials)
        workbook = file.open("536C_Scouting")
        self.sheet = workbook.worksheet(self.sheet_name)





    def getTeams(self):
        """
        gets the teams by reading the data in the sheet
        this looks for data to find by comparing the first
        row to a list of valid headers then removes the headers
        from the list of cells to be updated
        """
        valid_headers = [
                'Rank',
                'WP/AP/SP/CCWM',
                'W-L-T (today)',
                'W-L-T (overall season)',
                'driver skills',
                'combined skills score'
                ]

        self.allTeams = self.sheet.col_values(1) #column that teams are stored in
        self.allTeams.pop(0) #remove header
        headers = self.sheet.row_values(1) #remove header
        row = 2
        column = 1
        for header in headers:
            if header in valid_headers:
                start = chr(ord('@')+column) + str(row)
                end = chr(ord('@')+column) + str((len(self.allTeams) + 1))
                string = start + ':' + end
                range_= self.sheet.range(string)
                self.ranges.update({header:range_})
            column += 1
            
        #limits num processes to the number of teams
        if self.NUM_PROCESSES > len(self.allTeams):
            self.NUM_PROCESSES = len(self.allTeams)

        self.teams = list(split.split(self.allTeams, self.NUM_PROCESSES))





    def collect(self):
        """
        collects all the data if that data is a valid header
        """
        if 'W-L-T (overall season)' in self.ranges.keys():
            self.__parralellise(self.__getMatchesWon, self.matchesWonOverallData)
        
        self.matchesWonUrl = self.matchesWonUrl + '&sku=' + self.sku

        if 'W-L-T (today)' in self.ranges.keys():
            self.__parralellise(self.__getMatchesWon, self.matchesWonData)
            
        if 'combined skills score' in self.ranges.keys():
            self.__parralellise(self.__getCombinedSkillsScore, self.combinedSkillsScoreData)
        if 'driver skills' in self.ranges.keys():
            self.__parralellise(self.__getDriverSkillsScore, self.driverSkillsScoreData)
        if 'WP/AP/SP/CCWM' in self.ranges.keys():
            self.__parralellise(self.__getPoints, self.pointsData)
        if 'Rank' in self.ranges.keys():
            self.__parralellise(self.__getRanking, self.rankingData)



    def printData(self):
        """
        prints the data for each team to be used for debugging
        purposes
        """
        if self.matchesWonData:
            #print("w-l-t")
            for team in self.matchesWonData.keys():
                try:
                    wins = self.matchesWonData.get(team)
                    record = [wins.get('wins'), wins.get('losses'), wins.get('ties')]
                    formattedRecord = str(record[0]) + '-' + str(record[1]) + '-' + str(record[2])
                    print(formattedRecord)
                except AttributeError:
                    print('N/A')
            for i in range(20):
                print("")

        if self.combinedSkillsScoreData:
            print("combined skills")
            for team in self.combinedSkillsScoreData.keys():
                print(self.combinedSkillsScoreData.get(team))


            for i in range(20):
                print("")

        if self.driverSkillsScoreData:
            print("driver skills")
            for team in self.driverSkillsScoreData.keys():
                print(self.driverSkillsScoreData.get(team))

            for i in range(20):
                print("")


        if self.rankingData:
            print("ranking")
            for team in self.rankingData.keys():
                print(self.rankingData.get(team))

            for i in range(20):
                print("")


        if self.pointsData:
            print("point values")
            for team in self.pointsData.keys():
                try:
                    val = self.pointsData.get(team)
                    points = (str(val.get('wp'))  + ' / '
                            + str(val.get('ap')) + ' / '
                            + str(val.get('sp')) + ' / '
                            + str(val.get('ccwm'))
                            )
                    print(points)
                except AttributeError:
                    print('N/A')
            for i in range(6):
                print("")





    def writeData(self):
        """
        writes the data in one chunk because the google api
        only allows so many edits
        by making it only one edit once all the data is collected
        this constraing can be worked around
        """
        cell_list = self.ranges.get('Rank') #write rank
        row = 0
        for cell in cell_list:
            try:
                team = self.allTeams[row].split('\n')[0]
                data = self.rankingData.get(team)
            except:
                data = "N/A"
            cell.value = data
            row += 1

        self.sheet.update_cells(cell_list)


        cell_list = self.ranges.get('WP/AP/SP/CCWM') #write points
        row = 0
        for cell in cell_list:
            try:
                team = self.allTeams[row].split('\n')[0]
                val = self.pointsData.get(team)
                data = (str(val.get('wp'))  + ' / '
                            + str(val.get('ap')) + ' / '
                            + str(val.get('sp')) + ' / '
                            + str(val.get('ccwm'))
                            )
                cell.value = data
            except AttributeError:
                cell.value = 'N/A'
            except IndexError:
                cell.value = 'N/A'
            row += 1
        self.sheet.update_cells(cell_list)



        cell_list = self.ranges.get('W-L-T (today)') #write w-l-t
        row = 0
        for cell in cell_list:
            try:
                team = self.allTeams[row].split('\n')[0]
                wins = self.matchesWonData.get(team)
                record = [wins.get('wins'), wins.get('losses'), wins.get('ties')]
                data = str(record[0]) + '-' + str(record[1]) + '-' + str(record[2])

                cell.value = data
            except AttributeError:
                cell.value = 'N/A'
            row += 1

        self.sheet.update_cells(cell_list)



        cell_list = self.ranges.get('W-L-T (overall season)') #write w-l-t for
        row = 0                                               #overall season
        for cell in cell_list:
            try:
                team = self.allTeams[row].split('\n')[0]
                wins = self.matchesWonOverallData.get(team)
                record = [wins.get('wins'), wins.get('losses'), wins.get('ties')]
                data = str(record[0]) + '-' + str(record[1]) + '-' + str(record[2])

                cell.value = data
            except AttributeError:
                cell.value = 'N/A'
            row += 1

        self.sheet.update_cells(cell_list)




        cell_list = self.ranges.get('driver skills')#write driver skills
        row = 0
        for cell in cell_list:
            team = self.allTeams[row].split('\n')[0]
            data = self.driverSkillsScoreData.get(team)
            cell.value = data

            row += 1

        self.sheet.update_cells(cell_list)



        cell_list = self.ranges.get('combined skills score') #write combined skills
        row = 0
        for cell in cell_list:
            team = self.allTeams[row].split('\n')[0]
            data = self.combinedSkillsScoreData.get(team)
            cell.value = data

            row += 1

        self.sheet.update_cells(cell_list)








while 1: #collect data every so often
    stp = stopwatch.stopwatch()
    stp.start()
    d = getData()
    d.openSheet()

    d.getTeams()
    d.collect()
    #print(d.rankingData)
    d.writeData()
    print("time taken:", stp.stop(), "sec")

    time.sleep(45)




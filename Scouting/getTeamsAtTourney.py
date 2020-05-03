#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Apr 28 11:36:52 2019

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


class getTeams:
    """
    class for getting teams at a tournament
    based on the tournaments sku
    """
    def __init__(self):
        self.sheet_name = ""
        
        sku = ""
        
        self.url = "https://api.vexdb.io/v1/get_teams?round=5?&sku=" + sku
        self.elimsUrl = "https://api.vexdb.io/v1/get_teams?round=5?&sku=" + sku + '&matchnum='
        
        #legacy version that only works if matches have started
        #self.url = 'https://api.vexdb.io/v1/get_matches?round=2?&sku=' + sku
        #self.elimsUrl = 'https://api.vexdb.io/v1/get_matches?&sku=' + sku + '&matchnum='

        self.allTeams = []
        self.elimTeams = []

        self.COLLECT_ELIMS = 0
        self.COLLECT_TEAMS = 1

        self.NUM_PROCESSES = 50

        self.sheet = None

    def __getAllTeams(self, dicts, queue):
        """
        gets all teams that are registered
        """
        for entry in dicts:
            print(entry.get("number"))
            queue.put(entry.get("number"))



    def __getElimTeams(self, dicts, queue):
        """
        gets teams that are in the elimination matches
        does not work need to update
        """
        print(dicts)
        for entry in dicts:
            print(entry.get("number"))
            queue.put(entry.get("number"))


    def __parralellise(self, func, returnList, dicts):
        """
        starts threads that look through lists of entries
        for teams that are at the tournament
        this is used so that at events like worlds it does
        not take forever to run
        """
        queues = []
        processes = []
        for i in range(len(dicts)):
            queues.append(mp.Queue())
            p = mp.Process(target=func, args=(dicts[i], queues[i],))
            processes.append(p)
            p.daemon = True
            p.start()
        for process in processes:
            process.join()

        for q in queues:
            while not q.empty():
                returnList.append(q.get(timeout=.1))


    def collect(self):
        """
        collects the data from vexdb and splits it into
        entries based on self.NUM_PROCESSES so that data
        can be parsed faster especially for events like worlds
        """
        data = requests.get(self.url)
        if data.status_code == 200 and self.COLLECT_TEAMS:
            data = json.loads(data.content.decode('utf-8'))
            data = data.get('result')
            data = list(split.split(data, self.NUM_PROCESSES))

            self.__parralellise(self.__getAllTeams, self.allTeams, data)

            self.allTeams = list(set(self.allTeams))


        if self.COLLECT_ELIMS:
            links = []
            for num in range(1, 9):
                match = 'R16 #' + str(num) + '-1'
                link = self.elimsUrl + match
                links.append(link)

            data = []

            for url in links:
                response = requests.get(url)
                if response.status_code == 200:
                    response = json.loads(response.content.decode('utf-8'))
                    allData = response.get('result') + data #merge lists


    #        data = requests.get(self.elimsUrl)
    #        if data.status_code == 200 and self.COLLECT_ELIMS:
    #            data = json.loads(data.content.decode('utf-8'))
    #            data = data.get('result')
            data = list(split.split(allData, self.NUM_PROCESSES))

            self.__parralellise(self.__getElimTeams, self.elimTeams, data)

            self.elimTeams = list(set(self.elimTeams))








    def printTeams(self):
        """
        prints the teams out
        used for debugging
        """
        for team in self.allTeams:
            print(team)

        print("")
        print("")

        for team in self.elimTeams:
            print(team)



    def openSheet(self):
        """
        opens the sheet and loads the work book
        need to change the workbook to the file name
        and the sheet to the sheet that will be edited
        """
        json_key = json.load(open('/home/aiden/Documents/google_credentials/creds.json'))
        scope = scope = ['https://spreadsheets.google.com/feeds',
                         'https://www.googleapis.com/auth/drive']

        credentials = SignedJwtAssertionCredentials(json_key['client_email'], json_key['private_key'].encode(), scope)

        file = gspread.authorize(credentials)
        workbook = file.open("536C_Scouting")
        self.sheet = workbook.worksheet(self.sheet_name)


    def writeData(self, column):
        """
        writes the data in one chunk because the api only allows
        so many operations
        """
        if self.COLLECT_TEAMS:
            #leave room for header by setting to two and adding 1
            start = chr(ord('@')+column) + '2'
            end = chr(ord('@')+column) + str(len(self.allTeams) + 1)
            rang = start + ':' + end
            print(rang)
            cell_list = self.sheet.range(rang)

            x = 0
            for cell in cell_list:
                cell.value = self.allTeams[x]
                x += 1

            self.sheet.update_cells(cell_list)


        if self.COLLECT_ELIMS:
            start = chr(ord('@')+column) + '2'
            end = chr(ord('@')+column) + str(len(self.elimTeams) + 1)
            rang = start + ':' + end
            print(rang)
            cell_list = self.sheet.range(rang)

            x = 0
            for cell in cell_list:
                cell.value = self.elimTeams[x]
                x += 1

            self.sheet.update_cells(cell_list)




g = getTeams()
g.collect()
g.printTeams()
g.openSheet()
print("sheet opened")
g.writeData(1)

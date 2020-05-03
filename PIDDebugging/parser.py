#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Jan  5 15:35:01 2020

@author: aiden
"""
import math
import random 
import os


class Parser:
    """
    parses data from motors so that it can be graphed
    """
    def __init__(self):
        self.__voltage_data = []
        self.__velocity_data = []
        self.__time_data = []
        self.__integral_data = []
        self.__setpoint_data = []

        self.__brakemode = None
        self.__gearset = None
        self.__slew = 0
        self.__pid = {
            "kP":0,
            "kI":0,
            "kD":0,
            "I_max":0
            }

        self.__brakemode_names = {
            0:"Coast",
            1:"Brake",
            2:"Hold",

        }

        self.__gearset_names = {
            0:"36:1",
            1:"18:1",
            2:"6:1"
        }


    def __get_data_point(self, line):
        """
        parses a line from the file and returns the data of voltage,
        velocity, integral value, and time in the form of a dictionary

        sample line:
            [INFO] Motor 1, Brakemode: xxxx, Actual_Voltage: xxx, ...
        """
        try:
            line = line.split("[INFO]")[1]
            data = line.split(",")
        except IndexError:  #ensures that line is an actual data line
            return 0

        try:
            voltage = [ item.strip().split("Actual_Vol:")[1] for item in data if "Actual_Vol:" in item ]
            velocity = [ item.strip().split("Vel:")[1] for item in data if "Vel:" in item ]
            time = [ item.strip().split("Time:")[1] for item in data if "Time:" in item ]
            integral = [ item.strip().split("I:")[1] for item in data if "I:" in item ]
            setpoint = [ item.strip().split("Vel_Sp:")[1] for item in data if "Vel_Sp:" in item ]
    
            data_dict = {
                "voltage":float(voltage[0].strip()),
                "velocity":float(velocity[0].strip()),
                "time":float(time[0].strip()),
                "integral":float(integral[0].strip()),
                "setpoint":float(setpoint[0].strip())
                }
    
            return data_dict
        except IndexError:
            return 0


    def parse_file(self, file):
        """
        parses a file line by line and adds data to list
        """
        f = open(file)

        #find first valid line
        while 1:
            first_line = f.readline()
            data = self.__get_data_point(first_line)

            if data:
                self.__voltage_data.append(data.get("voltage"))
                self.__velocity_data.append(data.get("velocity"))
                self.__time_data.append(data.get("time"))
                self.__integral_data.append(data.get("integral"))
                self.__setpoint_data.append(data.get("setpoint"))

                first_line = first_line.split("[INFO]")[1]
                data = first_line.split(",")

                self.__brakemode = int([ item.strip().split("Brake:")[1].strip() for item in data if "Brake:" in item ][0])
                self.__gearset = int([ item.strip().split("Gear:")[1].strip() for item in data if "Gear:" in item ][0])
                self.__slew = int([ int(item.strip().split("Slew:")[1].strip()) for item in data if "Slew:" in item ][0])

                self.__brakemode = self.__brakemode_names.get(self.__brakemode, "???")
                self.__gearset = self.__gearset_names.get(self.__gearset, "???")

                self.__pid["kP"] = float([ float(item.strip().split("kP:")[1].strip()) for item in data if "kP:" in item ][0])
                self.__pid["kI"] = float([ float(item.strip().split("kI:")[1].strip()) for item in data if "kI:" in item ][0])
                self.__pid["kD"] = float([ float(item.strip().split("kD:")[1].strip()) for item in data if "kD:" in item ][0])
                self.__pid["I_max"] = float([ float(item.strip().split("I_max:")[1].strip()) for item in data if "I_max:" in item ][0])

                break


        for line in f.readlines():
            data = self.__get_data_point(line)
            if data:
                self.__voltage_data.append(data.get("voltage"))
                self.__velocity_data.append(data.get("velocity"))
                self.__time_data.append(data.get("time"))
                self.__integral_data.append(data.get("integral"))
                self.__setpoint_data.append(data.get("setpoint"))

        f.close()


    def print_data(self):
        """
        prints data

        useful for debugging
        """
        print("\nVoltage Data:", len(self.__voltage_data), "data points")
        for item in self.__voltage_data:
            print(item)

        print("\nVelocity Data:", len(self.__velocity_data), "data points")
        for item in self.__velocity_data:
            print(item)

        print("\nIntegral Data:", len(self.__integral_data), "data points")
        for item in self.__integral_data:
            print(item)


        print("\nTime Data:", len(self.__time_data), "data points")
        for item in self.__time_data:
            print(item)

        print("\nSetpoint Data:", len(self.__time_data), "data points")
        for item in self.__setpoint_data:
            print(item)

        print("\nPID constants:")
        for key in self.__pid:
            print(key, ":", self.__pid[key])


        print("\nBrakemode: ", self.__brakemode)
        print("Gearset: ", self.__gearset)
        print("Slew Rate: ", self.__slew)


    def get_data(self):
        """
        returns a dictionary of the data that was parsed
        """
        data = {
            "voltage":self.__voltage_data,
            "velocity":self.__velocity_data,
            "integral":self.__integral_data,
            "setpoint":self.__setpoint_data,
            "time":self.__time_data,
            "brakemode":self.__brakemode,
            "gearset":self.__gearset,
            "slew_rate":self.__slew,
            "pid_constants":self.__pid   
        }

        return data



def gen_sample_data(num_data_pts=1000, setpoint=100, file="ut.txt"):
    """
    makes a random set of data that can be used for a unit test
    """
    os.remove(file)
    f = open(file, "a")
    
    step = setpoint / num_data_pts #setpoint / ...
    min_vel = 0
    for i in range(num_data_pts):
        vel = (random.randint(int(min_vel), int(min_vel + step)) ** 1/((i+setpoint)/num_data_pts)) + setpoint/10
        vol = ((((vel + 200) * (12000 + 12000)) / (200 + 200)) - 12000) + random.randint(-600, 600); #scale vel to voltage range and add jitter
        data = "[INFO] Motor 1, Actual_Vol: " + str(vol)
        data += ", Brake: 1, Gear: 1, I_max: 1000, I: 100, kD: 0, kI: 0, "
        data += "kP: 1.0, Slew: 40, Time: " + str(i)
        data += ", Vel_Sp: " + str(setpoint) + ", Vel: " + str(vel) + "\n"

        f.write(data)

        min_vel += step


    f.close()
    
#unit test
#
#gen_sample_data(file="test.txt")
#P = Parser()
#P.parse_file("test.txt")
#P.print_data()



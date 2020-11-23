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
        self.__voltage_data = {
            "back_right":[],
            "back_left":[],
            "front_right":[],
            "front_left":[]
        }
        self.__velocity_data = {
            "back_right":[],
            "back_left":[],
            "front_right":[],
            "front_left":[]
        }
        self.__time_data = []
        self.__integral_data = []
        self.__vel_setpoint_data = []
        self.__heading_sp_data = []
        self.__heading_data = []
        self.__position_sp = []
        self.__position_l_data = []
        self.__position_r_data = []

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
            voltage1 = [ item.strip().split("Actual_Vol1:")[1].split(": ")[1] for item in data if "Actual_Vol1:" in item ]
            voltage2 = [ item.strip().split("Actual_Vol2:")[1].split(": ")[1] for item in data if "Actual_Vol2:" in item ]
            voltage3 = [ item.strip().split("Actual_Vol3:")[1].split(": ")[1] for item in data if "Actual_Vol3:" in item ]
            voltage4 = [ item.strip().split("Actual_Vol4:")[1].split(": ")[1] for item in data if "Actual_Vol4:" in item ]
            
            velocity1 = [ item.strip().split("Actual_Vel1:")[1].split(": ")[1] for item in data if "Actual_Vel1:" in item ]
            velocity2 = [ item.strip().split("Actual_Vel2:")[1].split(": ")[1] for item in data if "Actual_Vel2:" in item ]
            velocity3 = [ item.strip().split("Actual_Vel3:")[1].split(": ")[1] for item in data if "Actual_Vel3:" in item ]
            velocity4 = [ item.strip().split("Actual_Vel4:")[1].split(": ")[1] for item in data if "Actual_Vel4:" in item ] 
            
            time = [ item.strip().split("Time:")[1] for item in data if "Time:" in item ]
            integral = [ item.strip().split("I:")[1] for item in data if "I:" in item ]
            vel_sp = [ item.strip().split("Vel_Sp:")[1] for item in data if "Vel_Sp:" in item ]
            heading_sp = [ item.strip().split("Heading_Sp:")[1] for item in data if "Heading_Sp:" in item ]
            relative_heading = [ item.strip().split("Relative_Heading:")[1] for item in data if "Relative_Heading:" in item ]
            position_sp = [ item.strip().split("Position_Sp:")[1] for item in data if "Position_Sp:" in item ]
            position_r = [ item.strip().split("position_r:")[1] for item in data if "position_r:" in item ]
            position_l = [ item.strip().split("position_l:")[1] for item in data if "position_l:" in item ]
            
            data_dict = {
                "voltage1":float(voltage1[0].strip()),
                "voltage2":float(voltage2[0].strip()),
                "voltage3":float(voltage3[0].strip()),
                "voltage4":float(voltage4[0].strip()),
                "velocity1":float(velocity1[0].strip()),
                "velocity2":float(velocity2[0].strip()),
                "velocity3":float(velocity3[0].strip()),
                "velocity4":float(velocity4[0].strip()),
                "time":float(time[0].strip()),
                "integral":float(integral[0].strip()),
                "heading_sp":float(heading_sp[0].strip()),
                "relative_heading":float(relative_heading[0].strip()),
                "position_sp":float(position_sp[0].strip()),
                "position_r_data":float(position_r[0].strip()),
                "position_l_data":float(position_l[0].strip())
                }
        except:
                return 0
        
        try:
            data_dict.update({"vel_setpoint":float(vel_sp[0].strip())})
        except IndexError:
            data_dict.update({"vel_setpoint":[]})
            

        return data_dict
        # except IndexError:
        #     return 0


    def parse_file(self, file):
        """
        parses a file line by line and adds data to list
        """
        is_first_line = True
        with open(file) as f:
            #find first valid line
            for line in f:
                if is_first_line:
                    data = self.__get_data_point(line)
                    print(data)
                    if data:
                        self.__voltage_data["back_right"].append(data.get("voltage4"))
                        self.__voltage_data["back_left"].append(data.get("voltage3"))
                        self.__voltage_data["front_right"].append(data.get("voltage2"))
                        self.__voltage_data["front_left"].append(data.get("voltage1"))
                        
                        self.__velocity_data["back_right"].append(data.get("velocity4"))
                        self.__velocity_data["back_left"].append(data.get("velocity3"))
                        self.__velocity_data["front_right"].append(data.get("velocity2"))
                        self.__velocity_data["front_left"].append(data.get("velocity"))
                        
                        self.__time_data.append(data.get("time"))
                        self.__integral_data.append(data.get("integral"))
                        self.__vel_setpoint_data.append(data.get("vel_setpoint"))
                        self.__heading_sp_data.append(data.get("heading_sp"))
                        self.__heading_data.append(data.get("relative_heading"))
                        self.__position_sp.append(data.get("position_sp"))
                        self.__position_r_data.append(data.get("position_r_data"))
                        self.__position_r_data.append(data.get("position_l_data"))
        
                        first_line = line.split("[INFO]")[1]
                        data = first_line.split(",")
        
                        self.__brakemode = int([ item.strip().split("Brake:")[1].strip() for item in data if "Brake:" in item ][0])
                        self.__gearset = int([ item.strip().split("Gear:")[1].strip() for item in data if "Gear:" in item ][0])
                        self.__slew = int([ int(item.strip().split("Slew:")[1].strip()) for item in data if "Slew:" in item ][0])
                        # self.__slew = 120
        
        
                        self.__brakemode = self.__brakemode_names.get(self.__brakemode, "???")
                        self.__gearset = self.__gearset_names.get(self.__gearset, "???")
        
                        self.__pid["kP"] = float([ float(item.strip().split("kP:")[1].strip()) for item in data if "kP:" in item ][0])
                        self.__pid["kI"] = float([ float(item.strip().split("kI:")[1].strip()) for item in data if "kI:" in item ][0])
                        self.__pid["kD"] = float([ float(item.strip().split("kD:")[1].strip()) for item in data if "kD:" in item ][0])
                        self.__pid["I_max"] = float([ float(item.strip().split("I_max:")[1].strip()) for item in data if "I_max:" in item ][0])
                        is_first_line = False
                    continue
        

                data = self.__get_data_point(line)
                if data:
                    self.__voltage_data["back_right"].append(data.get("voltage4"))
                    self.__voltage_data["back_left"].append(data.get("voltage3"))
                    self.__voltage_data["front_right"].append(data.get("voltage2"))
                    self.__voltage_data["front_left"].append(data.get("voltage1"))
                    
                    self.__velocity_data["back_right"].append(data.get("velocity4"))
                    self.__velocity_data["back_left"].append(data.get("velocity3"))
                    self.__velocity_data["front_right"].append(data.get("velocity2"))
                    self.__velocity_data["front_left"].append(data.get("velocity"))
                    
                    self.__time_data.append(data.get("time"))
                    self.__integral_data.append(data.get("integral"))
                    self.__vel_setpoint_data.append(data.get("vel_setpoint"))
                    self.__heading_sp_data.append(data.get("heading_sp"))
                    self.__heading_data.append(data.get("relative_heading"))
                    self.__position_sp.append(data.get("position_sp"))
                    self.__position_data.append(data.get("position_data"))



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

        print("\nVelocity Setpoint Data:", len(self.__vel_setpoint_data), "data points")
        for item in self.__vel_setpoint_data:
            print(item)
            
        print("\nHeading Setpoint Data:", len(self.__heading_sp_data), "data points")
        for item in self.__heading_sp_data:
            print(item)
            
        print("\Relative Heading Data:", len(self.__heading_data), "data points")
        for item in self.__heading_data:
            print(item)

        print("\Position Setpoint:", len(self.__position_sp), "data points")
        for item in self.__position_sp:
            print(item)
            
        print("\Position Data:", len(self.__position_data), "data points")
        for item in self.__position_data:
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
            "vel_setpoint":self.__vel_setpoint_data,
            "heading_setpoint":self.__heading_sp_data,
            "heading_data":self.__heading_data,
            "position_sp":self.__position_sp,
            "position_r":self.__position_r_data,
            "position_l":self.__position_l_data,
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
    if os.path.isfile(file):
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
        data += ", Heading_Sp: 0.0, Relative_Heading: " + str((0 + (random.randrange(-100, 100, 1) / 100)))
        data += ", Position_Sp: 1000, Position: 1000"
        data += ", Vel_Sp: " + str(setpoint) + ", Vel: " + str(vel) + "\n"

        f.write(data)

        min_vel += step


    f.close()
    
# unit test

# gen_sample_data(file="test.txt")
# P = Parser()
# P.parse_file("test.txt")
# P.print_data()



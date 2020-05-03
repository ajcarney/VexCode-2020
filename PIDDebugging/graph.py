#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Dec 29 12:38:16 2019

@author: aiden
"""
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D


class DebugGraph:
    """
    class for making a graph for debugging PID data
    """
    def __init__(self, time_data, velocity_data, voltage_data, setpoint, parameters):
        
#        #calculate axes and round to the nearest 50 that has the higher absolute value
#        time_max = (abs(max(time_data)) / max(time_data)) * 50 * math.ceil(abs(max(time_data))/50)
#        
#        velocity_min = (abs(min(velocity_data)) / min(velocity_data)) * 50 * math.ceil(abs(min(velocity_data))/50)
#        velocity_max = (abs(max(velocity_data)) / max(velocity_data)) * 50 * math.ceil(abs(max(velocity_data))/50)
#        
#        voltage_min = (abs(min(voltage_data)) / min(voltage_data)) * 50 * math.ceil(abs(min(voltage_data))/50)
#        voltage_max = (abs(max(voltage_data)) / max(voltage_data)) * 50 * math.ceil(abs(max(voltage_data))/50)
#        
        #set up graph
        self.__graph = plt.figure(dpi=800)
        self.__ax_vol = self.__graph.add_subplot(111)
        self.__ax_vel = self.__ax_vol.twinx()
        
        #set up axes
#        self.__ax_vel.set_xlim(0, time_max)
#        self.__ax_vel.set_ylim(velocity_min, velocity_max)
#        
#        self.__ax_vol.set_xlim(0, time_max)
#        self.__ax_vol.set_ylim(voltage_min, voltage_max)
        
        #plot lines
        if type(setpoint) != list:
            setpoint_x = [0, max(time_data)]
            setpoint_y = [setpoint, setpoint]
        else:
            setpoint_x = setpoint
            setpoint_y = time_data
            
        self.__voltage_line = self.__ax_vol.plot(time_data, voltage_data, color="green", alpha=.25)
        self.__setpoint_line = self.__ax_vel.plot(setpoint_y, setpoint_x, color="blue", linestyle=":")
        self.__velocity_line = self.__ax_vel.plot(time_data, velocity_data, color="blue")
        
        #set up axis titles
        self.__ax_vel.set_ylabel("Velocity (RPM)")
        self.__ax_vol.set_ylabel("Voltage (mV)")
        self.__ax_vol.set_xlabel("Time (ms)")
        
        #set axis default colors
        for tl in self.__ax_vel.get_yticklabels():
            tl.set_color("blue")
        
        for tl in self.__ax_vol.get_yticklabels():
            tl.set_color("green")
        
        #add legend for lines
        self.__graph.legend(
            [
                Line2D([0], [0], color="blue"),
                Line2D([0], [0], color='blue', linestyle=":"), 
                Line2D([0], [0], color="green")
            ], 
            ["Actual Velocity", "Setpoint", "Actual Voltage"],
            loc=7, bbox_to_anchor=(1.4, .75))
        self.__graph.subplots_adjust(right=1)   
        
        #add legend for constants
        constants_text = "kP : " + str(parameters.get("kP")) + "\n"
        constants_text += "kI : " + str(parameters.get("kI")) + "\n"
        constants_text += "kD: " + str(parameters.get("kD")) + "\n"
        constants_text += "Integral Max: " + str(parameters.get("I_max")) + "\n"
        constants_text += "\n"
        constants_text += "Brakemode: " + parameters.get("brakemode") + "\n"
        constants_text += "Gearset: " + parameters.get("gearset") + "\n"
        constants_text += "Slew Rate (mV/ms): " + str(parameters.get("slew")) + "\n"
        
        self.__graph.text(
            1.13, 
            .25,
            constants_text,
            )
        self.__graph.subplots_adjust(right=1)   
        
        
        #add title
        self.__graph.suptitle("Velocity and Voltage vs Time - PID Tuning")


        
    def set_velocity_color(self, color):
        """
        sets the color of the velocity line on the graph
        """
        self.__setpoint_line.set_color(color)
        self.__velocity_line.set_color(color)
        
        for tl in self.__ax_vel.get_yticklabels():
            tl.set_color(color)
            
            
    def set_voltage_color(self, color):
        """
        sets the color of the voltage line on the graph
        """
        self.__voltage_line.set_color(color)
        
        for tl in self.__ax_vol.get_yticklabels():
            tl.set_color(color)
        
        
    def get_graph(self):
        """
        returns the graph object so that it can be saved or manipulated
        """
        return self.__graph


#unit test
#
#import numpy as np
#
#time = range(50)
#velocity = [20 * np.sin(x) for x in time]
#voltage = [x * 60 for x in velocity]
#pid = {"kP":1.0,"kI":.001,"kD":.25,"kI_max":5,"brakemode":"Brake","gearset":"18:1","slew":400}
#x = DebugGraph(time, velocity, voltage, 12, pid)
#x.get_graph().show()
#x.get_graph().savefig("test.png", bbox_inches='tight')

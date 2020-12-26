#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Dec 29 12:38:16 2019

@author: aiden
"""
import plotly.graph_objects as go
from plotly.subplots import make_subplots
import numpy as np
import math

class DebugGraph:
    """
    class for making a graph for debugging PID data
    """
    def __init__(self, data_dict, parameters):
        self.time_data = data_dict.get("time")
        self.vel_data = data_dict.get("velocity")
        self.vol_data = data_dict.get("voltage")
        self.vel_sp = data_dict.get("vel_sp")
        self.heading_sp = data_dict.get("heading_sp")
        self.heading_data = data_dict.get("heading_data")
        self.position_sp = data_dict.get("position_sp")
        self.position_r_data = data_dict.get("position_r")
        self.position_l_data = data_dict.get("position_l")
        self.integral_data = data_dict.get("integral")
        self.correction_data = data_dict.get("correction")
        
        #add legend for constants
        self.constants_text = "kP : " + str(parameters.get("kP")) + "\n"
        self.constants_text += "kI : " + str(parameters.get("kI")) + "\n"
        self.constants_text += "kD: " + str(parameters.get("kD")) + "\n"
        self.constants_text += "Integral Max: " + str(parameters.get("I_max")) + "\n"
        self.constants_text += "\n"
        self.constants_text += "Brakemode: " + str(parameters.get("brakemode")) + "\n"
        self.constants_text += "Gearset: " + str(parameters.get("gearset")) + "\n"
        self.constants_text += "Slew Rate (mV/ms): " + str(parameters.get("slew")) + "\n"
  
    def __to_in_per_sec2(self, degrees_per_ms2, wheel_diameter=3.25):
        circumference = math.pi * wheel_diameter
        revolutions_per_ms2 = degrees_per_ms2 / 360
        linear_distance = revolutions_per_ms2 * circumference
        in_per_sec2 = linear_distance * 1000  # convert ms to sec
        return in_per_sec2
    
    def make_graph(self, y1, y2=None, track_y1_sp=True, track_y2_sp=True):
        """
        returns a graph object with given axis parameters
        """
        title = "PID Tuning - "
        x = self.time_data
        if y1 == "velocity":
            y1_data = [self.vel_data.get("back_right"), self.vel_data.get("front_right"), self.vel_data.get("back_left"), self.vel_data.get("front_left")]
            y1_sp = self.vel_sp
            y1_title = "Velocity (RPM)"
            name1 = ["Back Right Velocity", "Front Right Velocity", "Back Left Velocity", "Front Left Velocity"]
            title += "Velocity"
        elif y1 == "voltage":
            y1_data = [self.vol_data.get("back_right"), self.vol_data.get("front_right"), self.vol_data.get("back_left"), self.vol_data.get("front_left")]
            y1_sp = []
            y1_title = "Voltage (mV)"
            name1 = ["Back Right Voltage", "Front Right Voltage", "Back Left Voltage", "Front Left Voltage"]
            title += "Voltage"
        elif y1 == "heading":
            y1_data = [self.heading_data]
            y1_sp = self.heading_sp
            y1_title = "Relative Heading (Degrees)"
            name1 = ["Relative Heading of Robot"]
            title += "Relative Heading of Robot"
        elif y1 == "position":
            y1_data = [self.position_l_data, self.position_r_data]
            y1_sp = self.position_sp
            y1_title = "Position"
            name1 = ["Position of Right Sensor", "Position of Left Sensor"]
            title += "Position of Sensor"
        elif y1 == "integral":
            y1_data = [self.integral_data]
            y1_title = "Integral"
            y1_sp = []
            name1 = ["Integral Value"]
            title += "Integral"
        elif y1 == "correction":
            y1_data = [self.correction_data]
            y1_title = "correction"
            y1_sp = []
            name1 = ["Correction"]
            title += "Correction"
        elif y1 == "acceleration":
            vel_data_l = np.diff(self.position_l_data) / np.diff(x)
            vel_data_r = np.diff(self.position_r_data) / np.diff(x)
            accel_data_l = []
            accel_data_r = []
            for l, r in zip(np.diff(vel_data_l) / np.diff(x[:-1]), np.diff(vel_data_r) / np.diff(x[:-1])):
                accel_data_l.append(self.__to_in_per_sec2(l))
                accel_data_r.append(self.__to_in_per_sec2(r))
            y1_data = [accel_data_l, accel_data_r]
            x = x[:-2]
            y1_title = "Acceleration"
            y1_sp = []
            name1 = ["Acceleration of Right Sensor", "Acceleration of Left Sensor"]
            title += "Acceleration"
        else:
            y1_data = []
            y1_sp = []
            y1_title = ""
            name1 = ""
            
            
        if y2 == "velocity":
            y2_data = [self.vel_data.get("back_right"), self.vel_data.get("front_right"), self.vel_data.get("back_left"), self.vel_data.get("front_left")]
            y2_sp = self.vel_sp
            y2_title = "Velocity (RPM)"
            name2 = ["Back Right Velocity", "Front Right Velocity", "Back Left Velocity", "Front Left Velocity"]
            title += "Velocity"
        elif y2 == "voltage":
            y2_data = [self.vol_data.get("back_right"), self.vol_data.get("front_right"), self.vol_data.get("back_left"), self.vol_data.get("front_left")]
            y2_sp = []
            y2_title = "Voltage (mV)"
            name2 = ["Back Right Voltage", "Front Right Voltage", "Back Left Voltage", "Front Left Voltage"]
            title += "Voltage"
        elif y2 == "heading":
            y2_data = [self.heading_data]
            y2_sp = self.heading_sp
            y2_title = "Relative Heading (Degrees)"
            name2 = ["Relative Heading of Robot"]
            title += "Relative Heading of Robot"
        elif y2 == "position":
            y2_data = [self.position_l_data, self.position_r_data]
            y2_sp = self.position_sp
            y2_title = "Position"
            name2 = ["Position of Right Sensor", "Position of Left Sensor"]
            title += "Position of Sensor"
        elif y2 == "integral":
            y2_data = [self.integral_data]
            y2_sp = []
            name2 = ["Integral Value"]
            title += "Integral"
        else:
            y2_data = []
            y2_sp = []
            y2_title = ""
            name2 = ""
            
        if y2_data:
            plot = make_subplots(specs=[[{"secondary_y": True}]])
        else:
            plot = go.Figure()

        for data, name in zip(y1_data, name1):
            plot.add_trace(
                go.Scatter(
                    x=x, 
                    y=data, 
                    mode="lines", 
                    line={'dash': 'solid', 'color': 'blue'},
                    name=name,
                    yaxis="y1",
                )
            )
        if track_y1_sp:
            plot.add_trace(
                go.Scatter(
                    x=x, 
                    y=y1_sp, 
                    mode="lines", 
                    line={'dash': 'dash', 'color': 'blue'},
                    name="Setpoint",
                    yaxis="y1",
                )
            )
            
        
        if y2_data:
            for data, name in zip(y2_data, name2):
                plot.add_trace(
                    go.Scatter(
                        x=x, 
                        y=data, 
                        mode="lines", 
                        line={'dash': 'solid', 'color': 'green'},
                        name=name,
                    ), 
                    secondary_y=True
                )
            if track_y2_sp:
                plot.add_trace(
                    go.Scatter(
                        x=x, 
                        y=y2_sp, 
                        mode="lines", 
                        line={'dash': 'dash', 'color': 'green'},
                        name="Setpoint",
                    ),
                    secondary_y=True
                )
        
        title += " vs Time"
        plot.update_layout(
            title=title,
            xaxis=dict(
                title="Time (ms)"
            ),
            font=dict(
                family="Courier New, monospace",
                size=14,
                color="#7f7f7f"
            ),
            showlegend=True,
            legend=dict(x=1.2, y=1.1),
            plot_bgcolor="rgb(255, 255, 255)"
        )
        
        
        plot.update_xaxes(
            tickangle=-45, 
            showgrid=False, 
            mirror=True,
            ticks='outside',
            showline=True

        )
        plot.update_yaxes(
            tickangle=0, 
            showgrid=False, 
            zeroline=True, 
            zerolinewidth=2, 
            zerolinecolor="black",
            mirror=True,
            ticks='outside',
            showline=True

        )
        
        if y2_data:
            plot.update_yaxes(title_text=y1_title, secondary_y=False, color="blue")
            plot.update_yaxes(title_text=y2_title, secondary_y=True, color="green")
        else:
            plot.update_yaxes(title_text=y1_title)
            pass

        return plot


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

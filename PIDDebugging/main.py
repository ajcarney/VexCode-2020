#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Jan  5 17:13:31 2020

@author: aiden
"""
import sys

import parser
import graph

if len(sys.argv) == 1:
    file = input("enter file to parse: ")
else:
    file = sys.argv[1]

parser.gen_sample_data()    
p = parser.Parser()
p.parse_file(file)

g = graph.DebugGraph(
    p.get_data()["time"],
    p.get_data()["velocity"],
    p.get_data()["voltage"],            
    p.get_data()["setpoint"],
    {
        "kP":p.get_data()["pid_constants"]["kP"],
        "kI":p.get_data()["pid_constants"]["kI"],
        "kD":p.get_data()["pid_constants"]["kD"],
        "I_max":p.get_data()["pid_constants"]["I_max"],
        "brakemode":p.get_data()["brakemode"],
        "gearset":p.get_data()["gearset"],
        "slew":p.get_data()["slew_rate"]
    }
    )  

g.get_graph().show()   
g.get_graph().savefig("test2.png", bbox_inches='tight')



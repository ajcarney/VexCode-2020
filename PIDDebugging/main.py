#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Jan  5 17:13:31 2020

@author: aiden
"""
import streamlit as st

import data_parser
import graph

# if len(sys.argv) == 1:
#     file = input("enter file to parse: ")
# else:
#     file = sys.argv[1]
file = "./log.txt"

# parser.gen_sample_data()    
p = data_parser.Parser()
p.parse_file(file)
p.print_data()
g = graph.DebugGraph(
    p.get_data(),
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
y1 = st.sidebar.selectbox("Y1 data", ["velocity", "voltage", "heading", "position", "acceleration"], 1)
track_y1 = st.sidebar.checkbox("Graph Y1 setpoint")
y2 = st.sidebar.selectbox("Y2 data", ["velocity", "voltage", "heading", "position", "None"], 4)
track_y2 = st.sidebar.checkbox("Graph Y2 setpoint")
plot = g.make_graph(y1, y2, track_y1, track_y2)
st.plotly_chart(plot, use_container_width=True)

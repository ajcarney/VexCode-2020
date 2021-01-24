#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Jan 22 12:49:32 2021

@author: aiden
"""

tower_colors = ["blue", "blue", "red"] # top, middle, bottom
indexer_colors = ["red", "none", "none"] #top, middle, bottom

final_tower = ["red", "red", "none"]

num_balls_to_cycle = 0
tower_initial = [i for i in tower_colors if i != "none"]
indexer_initial = [i for i in tower_colors if i != "none"]
cycle = indexer_colors + tower_colors 
while cycle[3:] != final_tower:
    num_balls_to_cycle += 1
    cycle.insert(0, cycle.pop()) 
    in_tower = cycle[3:]
    in_indexer = cycle[:3]
    print(num_balls_to_cycle, in_indexer, in_tower)

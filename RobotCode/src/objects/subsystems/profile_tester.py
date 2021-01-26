#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Jan 17 12:47:59 2021

@author: aiden
"""


import matplotlib.pyplot as plt

def gen_profile(enc_ticks, max_acceleration, max_decceleration, max_velocity, initial_velocity):
    profile = [initial_velocity]
    
    i = 0
    while(i < enc_ticks):
        ticks_left = enc_ticks - i
        ticks_to_deccelerate = profile[i] / max_decceleration
        if(ticks_to_deccelerate < ticks_left):
            step = (profile[i] + max_acceleration(i))
            if(step > max_velocity):
                step = max_velocity
            profile.append(step)
        else:
            profile.append((profile[i] - max_decceleration))
            
        i += 1
            
    return profile

def accel_profile(x):
    vel = .005 * x
    print(vel)
    return vel

ticks = 1000

y = gen_profile(ticks, accel_profile, 0.8, 450, 50)
x = list(range(ticks + 1))
print(len(y))
plt.scatter(x, y)
plt.show()
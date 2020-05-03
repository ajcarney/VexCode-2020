#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Oct 13 21:20:16 2019

@author: aiden
"""

import time
import pyautogui

time.sleep(.5)

pyautogui.typewrite("/**")
pyautogui.press("enter")

pyautogui.typewrite(" * how_function_works_line_1")
pyautogui.press("enter")

pyautogui.typewrite("* how_function_works_line_2")
pyautogui.press("enter")

pyautogui.typewrite("* how_function_works_line_3")
pyautogui.press("enter")


pyautogui.typewrite("*/")
pyautogui.press("enter")

pyautogui.press("backspace")

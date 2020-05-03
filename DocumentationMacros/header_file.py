#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Oct 13 20:50:03 2019

@author: aiden
"""

import time
import pyautogui

time.sleep(.5)

pyautogui.typewrite("/**")
pyautogui.press("enter")

pyautogui.typewrite(" * @file:")
pyautogui.press("enter")

pyautogui.typewrite("* @author:")
pyautogui.press("enter")

pyautogui.typewrite("* @reviewed_on:")
pyautogui.press("enter")

pyautogui.typewrite("* @reviewed_by:")
pyautogui.press("enter")

pyautogui.typewrite("* TODO:")
pyautogui.press("enter")

pyautogui.typewrite("*")
pyautogui.press("enter")

pyautogui.typewrite("* description_of_contents_line_1")
pyautogui.press("enter")

pyautogui.typewrite("* description_of_contents_line_2")
pyautogui.press("enter")

pyautogui.typewrite("* description_of_contents_line_3")
pyautogui.press("enter")

pyautogui.typewrite("*")
pyautogui.press("enter")

pyautogui.typewrite("*/")
pyautogui.press("enter")

pyautogui.press("backspace")

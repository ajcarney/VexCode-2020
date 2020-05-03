#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Oct 13 21:23:07 2019

@author: aiden
"""

import time
import pyautogui

time.sleep(.5)

pyautogui.typewrite("/**")
pyautogui.press("enter")

pyautogui.typewrite(" * @see:")
pyautogui.press("enter")

pyautogui.typewrite("* @see:")
pyautogui.press("enter")

pyautogui.typewrite("*")
pyautogui.press("enter")

pyautogui.typewrite("* purpose_of_class_line_1")
pyautogui.press("enter")

pyautogui.typewrite("* purpose_of_class_line_2")
pyautogui.press("enter")

pyautogui.typewrite("* purpose_of_class_line_3")
pyautogui.press("enter")

pyautogui.typewrite("*/")
pyautogui.press("enter")

pyautogui.press("backspace")

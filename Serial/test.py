#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Aug  6 14:30:19 2020

@author: aiden
"""


from kivy.app import App
from kivy.properties import StringProperty
from kivy.uix.floatlayout import FloatLayout
from kivy.clock import Clock
import random
import time as t

class MainScreen(FloatLayout):
    pass

class TestApp(App):
    time = ""

    def update(self, *args):
        self.time = str(t.asctime()) # + 'time'?

    def build(self):
        Clock.schedule_interval(self.update, 1)
        return MainScreen()

    
    
if __name__ == "__main__":
    TestApp().run()



#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Jul 31 14:01:59 2020

@author: aiden
"""

import kivy
from kivy.app import App
from kivy.clock import Clock
from kivy.uix.label import Label
from kivy.uix.gridlayout import GridLayout
from kivy.uix.textinput import TextInput
from kivy.uix.button import Button
from kivy.uix.widget import Widget
from kivy.properties import ObjectProperty
from kivy.uix.tabbedpanel import TabbedPanel
from kivy.uix.floatlayout import FloatLayout
from kivy.uix.popup import Popup
from kivy.properties import StringProperty
from kivy.event import EventDispatcher


class Data:
    __instance = None 
    
    motors = {
        0:"Front Right",    
        1:"Front Left",    
        2:"Back Right",    
        3:"Back Left",    
        4:"Main Intake",    
        5:"Hoarding Intake",    
        6:"Lift",    
    }
        
    @staticmethod 
    def get_instance():
       """ Static access method. """
       if Data.__instance == None:
          Data()
       return Data.__instance
   
    def __init__(self):
        if Data.__instance != None:
            raise RuntimeError("Constructor has already been called and exists at " + str(Data.__instance))
        else:
            Data.__instance = self
            self.motors_data = {}
            
    def api_interaction(self, byte1, byte2, msg):
        return None
            
    def retrieve_motor_data(self, *args):
        data = {}
        for i, motor in self.motors.items():
            motor_data = {
                "Actual Velocity":self.api_interaction(0xA0, 0xA0, i),
                "Actual Voltage":self.api_interaction(0xA0, 0xA1, i),
                "Current Draw":self.api_interaction(0xA0, 0xA2, i),
                "Encoder Position":self.api_interaction(0xA0, 0xA3, i),
                "Brakemode":self.api_interaction(0xA0, 0xA4, i),
                "Gearset":self.api_interaction(0xA0, 0xA5, i),
                "Port":self.api_interaction(0xA0, 0xA6, i),
                "PID Constants":self.api_interaction(0xA0, 0xA7, i),
                "Slew Rate":self.api_interaction(0xA0, 0xA8, i),
                "Power":self.api_interaction(0xA0, 0xA9, i),
                "Temperature":self.api_interaction(0xA0, 0xAA, i),
                "Torque":self.api_interaction(0xA0, 0xAB, i),
                "Direction":self.api_interaction(0xA0, 0xAC, i),
                "Efficiency":self.api_interaction(0xA0, 0xAD, i),
                "Is Stopped":self.api_interaction(0xA0, 0xAE, i),
                "Is Reversed":self.api_interaction(0xA0, 0xAF, i),
                "Is Registered":self.api_interaction(0xA1, 0xA0, i)
            }
            
            data.update({i:motor_data})
        self.motors_data = data


class Settings:
    __instance = None 
    
    @staticmethod 
    def get_instance():
       """ Static access method. """
       if Settings.__instance == None:
          Settings()
       return Settings.__instance
   
    def __init__(self):
        if Settings.__instance != None:
            raise RuntimeError("Constructor has already been called and exists at " + str(Settings.__instance))
        else:
            Settings.__instance = self
            self.ip_address = "127.0.0.1"      
            self.motor_dashboard_selected = "Actual Velocity"
            self.motor_selected = 0
            
    def update_motor_dashboard_selected(self, new_value):
        self.motor_dashboard_selected = new_value
        
    def update_motor_selected(self, new_value):
        self.motor_selected = new_value
        if self.motor_selected not in Data.motors.keys():
            self.motor_selected = 0



class SettingsPopup(FloatLayout):        
    def update_ip_addr(self, new_ip):
        print("New IP set: ", new_ip)



class MainScreen(FloatLayout):    
    
    motor_title_label_text = StringProperty("")
    
    def __init__(self):
        super(MainScreen, self).__init__()
    
    def open_settings(self):
        s = SettingsPopup()
        popup_window = Popup(
            title="Settings", 
            content=s, 
            size_hint=(None,None),
            size=(self.width / 2, self.height / 2)
        )
        
        popup_window.open()
        
    @classmethod
    def update_motor_info_labels(cls):
        # self.ids.get("label0").text = str(Data.get_instance().motors_data.get(0, {}).get(App.get_running_app().settings.motor_dashboard_selected))
        # self.ids.get("label1").text = str(Data.get_instance().motors_data.get(1, {}).get(App.get_running_app().settings.motor_dashboard_selected))
        # self.ids.get("label2").text = str(Data.get_instance().motors_data.get(2, {}).get(App.get_running_app().settings.motor_dashboard_selected))
        # self.ids.get("label3").text = str(Data.get_instance().motors_data.get(3, {}).get(App.get_running_app().settings.motor_dashboard_selected))
        # self.ids.get("label4").text = str(Data.get_instance().motors_data.get(4, {}).get(App.get_running_app().settings.motor_dashboard_selected))
        # self.ids.get("label5").text = str(Data.get_instance().motors_data.get(5, {}).get(App.get_running_app().settings.motor_dashboard_selected))
        # self.ids.get("label6").text = str(Data.get_instance().motors_data.get(6, {}).get(App.get_running_app().settings.motor_dashboard_selected))
        
        cls.motor_title_label_text = str(Data.get_instance().motors.get(Settings.get_instance().motor_selected))


class VexServer(App):
    settings = Settings.get_instance()
    l0 = StringProperty("") 
    l1 = StringProperty("") 
    l2 = StringProperty("") 
    l3 = StringProperty("") 
    l4 = StringProperty("") 
    l5 = StringProperty("") 
    l6 = StringProperty("") 
    motor_data_title = StringProperty("")
    motor_data_body = StringProperty("")
    

    def update_motor_data(self, *args):
        self.l0 = str(Data.get_instance().motors_data.get(0, {}).get(self.settings.motor_dashboard_selected))
        self.l1 = str(Data.get_instance().motors_data.get(1, {}).get(self.settings.motor_dashboard_selected))
        self.l2 = str(Data.get_instance().motors_data.get(2, {}).get(self.settings.motor_dashboard_selected))
        self.l3 = str(Data.get_instance().motors_data.get(3, {}).get(self.settings.motor_dashboard_selected))
        self.l4 = str(Data.get_instance().motors_data.get(4, {}).get(self.settings.motor_dashboard_selected))
        self.l5 = str(Data.get_instance().motors_data.get(5, {}).get(self.settings.motor_dashboard_selected))
        self.l6 = str(Data.get_instance().motors_data.get(6, {}).get(self.settings.motor_dashboard_selected))

        self.motor_data_title = str(Data.get_instance().motors.get(self.settings.motor_selected))
        body_text = ""
        for key, value in Data.get_instance().motors_data.get(self.settings.motor_selected, {}).items():
            body_text += key + ": " + str(value) + '\n'
        print(self.motor_data_body)
        self.motor_data_body = body_text

    def build(self):
        Clock.schedule_interval(self.update_motor_data, 0.1)
        Clock.schedule_interval(Data.get_instance().retrieve_motor_data, 0.01)
        return MainScreen()


def mainloop(dt):
    # VexServer.set_more_info_labels(App.get_running_app().settings.motor_selected)
    # VexServer.set_comparison_labels(App.get_running_app().settings.motor_dashboard_selected)
    # print(App.get_running_app().l0)
    MainScreen.update_motor_info_labels()

        

if __name__ == "__main__":
    VexServer().run()
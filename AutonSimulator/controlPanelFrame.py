#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import tkinter as tk
import time

class controlPanelFrame:
    """
    makes frame object for all labels and buttons
    """
    def __init__(self, master, runningFrame):
        self.runningLabelText = runningFrame.runningLabelText
        self.master = master

        self.guiFrame = tk.Frame(self.master)
        self.guiFrame.grid(row=1, column=0, sticky='news')

        self.speed = 7
        self.keepRunning = True

        self.options = {
            "True": "running",
            "False":"paused "
            }

        self.pauseButtonTextOptions = {
            "True": "pause ",
            "False":"resume"
             }


        ##### labels #####

        #speed label
        self.speedLabelText = tk.StringVar()
        self.speedLabel = tk.Label(self.guiFrame, textvariable=self.speedLabelText)


        ##### buttons #####

        self.fasterButton = tk.Button(self.guiFrame, text=' + ', command=self.__faster)
        self.slowerButton = tk.Button(self.guiFrame, text=' - ', command=self.__slower)

        self.pauseButtonText = tk.StringVar()
        self.pauseButton = tk.Button(self.guiFrame, textvariable=self.pauseButtonText, command=self.__pause)
        self.master.bind("<space>", self.__pause)




    def __changeSpeedLabel(self):
        self.speedLabelText.set("speed: " + str(self.speed))



    def __faster(self):
        if self.speed < 10:
            self.speed = self.speed + 1
            self.__changeSpeedLabel()

    def __slower(self):
        if self.speed > 1:
            self.speed = self.speed - 1
            self.__changeSpeedLabel()

    def __pause(self, event=None):
        """
        pauses robot actions
        """
        self.keepRunning = not self.keepRunning

        self.runningLabelText.set(self.options.get(str(self.keepRunning)))
        self.pauseButtonText.set(self.pauseButtonTextOptions.get(str(self.keepRunning)))

        self.master.update()


    def idle(self):
        """
        sends canvas into idle state
        """
        while 1:
            time.sleep(0.1)
            self.master.update()

    def disable(self):
        self.pauseButton.config(state="disabled")
        self.fasterButton.config(state="disabled")
        self.slowerButton.config(state="disabled")

        self.master.unbind("<space>")

    def placeObjects(self):
        """
        places all buttons and labels
        """

        self.pauseButton.grid(row=0, column=2, columnspan=4, rowspan=2, sticky='news', ipadx=30)

        self.speedLabel.grid(row=2, column=2, columnspan=4, rowspan=1, sticky='news')

        self.slowerButton.grid(row=0, column=0, rowspan=3, columnspan=2, sticky='news')
        self.fasterButton.grid(row=0, column=6, rowspan=3, columnspan=2, sticky='news')


        #sets default value of labels
        self.pauseButtonText.set(self.pauseButtonTextOptions.get(str(self.keepRunning)))
        self.speedLabelText.set("speed: " + str(self.speed))
        self.runningLabelText.set(self.options.get(str(self.keepRunning)))



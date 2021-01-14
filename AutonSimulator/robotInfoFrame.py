#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import tkinter as tk

class robotInfoFrame:
    """
    makes frame object for all labels and buttons
    """
    def __init__(self, master):
        self.master = master

        self.guiFrame = tk.Frame(self.master)
        self.guiFrame.grid(row=1, column=2, sticky='news')


        ##### labels #####
        
        # position label
        self.positionLabelText = tk.StringVar()
        self.positionLabel = tk.Label(self.guiFrame, textvariable=self.positionLabelText)

        #orientation label
        self.orientationLabelText = tk.StringVar()
        self.orientationLabel = tk.Label(self.guiFrame, textvariable=self.orientationLabelText)

        #distance moved label
        self.distanceMovedLabelText = tk.StringVar()
        self.distanceMovedLabel = tk.Label(self.guiFrame, textvariable=self.distanceMovedLabelText)

        #current command label
        self.commandLabelText = tk.StringVar()
        self.commandLabel = tk.Label(self.guiFrame, textvariable=self.commandLabelText)

        #white space
        self.whiteSpaceLabel = tk.Label(self.guiFrame)

        #trailing white space
        self.trailingWhiteSpaceLabel = tk.Label(self.guiFrame)




    def placeObjects(self):
        """
        places all labels and sets default value
        """
        self.positionLabel.grid(row=0, column=8, columnspan=1, sticky='w')
        self.orientationLabel.grid(row=1, column=8, columnspan=1, sticky='w')
        self.distanceMovedLabel.grid(row=2, column=8, columnspan=1, sticky='w')
        self.commandLabel.grid(row=3, column=8, columnspan=1, sticky='w')

        self.whiteSpaceLabel.grid(row=0, column=0, columnspan=7, rowspan=3, sticky='news')
        self.trailingWhiteSpaceLabel.grid(row=0, column=4, sticky='news')


        #configures trailing white space to be eaten
        self.guiFrame.grid_columnconfigure(4, weight=1)

        self.positionLabelText.set("(x: ?, y: ?)")
        self.orientationLabelText.set("orientation: ")
        self.distanceMovedLabelText.set("distance moved: N/A                   ")
        self.commandLabelText.set("command: ")






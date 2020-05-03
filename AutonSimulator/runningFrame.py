#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import tkinter as tk

class runningFrame:
    """
    makes frame for running label
    """
    def __init__(self, master):
        self.master = master

        self.guiFrame = tk.Frame(self.master)
        self.guiFrame.grid(row=2, column=0, sticky='news')


        ##### labels #####

        #running label
        self.runningLabelText = tk.StringVar()
        self.runningLabel = tk.Label(self.guiFrame, textvariable=self.runningLabelText, font=("Courier", 15))



    def placeObjects(self):
        """
        places labels
        """

        self.runningLabel.grid(sticky='w', row=0, column=0)

        self.guiFrame.grid_columnconfigure(0, weight=1)

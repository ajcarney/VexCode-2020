#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import time

class auton:
    """
    contains autonomous commands
    """
    def __init__(self, robotObj, master, controlPanelFrame, robotInfoFrame):
        self.controlPanelFrame = controlPanelFrame
        self.robotInfoFrame = robotInfoFrame
        self.robot = robotObj
        self.master = master

        self.orientationLabelText = self.robotInfoFrame.orientationLabelText
        self.orientationLabelText.set("orientation: " + str(self.robot.orientationDegrees))

        self.controlPanelFrame.runningLabelText.set(self.controlPanelFrame.options.get(str(self.controlPanelFrame.keepRunning)))



    def nextFrame(self, waitTime=0):
        """
        updates tkinter canvas and allows for a wait
        """

        self.master.update()

        timeSlept = 0
        while timeSlept <= waitTime:
            if self.controlPanelFrame.keepRunning:
                time.sleep(0.1)
                timeSlept += 0.1
                self.master.update()
            else: #sets robot to idle
                time.sleep(0.1)
                self.master.update()



    def intakeStart(self, rotations):
        """
        for translator
        """
        pass

    def outakeStart(self, rotations):
        """
        for translator
        """
        pass

    def intakeEnd(self, Time):
        """
        for translator
        """
        pass

    def catapult(self, rotations):
        """
        for translator
        """
        pass

    def capFlipper(self, rotations):
        """
        for translator
        """
        pass


    def commands(self):
        """
        autonomous commands
        """
        self.nextFrame(1)
    
        self.robot.drive_to_point(0, 27)
        self.nextFrame(.25)
        
        self.robot.drive_to_point(27.2, 5.5, 1)
        self.nextFrame(.25)
        
        self.robot.drive_to_point(0, 27)
        self.nextFrame(.25)
        
        self.robot.drive_to_point(-22.5, 4.8, 1)
        self.nextFrame(.25)
        
        # self.robot.forward(350)
        # self.nextFrame(.25)
        
        #self.robot.turnLeft(10)
        #self.nextFrame(.25)

        
        # self.robot.drive_to_point(0, 30)
        # self.nextFrame(.25)
        
     
        # self.robot.drive_to_point(0, 0)
        # self.nextFrame(1)
        

        

        

######################### unit test #########################
#        import random                                      #
#        for x in range(0,4):                               #
#            self.robot.forward(500)                        #
#            self.nextFrame(1)                              #
#                                                           #
#            self.robot.turnLeft(90)                        #
#            self.nextFrame(1)                              #
#                                                           #
#        self.robot.turnLeft(45)                            #
#        self.nextFrame(1)                                  #
#                                                           #
#        for x in range(0,4):                               #
#            self.robot.forward(400)                        #
#            self.nextFrame(1)                              #
#                                                           #
#            self.robot.turnRight(90)                       #
#            self.nextFrame(1)                              #
#                                                           #
#        self.robot.turnRight(45)                           #
#        self.nextFrame(1)                                  #
#                                                           #
#                                                           #
#        for x in range(0, 6):                              #
#            self.robot.forward(500)                        #
#            self.nextFrame(1)                              #
#                                                           #
#            self.robot.turnRight(random.randint(10,90))    #
#            self.nextFrame(1)                              #
#                                                           #
#############################################################




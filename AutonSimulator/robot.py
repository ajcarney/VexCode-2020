#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from PIL import Image
import io
import time
import math
import stopwatch


class robot:
    """
    contains all robot move functions
    """
    def __init__(self, fieldSize=None, tkobj=None, canvas=None, diameterOfWheel=4.1, controlPanelFrame=None, robotInfoFrame=None):
        self.controlPanelFrame = controlPanelFrame
        self.robotInfoFrame = robotInfoFrame

        self.canvas = canvas
        self.master = tkobj

        self.diameterOfWheel = diameterOfWheel
        self.fieldSize = fieldSize
        self.reversed = 0

        self.sqaure = None
        self.line = None

        self.squareVertexes = []
        self.lineVertexes = []

        self.iteration = 0

    def __calcSleepTime(self, distance, iterations):
        return (distance / (10*(2 ** self.controlPanelFrame.speed))) / iterations


    def __generate_postscript(self):
        # ps = self.canvas.postscript(colormode="color")
        # im = Image.open(io.BytesIO(ps.encode('utf-8')))
        # im.save("ps/test" + str(self.iteration) + ".jpg")
        # self.iteration += 1
        pass

    def __calcCenters(self):
        """
        returns center of each polygon
        """
        xVals = []
        yVals = []

        for i in range(0, len(self.squareVertexes)): #calculates square center
            xVals.append(self.squareVertexes[i][0])
            yVals.append(self.squareVertexes[i][1])

        xVals.sort(reverse=True)
        yVals.sort(reverse=True)

        greatestX = xVals[0]
        greatestY = yVals[0]

        xVals.sort()
        yVals.sort()

        leastX = xVals[0]
        leastY = yVals[0]

        x = abs(((greatestX - leastX)/2)) + leastX
        y = abs(((greatestY - leastY)/2)) + leastY

        center = [x, y]


        return center





    def __rotate(self, Angle, pivotPoint):
        """
        rotates the robot simulating one side of the
        chassis moving
        """
        center = (pivotPoint[0], pivotPoint[1])
        angle = math.radians(Angle)
        cos_val = math.cos(angle)
        sin_val = math.sin(angle)
        cx, cy = center

        new_points = []
        for x_old, y_old in self.squareVertexes:
            x_old -= cx
            y_old -= cy
            x_new = x_old * cos_val - y_old * sin_val
            y_new = x_old * sin_val + y_old * cos_val
            new_points.append([x_new + cx, y_new + cy])


        angle = math.radians(Angle)
        cos_val = math.cos(angle)
        sin_val = math.sin(angle)
        cx, cy = center

        new_points2 = []
        for x_old, y_old in self.lineVertexes:
            x_old -= cx
            y_old -= cy
            x_new = x_old * cos_val - y_old * sin_val
            y_new = x_old * sin_val + y_old * cos_val
            new_points2.append([x_new + cx, y_new + cy])

        self.squareVertexes = new_points
        self.lineVertexes = new_points2




    def __rotateInPlace(self, Angle):
        """
        rotates the robot in place simulating both sides of the chassis
        moving
        """
        center = self.__calcCenters()


        center = (center[0], center[1])

        angle = math.radians(Angle) #moves square
        cos_val = math.cos(angle)
        sin_val = math.sin(angle)
        cx, cy = center
        new_points = []
        for x_old, y_old in self.squareVertexes:
            x_old -= cx
            y_old -= cy
            x_new = x_old * cos_val - y_old * sin_val
            y_new = x_old * sin_val + y_old * cos_val
            new_points.append([x_new + cx, y_new + cy])

        angle = math.radians(Angle) #moves line
        cos_val = math.cos(angle)
        sin_val = math.sin(angle)
        cx, cy = center
        new_points2 = []
        for x_old, y_old in self.lineVertexes:
            x_old -= cx
            y_old -= cy
            x_new = x_old * cos_val - y_old * sin_val
            y_new = x_old * sin_val + y_old * cos_val
            new_points2.append([x_new + cx, y_new + cy])


        self.squareVertexes = new_points
        self.lineVertexes = new_points2



    def __move(self, units):
        """
        simulates the robot moving in a straight line
        """
        quadrants = {  #quadrant: [xVal, yVal]
            1:[1, -1],
            2:[-1, -1],
            3:[-1, 1],
            4:[1, 1]
            }


        #determine quadrant of final position
        #used to determine if robot needs to add or subtract
        #x and y value to move in that direction

        if units > 0 and (self.orientationDegrees >= 0 and self.orientationDegrees < 90):
            quadrant = 1
        elif units < 0 and (self.orientationDegrees >= 180 and self.orientationDegrees <= 270):
            quadrant = 1

        elif units > 0 and (self.orientationDegrees >= 90 and self.orientationDegrees <= 180):
            quadrant = 2
        elif units < 0 and (self.orientationDegrees > 270 and self.orientationDegrees < 360):
            quadrant = 2

        elif units > 0 and (self.orientationDegrees >= 180 and self.orientationDegrees < 270):
            quadrant = 3
        elif units < 0 and (self.orientationDegrees >= 0 and self.orientationDegrees <= 90):
            quadrant = 3

        elif units > 0 and (self.orientationDegrees >= 270 and self.orientationDegrees < 360):
            quadrant = 4
        elif units < 0 and (self.orientationDegrees > 90 and self.orientationDegrees < 180):
            quadrant = 4



        vals = quadrants.get(quadrant)
        xPol = vals[0]
        yPol = vals[1]

        #absolute value simulates reference angle
        #trig functions of reference angles are positive
        x = xPol * abs(math.cos(math.radians(self.orientationDegrees)))
        y = yPol * abs(math.sin(math.radians(self.orientationDegrees)))

        d = (math.sqrt((x**2) + (y**2)))
        distanceMoved = 0

        stp = stopwatch.stopwatch()
        stp.start()

        iterations = int(abs(units)/d)
        for i in range(0, iterations): #move animation
            if not self.controlPanelFrame.keepRunning: #allows for pause
                while not self.controlPanelFrame.keepRunning:
                    time.sleep(0.1)
                    self.master.update()

            self.canvas.move(self.square, x, y)
            self.canvas.move(self.line, x, y)

            self.master.update()
            self.__generate_postscript()
            time.sleep(self.__calcSleepTime(self.__encoderTicks(abs(units)), iterations))

            eus = self.__encoderTicks(d)  # shows distance moved
            distanceMoved = distanceMoved + eus

            self.__updateDistanceLabel(str(round(distanceMoved, 2)) ,"encoder ticks")


        #updates vertices of square and line
        xChange = xPol * abs(math.cos(math.radians(self.orientationDegrees)) * abs(units))
        yChange = yPol * abs(math.sin(math.radians(self.orientationDegrees)) * abs(units))

        self.squareVertexes = [
            [self.squareVertexes[0][0] + xChange, self.squareVertexes[0][1] + yChange],
            [self.squareVertexes[1][0] + xChange, self.squareVertexes[1][1] + yChange],
            [self.squareVertexes[2][0] + xChange, self.squareVertexes[2][1] + yChange],
            [self.squareVertexes[3][0] + xChange, self.squareVertexes[3][1] + yChange]
            ]

        self.lineVertexes = [
            [self.lineVertexes[0][0] + xChange, self.lineVertexes[0][1] + yChange],
            [self.lineVertexes[1][0] + xChange, self.lineVertexes[1][1] + yChange],
            [self.lineVertexes[2][0] + xChange, self.lineVertexes[2][1] + yChange],
            [self.lineVertexes[3][0] + xChange, self.lineVertexes[3][1] + yChange]
            ]



    def __update(self):
        """
        updates tkinter canvas so robot can be seen moving
        and also allows for a pause
        """
        self.canvas.delete(self.square)
        self.canvas.delete(self.line)

        self.square = self.canvas.create_polygon(self.squareVertexes,
                                                 outline="black",
                                                 fill="#949596",
                                                 width=3)

        self.line = self.canvas.create_polygon(self.lineVertexes, width=3)


        if not self.controlPanelFrame.keepRunning: #allows for pause during turns
            while not self.controlPanelFrame.keepRunning:
                time.sleep(0.1)
                self.master.update()
                
        self.master.update()


    def __updateDistanceLabel(self, distance=0, units=""):
        """
        updates distance travelled label
        """
        text = "distance moved: " + str(distance) + " " + units
        while len(text) < 38:
            text = text + " "
        self.robotInfoFrame.distanceMovedLabelText.set(text)

        self.master.update()


    def __pixels(self, rotationUnits):
        """
        converts encoder ticks to pixels
        """
        revolutions = rotationUnits / 360
        inches = revolutions * (self.diameterOfWheel * math.pi)
        pixelsToMove = (inches * self.fieldSize) / 144
        print(rotationUnits, pixelsToMove)
        return pixelsToMove


    def __encoderTicks(self, pixels):
        """
        converts pixels to encoder ticks
        """
        inches = pixels * (144 / self.fieldSize)
        revolutions = inches / (self.diameterOfWheel * math.pi)
        encoderTicks = revolutions * 360

        return encoderTicks


    def inches(self, pixels):
        """
        converts pixels to inches
        """
        inches = pixels * (144 / self.fieldSize)

        return inches


    def show(self, angle=0, position=[[0, 0], [0, 0]]):
        """
        starting function that shows the robot based on two coordinates
        if more than two coordinates are given use different show function
        """

        #if len(position) > 2:
        if 1:
            x1 = position[0][0]
            y1 = position[0][1]
            x2 = position[1][0]
            y2 = position[1][1]

            self.orientationDegrees = 90
            self.squareVertexes = [
                [x1, y1],
                [x2, y1],
                [x2, y2],
                [x1, y2]
                ]

            self.sizeOfSquare = abs(self.squareVertexes[0][0] - self.squareVertexes[1][0])

            y2 = (y2 - y1) / 4
            self.lineVertexes = [
                [x1, y1+y2],
                [x2, y1+y2],
                [x2, y1+y2+4],
                [x1, y1+y2+4]
                ]

#        else:
#            x1 = position[0][0]
#            x2 = position[1][0]
#            x3 = position[2][0]
#            x4 = position[3][0]
#
#            y1 = position[0][1]
#            y2 = position[1][1]
#            y3 = position[2][1]
#            y4 = position[3][1]
#
#            self.squareVertexes = position
#            self.lineVertexes = []


        self.square = self.canvas.create_polygon(self.squareVertexes,
                                                 outline="black",
                                                 fill="#949596",
                                                 width=3)

        turn = 90 - (angle % 360)

        self.line = self.canvas.create_polygon(self.lineVertexes, width=3)
        self.__rotateInPlace(turn)
        self.__update()
        self.__generate_postscript()

        self.orientationDegrees = angle % 360




    def reverse(self):
        """
        reverses orientation of the robot
        """
        self.reversed = not(self.reversed)




    def forward(self, rotationUnits):
        """
        moves the robot forward and straight
        """
        self.robotInfoFrame.commandLabelText.set(("forward " + str(rotationUnits)))

        if self.reversed:
            rotationUnits = 0 - rotationUnits

        pixelsToMove = self.__pixels((rotationUnits))

        self.__move(pixelsToMove)
        self.__update()




    def backward(self, rotationUnits):
        """
        moves the robot backwards and straight
        """
        self.robotInfoFrame.commandLabelText.set(("backward " + str(rotationUnits)))

        rotationUnits = 0 - rotationUnits
        if self.reversed:
            rotationUnits = 0 - rotationUnits

        pixelsToMove = self.__pixels(rotationUnits)


        self.__move(pixelsToMove)
        self.__update()


    def leftSide(self, angle):
        """
        turns the robot right so that only one side is moving
        """
        self.robotInfoFrame.commandLabelText.set(("leftSide " + str(angle)))

        if self.reversed:
            angle = 0 - angle

        pivotPoints = self.squareVertexes[2]

        turned = 0
        orientation = angle / abs(angle) #to account for negative turns
        toMove = orientation * .5
        while turned < abs(angle): #turn to specified angle
            self.__rotate(toMove, pivotPoints)
            self.__update()
            self.__generate_postscript()
            
            time.sleep(self.__calcSleepTime(angle, angle))

            turned += .5
            self.__updateDistanceLabel(str(round(turned, 2)), "degrees")

            self.orientationDegrees = (self.orientationDegrees - toMove) % 360
            self.robotInfoFrame.orientationLabelText.set("orientation: " + str(self.orientationDegrees))



    def rightSide(self, angle):
        """
        turns the robot left so that only the right side is moving
        """
        self.robotInfoFrame.commandLabelText.set(("rightSide " + str(angle)))

        angle = 0 - angle
        if self.reversed:
            angle = 0 - angle

        pivotPoints = self.squareVertexes[3]

        turned = 0
        orientation = angle / abs(angle) #to account for negative turns
        toMove = (-1 * orientation * .5)
        while turned < abs(angle): #turn to specified angle
            self.__rotate(toMove, pivotPoints)
            self.__update()
            self.__generate_postscript()
            
            time.sleep(self.__calcSleepTime(angle, angle))
            turned += .5
            self.__updateDistanceLabel(str(round(turned, 2)), "degrees")

            self.orientationDegrees = (self.orientationDegrees + toMove) % 360
            self.robotInfoFrame.orientationLabelText.set("orientation: " + str(self.orientationDegrees))



    def turnLeft(self, angle):
        """
        turn in place left
        """
        self.robotInfoFrame.commandLabelText.set(("turnLeft " + str(angle)))

        if self.reversed:
            angle = 0 - angle

        turned = 0
        orientation = angle / abs(angle) #to account for negative turns
        toMove = -1 * orientation * .5

        while turned < abs(angle): #turn to specified angle
            self.__rotateInPlace(toMove)
            self.__update()
            self.__generate_postscript()
            
            time.sleep(self.__calcSleepTime(angle, angle))
            turned += .5
            self.__updateDistanceLabel(str(round(turned, 2)), "degrees")

            self.orientationDegrees = (self.orientationDegrees - toMove) % 360
            self.robotInfoFrame.orientationLabelText.set("orientation: " + str(self.orientationDegrees))




    def turnRight(self, angle):
        """
        turn in place right
        """
        self.robotInfoFrame.commandLabelText.set(("turnRight " + str(angle)))

        if self.reversed:
            angle = 0 - angle

        turned = 0
        orientation = angle / abs(angle) #to account for negative turns
        toMove = .5 * orientation

        while turned < abs(angle): #turn to specified angle
            self.__rotateInPlace(toMove)
            self.__update()
            self.__generate_postscript()
            
            time.sleep(self.__calcSleepTime(angle*2, angle))
            turned += .5
            self.__updateDistanceLabel(str(round(turned, 2)), "degrees")


            self.orientationDegrees = (self.orientationDegrees - toMove) % 360
            self.robotInfoFrame.orientationLabelText.set("orientation: " + str(self.orientationDegrees))







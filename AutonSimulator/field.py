#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import tkinter as tk


class Tile:
    """
    gives ability to draw objects on each tile
    """
    def __init__(self, x, y, distance, canvas):
        self.canvas = canvas

        self.distance = distance

        self.P1 = [x, y]
        self.P2 = [x + distance, y]
        self.P3 = [x, y + distance]
        self.P4 = [x + distance, y + distance]


        self.grid = []
        x = self.P1[0]
        increment = self.distance/47
        for i in range(0, 48): #makes grid with coordinates
            column = []
            y = self.P1[1]
            for j in range(0, 48):
                coords = [x, y]
                column.append(coords)
                y = y + increment
            x = x + increment
            self.grid.append(column)



    def __makeCenteredGrid(self, size):
        centeredGrid = []
        x = self.P1[0]
        increment = (self.distance - size) / 2
        for i in range(0, 3): #makes grid with coordinates
            column = []
            y = self.P1[1]
            for j in range(0, 3):
                coords = [x, y]
                column.append(coords)
                y = y + increment
            x = x + increment
            centeredGrid.append(column)

        return centeredGrid


    def drawObjectTiles(self, position=[23, 23], size=24, color="#cccccc", outline=None, width=None):
        """
        draws object to fill a square of the grid in inches
        """
        x1 = self.grid[position[0]][position[1]][0]
        y1 = self.grid[position[0]][position[1]][1]

        s = self.distance/25
        for i in range(0, size):
            s = s + (self.distance/25)

        x1 = x1 - (s/2)
        y1 = y1 - (s/2)

        x2 = self.grid[position[0]][position[1]][0] + (s/2)
        y2 = self.grid[position[0]][position[1]][1] + (s/2)

        self.canvas.create_rectangle(x1, y1, x2, y2, outline=outline, fill=color, width=width)



    def drawObjectFieldElementSquare(self, position=[1, 1], size=8, color="#cccccc", outline=None, width=None):
        """
        draws object centered on the coordinate chosen in inches
        """
        x1 = self.grid[position[0]][position[1]][0]
        y1 = self.grid[position[0]][position[1]][1]

        s = self.distance/25
        for i in range(0, size):
            s = s + (self.distance/25)

        x1 = x1 - (s/2)
        y1 = y1 - (s/2)

        x2 = self.grid[position[0]][position[1]][0] + (s/2)
        y2 = self.grid[position[0]][position[1]][1] + (s/2)

        self.canvas.create_rectangle(x1, y1, x2, y2, outline=outline, fill=color, width=width)


    def drawObjectFieldElementCircle(self, position=[1, 1], size=8, color="#cccccc", outline=None, width=None):
        """
        draws object centered on the coordinate chosen in inches
        """
        x1 = self.grid[position[0]][position[1]][0]
        y1 = self.grid[position[0]][position[1]][1]

        s = self.distance/25
        for i in range(0, size):
            s = s + (self.distance/25)

        x1 = x1 - (s/2)
        y1 = y1 - (s/2)

        x2 = self.grid[position[0]][position[1]][0] + (s/2)
        y2 = self.grid[position[0]][position[1]][1] + (s/2)

        self.canvas.create_oval(x1, y1, x2, y2, outline=outline, fill=color, width=width)




    def drawObjectFieldElementCentered(self, position=[1, 1], size=8, color="#cccccc", outline=None, width=None):
        """
        draws object contained only in tile in inches
        """
        s = self.distance/25
        for i in range(0, size):
            s = s + (self.distance/25)

        centeredGrid = self.__makeCenteredGrid(s)

        x1 = centeredGrid[position[0]][position[0]][0]
        y1 = centeredGrid[position[0]][position[1]][1]

        x2 = centeredGrid[position[0]][position[0]][0] + s
        y2 = centeredGrid[position[0]][position[1]][1] + s

        self.canvas.create_rectangle(x1, y1, x2, y2, outline=outline, fill=color, width=width)


    def drawRectangle(self, vertexes=[[0, 1], [2, 1], [2, 1], [0, 1]], color="#cccccc", outline="white", width=3):
        """
        draws a rectangle given four vertices
        """
        points = [
            [
                self.grid[vertexes[0][0]][vertexes[0][0]][0],
                self.grid[vertexes[0][1]][vertexes[0][1]][1]
            ],
            [
                self.grid[vertexes[1][0]][vertexes[1][0]][0],
                self.grid[vertexes[1][1]][vertexes[1][1]][1]
            ],
            [
                self.grid[vertexes[2][0]][vertexes[2][0]][0],
                self.grid[vertexes[2][1]][vertexes[2][1]][1]
            ],
            [
                self.grid[vertexes[3][0]][vertexes[3][0]][0],
                self.grid[vertexes[3][1]][vertexes[3][1]][1]
            ]

            ]

        self.canvas.create_polygon(points, outline=outline, fill=color, width=width)



    def placeRobot(self, position=[1, 1], size=17):
        """
        draws robot in tile in inches
        """
        s = self.distance/25
        for i in range(0, size):
            s = s + (self.distance/25)

        #centeredGrid = self.__makeCenteredGrid(s)

        x1 = self.grid[position[0]][position[0]][0]
        y1 = self.grid[position[0]][position[1]][1]

        x2 = self.grid[position[0]][position[0]][0] + s
        y2 = self.grid[position[0]][position[1]][1] + s

        coords = [[x1, y1], [x2, y2]]
        return coords








class Field:
    """
    creates the field
    """
    def __init__(self, width, height):
        master = tk.Tk()
        self.master = master
        self.canvas = tk.Canvas(master, width=width, height=height)
        self.canvas.grid(row=0, column=0, columnspan=3)

        self.canvas.create_rectangle(0, 0, width, height, fill="#ffffff")

        distance = height - 100
        self.P1 = [((width-distance)/2), 50]
        self.P2 = [(((width-distance)/2) + (distance)), 50]
        self.P3 = [((width-distance)/2), (50+distance)]
        self.P4 = [(((width-distance)/2) + (distance)), (50+distance)]

        self.width = width
        self.height = height
        self.distance = distance

        self.field = []




    def drawField(self):
        """
        draws the field based on the resolution given
        """
        self.canvas.create_rectangle(self.P1[0], self.P1[1], self.P4[0], self.P4[1], fill="#cccccc")

        self.canvas.create_line(self.P1[0], self.P1[1], self.P2[0], self.P2[1], fill="black", width=7) #horizontal
        self.canvas.create_line(self.P1[0], self.P1[1] - 3, self.P3[0], self.P3[1] + 4, fill="black", width=7) #vertical

        self.canvas.create_line(self.P2[0], self.P2[1] - 3, self.P4[0], self.P4[1] + 4, fill="black", width=7) #vertical
        self.canvas.create_line(self.P3[0], self.P3[1], self.P4[0], self.P4[1], fill="black", width=7) #horizontal



    def drawGrid(self):
        """
        draws grid on the field
        """
        start = (self.width - self.distance) / 2
        increment = self.distance / 6

        for x in range(0, 6): #verical lines
            P1 = [start + (increment*x), 50]
            P2 = [start + (increment*x), (50+self.distance)]

            self.canvas.create_line(P1[0], P1[1], P2[0], P2[1], fill="black", width=2)

        start = 50
        for x in range(0, 6): #horizontal lines
            P1 = [((self.width - self.distance) / 2), start + (increment*x)]
            P2 = [(((self.width - self.distance) / 2) + self.distance), start + (increment*x)]

            self.canvas.create_line(P1[0], P1[1], P2[0], P2[1], fill="black", width=2)


    def fieldInfo(self):
        """
        creates objects of each tile and appends them to a list
        so that it is possible to create objects on the field
        """
        distance = self.distance/6
        x = ((self.width-self.distance)/2)

        for i in range(0, 6): #columns
            column = []
            y = 50
            for j in range(0, 6): #rows
                tileObject = Tile(x, y, distance, self.canvas)
                column.append(tileObject)

                y = y + distance
            x = x + distance

            self.field.append(column)












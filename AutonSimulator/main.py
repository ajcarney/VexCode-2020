import tkinter as tk

import robot
import field
import fieldObjects
import autonomous
import runningFrame
import robotInfoFrame
import controlPanelFrame


#user defines
X_RES = 1600
Y_RES = 900
ROBOT_START_TILE = [0, 1]
ROBOT_START = [0, 24]
ROBOT_START_ANGLE = 0



#sets up and draws field
f = field.Field(X_RES, Y_RES)
f.drawField()
f.drawGrid()
f.fieldInfo()
field = f.field
fieldSize = f.distance
robotCoordinates = field[ROBOT_START_TILE[0]][ROBOT_START_TILE[1]].placeRobot([ROBOT_START[0], ROBOT_START[1]])

fieldObjects.main(field)



#gets canvas and tkinter object
canvas = f.canvas
master = f.master



#sets up GUI further
master.title("Autonomous Simulator")
master.wm_title("Autonomous Simulator")
icon = tk.Image("photo", file="autonSimulator.png")
# master.tk.call('wm', 'iconphoto', master._w, icon)
master.resizable(0, 0)


#sets up buttons and labels
runningPane = runningFrame.runningFrame(master)
robotInfoPane = robotInfoFrame.robotInfoFrame(master)
controlPanelPane = controlPanelFrame.controlPanelFrame(master, runningPane)

runningPane.placeObjects()
robotInfoPane.placeObjects()
controlPanelPane.placeObjects()



#moves robot to start location
bot = robot.robot(fieldSize=fieldSize, tkobj=master, canvas=canvas, controlPanelFrame=controlPanelPane, robotInfoFrame=robotInfoPane)
bot.show(angle=ROBOT_START_ANGLE, position=robotCoordinates)


def motion(event):
    x, y = event.x, event.y
    screen_offset_x = (X_RES - (Y_RES - 100)) / 2
    screen_offset_y = 50
    offset_x = bot.x_offset_in 
    offset_y = bot.y_offset_in
    # print(bot.inches(x), bot.x_offset_in)
    corrected_coords = [bot.inches(x) - bot.y_offset_in, bot.inches(y) - bot.x_offset_in]
    print('{}, {}'.format(corrected_coords[0], corrected_coords[1]))
    
def forward(event):
    bot.forward(25)
    
def backward(event):
    bot.backward(25)
    
def turnRight(event):
    bot.turnRight(2)
    
def turnLeft(event):
    bot.turnLeft(2)


#runs autonomous
try:
    master.bind('<Motion>', motion)
    master.bind("w", forward)
    master.bind("s", backward)
    master.bind("a", turnLeft)
    master.bind("d", turnRight)

    auton = autonomous.auton(bot, master, controlPanelPane, robotInfoPane)
    auton.commands()

except tk.TclError:
    pass

finally:
    try: #finished all the way through
        controlPanelPane.disable()

        controlPanelPane.idle()

    except tk.TclError: #window closed
        print("finished")











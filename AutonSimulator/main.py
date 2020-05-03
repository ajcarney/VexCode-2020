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



def motion(event):
    x, y = event.x, event.y
    corrected_coords = [x - 400, y - 50]
    print('{}, {}'.format(bot.inches(corrected_coords[0]), bot.inches(corrected_coords[1])))
    


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
master.tk.call('wm', 'iconphoto', master._w, icon)
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





#runs autonomous
try:
    master.bind('<Motion>', motion)

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











#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import smtplib
import getpass


SEND_EMAIL = 0
FILE = 'autonomous.py'


class email():
    def __init__(self):
        self.passwd = None
        self.user_email = 'jg570144@gmail.com'
        self.recipients = []

        self.message = None

        self.smtpObj = None
        self.check = None
        self.subject = None
        self.body = None
        self.sent = 0


    def getCredentials(self):
        #self.user_email = input("email address:\n")
        self.passwd = getpass.getpass()


    def getMessage(self, oldCode, newCode):
        r = int(input("\nEnter number of recipients\n"))

        for x in range(r):
            remail = input("enter recipient email\n")
            self.recipients.append(remail)

        self.subject = "translated autonomous code"

        self.body = "old auton code:\n\n"

        for i in oldCode:
            self.body = self.body + i + '\n'
            print("")

        self.body = self.body + '\n\n\n\nnew auton code:\n\n'

        for i in finalCode:
            self.body = self.body + i + '\n'

        self.body = self.body + '\n\n\nthis is an automated message\ncourtesy of Aiden'
        print("")



    def login(self):
        self.smtpObj = smtplib.SMTP("smtp.gmail.com:587")
        self.smtpObj.starttls()
        self.smtpObj.ehlo()
        self.smtpObj.login(self.user_email, self.passwd)


    def send(self):
        print("sent: ")

        message = "Subject: " + self.subject + "\n" + self.body

        self.check = self.smtpObj.sendmail(self.user_email, self.recipients, message)

    def close(self):
        self.smtpObj.quit()











class Translator:

    def __init__(self, file):
        self.file = open(file)
        self.code = self.file.readlines()

        self.oldCode = []
        self.newCode = []

        self.conversion = {
            "turnRight": "turnRightV",
            "turnLeft": "turnLeftV",
            "rightSide": "rs",
            "leftSide": "ls",
            "forward": "driveForward",
            "backward": "driveForward",
            "reverse": "changeDirection",
            "intakeStart": "intakeStart",
            "intakeEnd": "intakeEnd",
            "outake": "intakeStart",
            "catapult": "shootBall",
            "capFlipper": "flip"
            }


        self.functionMap = {
            "value": self.value,
            "sleep100RPM": self.sleep100RPM,
			"sleep200RPM": self.sleep200RPM,
			"sleep600RPM": self.sleep600RPM,
            "negativeValue": self.negativeValue,
            "noParam": self.noParam
             }


        self.specialInstructions = {
            "forward": ["value", "sleep200RPM"],
            "backward": ["negativeValue", "sleep200RPM"],
            "turnLeft": ["noParam"],
            "turnRight": ["noParam"],
            "intakeStart": ["negativeValue"],
            "outakeStart": ["value"],
            "intakeEnd": ["sleep600"],
            "catapult": ["value", "sleep100"],
            "capFlipper": ["value"],
            }



        self.parameterValue = ""
        self.sleepTime = ""
        self.separator = ""
        self.instructions = []


    def value(self):
        self.parameterValue = str(self.parameterValue)
        self.sleepTime = ""
        self.separator = ""

    def sleep100RPM(self):
        RPM = 100
        RPS = RPM / 60
        revolutions = (abs(float(self.parameterValue)) / 360)
        self.sleepTime = (1000 * (revolutions / RPS)) + 50
        #self.sleepTime = str(int((1000 * abs((float(self.parameterValue))/360))) + 200)
        if len(self.instructions) == 1:
            self.parameterValue = ""
            self.separator = ""
        else:
            self.separator = ", "

    def sleep200RPM(self):
        RPM = 200
        RPS = RPM / 60
        revolutions = (abs(float(self.parameterValue)) / 360)
        self.sleepTime = (1000 * (revolutions / RPS)) + 50
        if len(self.instructions) == 1:
            self.parameterValue = ""
            self.separator = ""
        else:
            self.separator = ", "

    def sleep600RPM(self):
        RPM = 600
        RPS = RPM / 60
        revolutions = (abs(float(self.parameterValue)) / 360)
        self.sleepTime = (1000 * (revolutions / RPS)) + 50
        if len(self.instructions) == 1:
            self.parameterValue = ""
            self.separator = ""
        else:
            self.separator = ", "


    def negativeValue(self):
        self.parameterValue = str(0 - float(self.parameterValue))
        self.sleepTime = ""
        self.separator = ""

    def noParam(self):
        self.parameterValue = ""
        self.sleepTime = ""
        self.separator = ""





    def translate(self):

        for i in self.code:

            try:
                oldCommand = i.split(".")[1]
                try:
                    rawCommand = i.split(".")[2]
                    oldCommand = oldCommand + "." + rawCommand
                except:
                    rawCommand = i.split(".")[1]

                oldCommand = oldCommand.split(")")[0]
                oldCommand = 'self.' + oldCommand + ")"

                command = rawCommand.split("(")[0]
                value = rawCommand.split("(")[1]
                self.parameterValue = value.split(")")[0]

                self.instructions = list(self.specialInstructions.get(command))
                for j in self.instructions:
                    self.functionMap[str(j)]()

                newCommand = self.conversion.get(command)
                newCommand = (newCommand
                              + "(" + str(self.parameterValue)
                              + str(self.separator)
                              + str(self.sleepTime)
                              + ");"
                              )

                self.oldCode.append(oldCommand)
                self.newCode.append(newCommand)
                self.parameterValue = ""
                self.sleep = ""
                self.separator = ""

            except:
                self.parameterValue = ""
                self.sleep = ""
                self.separator = ""

        return self.oldCode, self.newCode



t = Translator(FILE)
oldCode, finalCode = t.translate()


if SEND_EMAIL:
    s = email()
    s.getCredentials()
    s.getMessage(oldCode, finalCode)
    s.login()
    s.send()
    s.close()

    print("")
    print("")
    print("done")

else:
    for i in oldCode:
        print(i)
    print("")
    for i in finalCode:
        print(i)













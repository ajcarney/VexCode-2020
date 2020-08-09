#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Jul 27 16:48:08 2020

@author: aiden
"""
import flask

import serial_client


# values = """0xA0 0xA0
# 0xA0 0xA1
# 0xA0 0xA2
# 0xA0 0xA3
# 0xA0 0xA4
# 0xA0 0xA5
# 0xA0 0xA6
# 0xA0 0xA7
# 0xA0 0xA8
# 0xA0 0xA9
# 0xA0 0xAA
# 0xA0 0xAB
# 0xA0 0xAC
# 0xA0 0xAD
# 0xA0 0xAE
# 0xA0 0xAF
# 0xA1 0xA0"""

# for line in values.split("\n"):
#     byte_s = line.split(" ")
#     msb = byte_s[0].strip()
#     lsb = byte_s[1].strip()
    
#     return_id = (int(msb, 0) << 8) | int(lsb, 0);
#     print(return_id)
motors = {
    0:"Front Right",    
    1:"Front Left",    
    2:"Back Right",    
    3:"Back Left",    
    4:"Main Intake",    
    5:"Hoarding Intake",    
    6:"Lift",    
}

app = flask.Flask(__name__)


def get_motor_data(client, motor_num):
    try:
        motor_data = {
            "Actual Velocity":client.get_command('\xA0', '\xA0', motor_num),
            "Actual Voltage":client.get_command('\xA0', '\xA1', motor_num),
            "Current Draw":client.get_command('\xA0', '\xA2', motor_num),
            "Encoder Position":client.get_command('\xA0', '\xA3', motor_num),
            "Brakemode":client.get_command('\xA0', '\xA4', motor_num),
            "Gearset":client.get_command('\xA0', '\xA5', motor_num),
            "Port":client.get_command('\xA0', '\xA6', motor_num),
            "PID Constants":client.get_command('\0xA0', '\xA7', motor_num),
            "Slew Rate":client.get_command('\xA0', '\xA8', motor_num),
            "Power":client.get_command('\xA0', '\xA9', motor_num),
            "Temperature":client.get_command('\xA0', '\xAA', motor_num),
            "Torque":client.get_command('\xA0', '\xAB', motor_num),
            "Direction":client.get_command('\xA0', '\xAC', motor_num),
            "Efficiency":client.get_command('\xA0', '\xAD', motor_num),
            "Is Stopped":client.get_command('\xA0', '\xAE', motor_num),
            "Is Reversed":client.get_command('\0A0', '\xAF', motor_num),
            "Is Registered":client.get_command('\xA1', '\xA0', motor_num)
        }
    except TimeoutError as e:
        print(e)
        motor_data = {
            "Actual Velocity":None,
            "Actual Voltage":None,
            "Current Draw":None,
            "Encoder Position":None,
            "Brakemode":None,
            "Gearset":None,
            "Port":None,
            "PID Constants":None,
            "Slew Rate":None,
            "Power":None,
            "Temperature":None,
            "Torque":None,
            "Direction":None,
            "Efficiency":None,
            "Is Stopped":None,
            "Is Reversed":None,
            "Is Registered":None
        }
    
    return motor_data


class InvalidUsage(Exception):
    status_code = 400

    def __init__(self, message, status_code=None, payload=None):
        Exception.__init__(self)
        self.message = message
        if status_code:
            self.status_code = status_code
        self.payload = payload

    def to_dict(self):
        rv = dict(self.payload or ())
        rv['message'] = self.message
        return rv


@app.errorhandler(InvalidUsage)
def handle_invalid_usage(error):
    response = flask.jsonify(error.to_dict())
    response.status_code = error.status_code
    return response


@app.route("/api/motor_data/<motor_number>", methods=["GET"])
def api_get_motor_data(motor_number):
    if int(motor_number) in motors.keys():
        data = get_motor_data(motor_client, motor_number)
        return flask.jsonify(data)
    
    else:
        raise InvalidUsage("Motor Number supplied was not valid", status_code=406)

@app.route("/api/debug", methods=["GET"])
def api_debug():
    motor_client.debug("test message")

    
server_conn = serial_client.ServerConnection(debug=True)
x = server_conn.mount_vex_brain()       
server_conn.start_server()
motor_client = serial_client.Client(55000)
server_conn.add_client(motor_client)




app.run(host='0.0.0.0')








        
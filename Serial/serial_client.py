#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Jul 26 11:00:42 2020

@author: aiden
"""

import serial
import subprocess
import threading
import time
import queue
from functools import wraps




def create_double(n):
    if n >= 0:
        sign = 0
    else:
        sign = 1
        
    bias = 1023
    interval = (0, 2048)
    exp_term = 0
    while not 1 <= exp_term < 2:
        mid = int(interval[0] + (interval[1] - interval[0]) / 2)
        exp_term = (abs(n) / (2**(mid - bias)))
        
        if exp_term < 1:  # use lower range
            interval = (interval[0], mid + 1) 
            exp_guess = mid
        else:  # use upper range
            mid = int(interval[0] + (interval[1] - interval[0]) / 2)
            interval = (mid - 1, interval[1]) 
            exp_guess = mid
    
    significand = 0
    total = exp_term - 1
    for i in range(1, 53):
        if total - (2**(-i)) >= 0: 
            total = total - (2**(-i))
            significand |= 2**(52 - i)
    
    byte_list = []
    # first 6 bytes are from significand
    for i in range(6):
        byte = significand & 0xff
        byte_list.append(byte)
        significand = significand >> 8

    # 7th byte is part significand (4 bits) and part exponent (4 bits)
    byte = significand
    byte |= ((exp_guess & 0x0f) << 4)
    exp_guess = exp_guess >> 4
    byte_list.append(byte)
    
    # 8th byte is part exponent (7 bits) and the sign bit (1 bit)
    byte = exp_guess & 0x7f
    byte |= sign << 7
    byte_list.append(byte)
        
    return byte_list
    
    

class Client:
    def __init__(self, uid):
        self.uid = uid
        self.send_queue = queue.Queue()
        self.send_queue_lock = threading.Lock()
        
        self.recv_queue = queue.Queue()
        self.recv_queue_lock = threading.Lock()
    
    
    def get_command(self, id1, id2, msg=""):
        msg = id1 + id2 + msg
        with self.send_queue_lock:
            self.send_queue.put(msg)
            
        start = time.time()
        while 1:  # set max waiting time to 5 sec
            if self.recv_queue.empty():
                time.sleep(.01)
            else:
                with self.recv_queue_lock:
                    received = self.recv_queue.get()
                break
            end = time.time()
            
            if (end - start) > 5:
                raise TimeoutError("No response returned from host with sent message: " + msg)
    
        return received
        
    def post_command(self, id1, id2, msg=""):
        pass
    
    def debug(self, debug_message):
        msg = '\xAB' + '\xA0' + debug_message
        with self.send_queue_lock:
            self.send_queue.put(msg)
            
        start = time.time()
        while 1:  # set max waiting time to 5 sec
            if self.recv_queue.empty():
                time.sleep(.01)
            else:
                with self.recv_queue_lock:
                    received = self.recv_queue.get()
                break
            end = time.time()
            
            if (end - start) > 5:
                raise TimeoutError("No response returned from host with sent message: " + debug_message)
    
        return received
    
    
class ServerConnection:
    def __init__(self, debug=False, read_chunk_size=20):
        self.connection = None
        self.debug = debug 
        self.read_chunk_size = read_chunk_size
        
        self.__write_thread = threading.Thread(target=self.write_thread)
        self.run_writing_thread = False
                
        self.__read_thread = threading.Thread(target=self.read_server_stdout)
        self.run_reading_thread = False
        
        self.__write_thread.daemon = True 
        self.__read_thread.daemon = True
        
        self.clients = []
        self.client_lock = threading.Lock()
        
        self.connection_lock = threading.Lock()
        
        self.__write_thread.start()
        self.__read_thread.start()
        
        
        
    def serial_exception_handler(func):
        @wraps(func)
        def inner_function(self, *args, **kwargs):
            while 1:
                try:
                    return func(self, *args, **kwargs)
                except serial.SerialException as e:
                    if self.debug:
                        print(e)
                except serial.serialutil.SerialException as e:
                    if self.debug:
                        print(e)   
                except OSError as e:
                    if self.debug:
                        print(e)                
                except AttributeError:
                    if self.debug:
                        print("Connection is not established; attemting to establish one")
                
                self.run_writing_thread = False
                self.run_reading_thread = False
                time.sleep(.5)
                self.connection = None
                
                with self.connection_lock:  # use a lock in case multiple threads are trying to establish a connection
                    while not self.mount_vex_brain():
                        time.sleep(.1)
                        if self.debug:
                            print("retrying connection", flush=True)
                            
                self.run_writing_thread = True
                self.run_reading_thread = True     
                
            
        return inner_function

    
    def mount_vex_brain(self):
        command = "./usb.sh"
        process = subprocess.Popen(command, stdout=subprocess.PIPE)
        
        ttys = []
        for i in process.stdout.readlines():
            i = i.decode("utf=8")
            if "VEX" in i:
                ttys.append(i.split(" ")[0])
        
        if not ttys:
            if self.debug:
                print("No mount points for the vex brain were found", flush=True)
            return 0
        
        mnts = sorted(ttys, reverse=True)
        tty = ""
        for i in mnts:
            if "ACM" in i:
                tty = i
                break
        
        try:
            self.connection = serial.Serial(
                tty, 
                baudrate=115200, 
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE, 
                stopbits=serial.STOPBITS_ONE
            )
            
        except serial.SerialException:
            if self.debug:
                print("Failed to open Vex Brain on ", tty)
            return 0
        
        if self.debug:
            print("connection established")
            
        return 1
        
    
    @serial_exception_handler
    def read_byte(self):
        return self.connection.read(1)

    @serial_exception_handler
    def write_bytes(self, send_array):
        self.connection.write(send_array)
        return 1
    

    def read_server_stdout(self):
        read_check = 0
        while 1:
            if self.run_reading_thread:
                for i in range(self.read_chunk_size):
                    byte = self.read_byte()
                    
                    if read_check == 0 and int.from_bytes(byte, "big") == 0xAA:
                        read_check = 1
                        print("here1")
                    elif read_check == 1 and int.from_bytes(byte, "big") == 0x55:
                        read_check = 2
                        print("here2")
                    elif read_check == 2 and int.from_bytes(byte, "big") == 0x1E:
                        read_check = 3
                        print("here3")
                    elif read_check == 3:
                        num_bytes_following = int.from_bytes(byte, "big")
                        uid_msb = int.from_bytes(self.read_byte(), "big")
                        uid_lsb = int.from_bytes(self.read_byte(), "big")
                        # print("uid msb:", uid_msb)
                        # print("uid lsb:", uid_lsb)
                        uid = (uid_msb << 8) | uid_lsb
                        
                        msg = ""
                        for i in range(num_bytes_following - 2):
                            try:
                                char = self.read_byte().decode("ascii")
                            except UnicodeDecodeError:
                                if self.debug:
                                    print("failed to decode character")
                                char = ""
                                
                            msg += char
                        if self.debug:
                            print("message received: ", msg)
                            
                        checksum = self.read_byte()
                        if int.from_bytes(checksum, "big") == 0xC6:
                            # find server with that id and add message to its queue
                            for client in self.clients:
                                if client.uid == uid:
                                    with client.recv_queue_lock:
                                        client.recv_queue.put(msg)
                                        
                        elif int.from_bytes(checksum, "big") != 0xC6 and self.debug:
                            print("checksum failed - received: ", checksum)
                            
                        read_check = 0
                                
                    else:  # if response from server is not part of message send to stdout
                        read_check = 0
                        try:
                            char = byte.decode("ascii")
                        except UnicodeDecodeError:
                            print(byte)
                            char = ""
                        print(char, end="")

                    
            
    def write_thread(self):
        while 1:
            if self.run_writing_thread:
                with self.client_lock:
                    for client in self.clients:
                        with client.send_queue_lock:
                            if not client.send_queue.empty():
                                to_write = client.send_queue.get()
                            else:
                                continue
                        
                        
                        send_array = bytearray()
                        send_array.append(0xAA)
                        send_array.append(0x55)
                        send_array.append(0x1E)
                        send_array.append(len(to_write) + 2)  # add two for the uid bytes
                        
                        send_array.append((client.uid >> 8) & 0xFF)
                        send_array.append(client.uid & 0xFF)
                        
                        for i in to_write:
                            send_array.append(ord(i))
                            
                        send_array.append(0xC6)
                        
                        if self.debug:
                            print("Message sent: ", send_array)
                        
                    
                        self.write_bytes(send_array)
                        
                    # write garbage on the stream to help clear any blocking functions on server
                    # send_array = bytearray()
                    # send_array.append(0xFF)
                    # send_array.append(0xFF)
                    # send_array.append(0xFF)
                    # send_array.append(0xFF)
                    # send_array.append(0xFF)
                    
                    # self.write_bytes(send_array)
                    
                time.sleep(.05)
                
        
    def add_client(self, client):
        with self.client_lock:
            self.clients.append(client)
            
    
    def start_server(self):
        self.run_writing_thread = True
        self.run_reading_thread = True
        
    def stop_server(self):
        self.run_writing_thread = False
        self.run_reading_thread = False        
        
        
if __name__ == "__main__":
    c = ServerConnection(debug=True)
    x = c.mount_vex_brain()       
    c.start_server()
    client = Client(55000)
    c.add_client(client)
    while 1:
        client.debug("test")
        time.sleep(2)
        
        
        
        
        
        
        
        
        
        
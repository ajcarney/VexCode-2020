#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Jan  6 22:38:31 2020

@author: aiden
"""
class LCD:
    """
    class for communicating with the vex lcd via serial communication
    """
    def __init__(self, serial_write, serial_read):
        self.lcd_screen = serial_write
        self.lcd_buttons = serial_read
        
        self.__flags = 0x00
        self.__line_1 = " " * 16
        self.__line_2 = " " * 16
        
        self.__num_chars = 16
        
        
    def __write_line(self):
        """
        Writes to the lcd. Runs from thread, not called by user

        Returns
        -------
        int
            1 on success.

        """
        #line 1 bytearray
        send_array = bytearray()
        send_array.append(0xAA)
        send_array.append(0x55)
        send_array.append(0x1E)
        send_array.append(0x12)
        send_array.append(0x00)
        checksum = 0x00
        
        for char in self.__line_1:
            send_array.append(ord(char))
            checksum += (ord(char))
            
        self.lcd_screen.write(send_array)
            
        #line 2 bytearray
        send_array = bytearray()
        send_array.append(0xAA)
        send_array.append(0x55)
        send_array.append(0x1E)
        send_array.append(0x12)
        send_array.append(0x01)
        checksum = 0x01
        
        for char in self.__line_1:
            send_array.append(ord(char))
            checksum += (ord(char))
            
        self.lcd_screen.write(send_array)
        
        return 1
            
            
        
        
    def write_string(self, string, ln=0, align="left"):
        """
        writes a string to the lcd
        handles multiline writing by keeping track of line one and line two
        and adding a newline character between them
        
        returns 0 on unsucessful write and 1 if completed successfully
        """
        buffer = self.__num_chars - len(string)
        if align == "center":
            left_spaces = round(buffer/2, 0)
            right_spaces = self.__num_chars - len(string) - left_spaces
            if right_spaces < 0:
                right_spaces = 0
            string = (" " * left_spaces) + string + (" " * right_spaces)
            
        elif align == "right":
            left_spaces = self.__num_chars - len(string)
            if left_spaces < 0:
                left_spaces = 0
            string = (" " * left_spaces) + string
            
        elif align == "left":
            string = string
            
        else:
            return 0
        
        if len(string) > self.__num_chars:  #cap string length to the number of characters on the lcd
            string = string[:self.__num_chars]
            
            
        if ln == 0:
            self.__line_1 = string
        elif ln == 1:
            self.__line_2 = string

        return 1
    
    
    
    
    def clear(self, line):
        """
        clears a line on the lcd by sending spaces

        Parameters
        ----------
        line : int
            the line to clear.

        Returns
        -------
        int
            1 on success, 0 on failure.

        """
        string = " " * 16
        ret = self.write_string(string, ln=line)
        
        if not ret:
            return 0
        
        return 1
    
    
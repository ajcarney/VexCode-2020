#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Aug 20 16:48:22 2019

@author: aiden
"""

class Exit(Exception):
    """
    thrown for graceful program termination
    stack trace might not be shown if raised
    """
    pass

class UnknownOption(Exception):
    """
    thrown when an invalid parameter or and invalid command is given
    """
    def __init__(self):
        msg = "Unknown option given"
        super().__init__(msg)
        print(msg)

class InvalidFileName(Exception):
    """
    exception for and invalid file name given
    """
    def __init__(self):
        msg = ("Invalid file name given:\n"
               + "must end in valid header extension type ex.'.hpp' or '.h'")

        super().__init__(msg)
        print(msg)
        raise Exit

class InvalidAddition(Exception):
    """
    addition of object failed
    """
    def __init__(self, msg=None):
        if not msg:
            msg = ("adding of object failed: check to see that parent is of a valid type\n"
                   + "and that parameters were passed correctly")
        super().__init__(msg)
        print(msg)

#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Feb  9 19:36:41 2021

@author: aiden
"""
import glob

# robot source code
header_files = glob.glob("src/**/" + '*.hpp', recursive=True)
impl_files = glob.glob("src/**/" + '*.cpp', recursive=True)
py_files = glob.glob("./**/" + '*.py', recursive=True)
files = header_files + impl_files + py_files
robocode_lines = 0
for file in files:
    with open(file) as f:
        for i, l in enumerate(f):
            pass
        print(file, i + 1)
        robocode_lines += i + 1

side_project_files = (
    glob.glob("../**/" + '*.py', recursive=True)
    + glob.glob("../**/" + '*.sh', recursive=True)

) 
sidecode_lines = 0
for file in side_project_files:
    with open(file) as f:
        for i, l in enumerate(f):
            pass
        print(file, i + 1)
        sidecode_lines += i + 1

print()
print()
print("Code that goes on robot:", robocode_lines, "lines")
print("Code from side projects:", sidecode_lines, "lines")
print()
print("Total:", sidecode_lines + robocode_lines, "lines")
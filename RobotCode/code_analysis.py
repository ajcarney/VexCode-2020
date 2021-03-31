#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Mar  1 13:05:32 2020

@author: aiden
"""

import datetime
import glob

header_files = glob.glob("src/**/" + '*.hpp', recursive=True)
impl_files = glob.glob("src/**/" + '*.cpp', recursive=True)

files = sorted(header_files + impl_files)
todo_comments = {}
review_dates = {}
for file in files:
    todo_comments.update({file:[]})
    with open(file) as f:
        for i, line in enumerate(f.readlines()):
            if "TODO: " in line:
                comment = line.split("TODO: ")[-1].strip()
                todo_comments[file].append([comment, i + 1])
            elif "@reviewed_on: " in line:
                date = line.split("@reviewed_on: ")[-1].strip()
                if date:
                    try:
                        date = datetime.datetime.strptime(date, "%m/%d/%Y")
                    except ValueError:
                        date = datetime.datetime.strptime(date, "%m/%d/%y")
                else:
                    date = False
                review_dates.update({file:date})

i1 = 0
i2 = 0

for file in todo_comments:
    if todo_comments.get(file):
        print(file)
        for comment in todo_comments.get(file):
            print("\t" + str(comment[1]) + ": " + comment[0])
            i1 += 1

to_review = []
for file in review_dates:
    if not review_dates.get(file):  # skip if no date is provided
        to_review.append([file, 999999999])
        continue
    days_elapsed = abs(datetime.datetime.now() - review_dates.get(file)).days
    if days_elapsed > 30:
        to_review.append([file, days_elapsed])

print("\n")

to_review = sorted(to_review, key=lambda x: (x[1], x[0]))
for file in to_review:
    if file[1] == 999999999:
        print(file[0], ": ", "never", sep="")
    else:
        print(file[0], ": ", file[1], " days", sep="")
    i2 += 1

print("\n")

print("number of files:", len(files))
print("number of todo comments:", i1)
print("number of files needing review:", i2)

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
        sidecode_lines += i + 1

print()
print()
print("Code that goes on robot:", robocode_lines, "lines")
print("Code from side projects:", sidecode_lines, "lines")
print("Total:", sidecode_lines + robocode_lines, "lines")

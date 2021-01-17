#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Jun  5 19:13:03 2019

@author: aiden
"""

import math 
import os
import shutil


class PostScript:
    """
    class for converting code to pictures
    """
    def __init__(self):
        self.alldirs = [".."] #list declaration that will contain all
                              #directories. Parent directory is included in it
                              #because there is code to be printed in it
                              #
                              #list will contain all directories that contain
                              #files to be printed

        self.allfiles = [] #list declaration that will contain all files that
                           #must be converted to postscript

        self.pictures_dir = "../Pictures/" #parent directory of where pictures
                                           #will be placed

        self.pictures = [] #contains locations of pictures

        self.directory_exceptions = [ #directories that contain no files to
                                      #be printed
            "../AutonSimulator/__pycache__",
            "../CodeGenerator/__pycache__",
            "../Pictures",
            "../PIDDebugging/__pycache__",
            "../Print/__pycache__",
            "../Prototypes",
            "../RobotCode/bin",
            "../RobotCode/firmware",
            "../RobotCode/include/display",
            "../RobotCode/include/okapi",
            "../RobotCode/include/pros",
            "../RobotCode/lib",
            "../RobotCode/.d",
            "../RobotCode/src/JSONLibrary",
            "../RobotCode/src/objects/lcdCode/fonts",
            "../Scouting/__pycache__",
            "../Cascades",
            "../.git"
            ]

        self.file_exceptions = [  #files in included directories of acceptable
                                  #extensions that should not be converted to
                                  #postscript to be printed
            "../license.html",
            "../PIDDebugging/ut.txt",
            "../PIDDebugging/test.txt",
            "../Print/tree.txt",
            "../Print/tree.rtf",
            "../RobotCode/include/main.h",
            "../RobotCode/include/api.h",
            "../RobotCode/compile_commands.json",
            "../RobotCode/src/objects/lcdCode/DriverControl/logo.c"
            ]

        self.valid_extensions = [ #files with this extension will be allowed to
                                  #be printed
            ".py",
            ".c",
            ".cpp",
            ".hpp",
            ".h",
            ".sh",
            ".txt",
            ".json",
            ".md"
            ]

        if not os.path.isdir(self.pictures_dir):
            os.mkdir(self.pictures_dir)  #makes directory for pictures


    def __get_directories_recursively(self, parent_directory, storage_list):
        """
        recursively prints all files in parent directory
        put in list "alldirs"

        @params = directory to start at, directory to write output to
        @return = None
        """

        dirs = [f.path for f in os.scandir(parent_directory) if f.is_dir()]

        for directory in dirs:
            storage_list.append(directory)
            self.__get_directories_recursively(directory, storage_list)



    def __find_files(self, directory):
        """
        finds all files that are not subdirectories in a given directory that
        are of a specific type

        @params = directory to look through
        @return = type list of files that are in the directory that are
                  not sub directories
        """

        files = [] #contains all files that will be returned

        contents = [f.path for f in os.scandir(directory) if not f.is_dir()]
            #returns everything in directory including
            #sub directories

        for file in contents: #checks to make sure that only files that are not
                              #directories and have a valid extension are added
                              #to the list of files
            _, extension = os.path.splitext(file)
            if extension in self.valid_extensions:
                files.append(file)

        return files





    def __convert_to_postscript(self, file):
        """
        converts the specified file to postscript format which is a printable
        code type. The files are placed inside of "../pictures/" in the same
        directories that they were originally in

        @params = relative path of file to convert
        @return = None
        """

        _, extension = os.path.splitext(file)
        output_name = ((self.pictures_dir + file.split(extension)[0]
                        + extension.upper().split(".")[1]).replace("/../", "/")
                       + ".ps")

        self.pictures.append(output_name)

        #get the highlight color that will be used by shell command
        #based on the type of file it is
        if extension == ".py":
            highlight_color = "--highlight=python"

        elif extension in [".cpp", ".hpp"]:
            highlight_color = "--highlight=cpp"

        elif extension in [".c", ".h"]:
            highlight_color = "--highlight=c"

        elif extension == ".sh":
            highlight_color = "--highlight=bash"

        else:
            highlight_color = "--highlight=mail"

        #runs command that will convert a file to postscript
        #the output file location is in self.pictures
        os.system("enscript -G --line-numbers -o "
                  + output_name + " "
                  + highlight_color
                  + " --color=1 -f Palatino-Roman5 --columns 1 "
                  + file)


    def prepare_code(self):
        """
        prepares code to be printed by converting all code to
        postscript

        @params = None
        @return = None
        """
        self.allfiles = [] #list declaration that will contain all files that
                           #must be converted to postscript

        self.pictures_dir = "../Pictures/" #parent directory of where pictures
                                           #will be placed

        self.pictures = [] #contains locations of pictures
        shutil.rmtree(self.pictures_dir)  #clean out pictures folder so that
                                          #nothing is there that shouldn't be
        os.mkdir(self.pictures_dir)
        
        self.__get_directories_recursively("..", self.alldirs)
                             #gets unrefined list of directories
                             #saved in list "alldirs"
        to_remove = []
        for directory in self.alldirs: #iterates through each directory
                                       #found
            for exception in self.directory_exceptions: #checks to see if
                                                   #directory should
                                                   #be excluded
                if exception in directory:
                    to_remove.append(directory)  #do not remove items from list
                                                 #because changing the size
                                                 #while iterating through the
                                                 #list will cause
                                                 #unexpected results

                    break #end looking for exception because if one has
                          #been found it is already in the to remove list
                          #and no other cases will match it

        alldirs = set(self.alldirs) #convert each list to type set
                               #and find the difference between them
                               #the difference will always be the
                               #directories wanted because all the items
                               #in "to_remove" will be in "alldirs" and all
                               #that is left is the directories to keep

        to_remove = set(to_remove)

        alldirs = list(alldirs - to_remove) #find difference of the two
                                            #sets and convert back to type
                                            #list remaining list will be
                                            #the directories that contain
                                            #code to be printed





        for directory in alldirs:
            self.allfiles.append(self.__find_files(directory))
                                                   #by adding
                                                   #the list
                                                   #directly to the all
                                                   #files list, list will
                                                   #be segmented and can
                                                   #be printed in order
                                                   #easier

            #make directory under pictures directory for a picture of the code
            #to go
            if not os.path.isdir((self.pictures_dir + directory).replace("/../", "/")):
                os.makedirs((self.pictures_dir + directory).replace("/../", "/"))
        
        for fileset in self.allfiles:
            for file in fileset:
                if not file in self.file_exceptions:
                    self.__convert_to_postscript(file)



    def get_page_count(self, files=None):
        """
        gets total size of all post script files in
        "Pictures" directory that was made on config file
        in "Pictures" directory

        @params = (optional) type list of paths of files to print
        @return = int amount of pages
        """
        if files:
            self.pictures = files

        elif not self.pictures and not files:
                              #if pictures list contains any file locations
                              #and if no files have been specified
                              #if it does not, then program will find all the
                              #files that are currently in the directory
                              #useful if user does not want to overwrite
                              #what is currently there

            self.pictures.append(self.pictures_dir) #add parent directory
                                                    #because it will not be
                                                    #there to start
            self.__get_directories_recursively(self.pictures_dir, self.pictures)

            self.valid_extensions.append(".ps") #temporarily add postscript to
                                                #acceptable extensions so that the
                                                #function used will not exclude it
                                                #entry is removed later
            picture_files = []
            for directory in self.pictures: #find all files that are not
                                            #subdirectories
                for file in self.__find_files(directory):
                    picture_files.append(file)

            self.valid_extensions.remove(".ps") #remove postscript from
                                                #acceptable extensions because
                                                #it is no longer needed

            self.pictures = picture_files #"self.pictures" now contains
                                          #locations of files

        total_pages = 0
        for picture in self.pictures: #iterate through and parse each file
                                      #looking for a keyword
                                      #keyword appears twice once with
                                      #parenthases and once without, so
                                      #algorithm makes sure it contains no
                                      #parenthases
            file = open(picture, "r")
            for line in file.readlines():
                if "%%Pages: " in line and "(" not in line:
                    total_pages = (total_pages + math.ceil(int(line.split("%%Pages: ")[1].split("\n")[0]) / 2))

        return total_pages

#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Jun  6 15:20:10 2019

@author: aiden
"""

import os
import treelib

import DirectoryTree


class Print(DirectoryTree.DirectoryTree): #inherits DirectoryTree so that
                                          #a directory tree can be made
    """
    contains methods for printing code based on
    "config.txt"
    """
    def __init__(self):
        DirectoryTree.DirectoryTree.__init__(self)

        self.directory_tree = treelib.Tree() #will contain all directories ordered according
                                             #to "config.txt"

        self.swap_tree = treelib.Tree() #used to re-arrange directories in
                                        #other node

        self.parent = "." #will contain parent to append to file paths
                           #in "config.txt"
        self.headers_only = 0 #option set in "config.txt"
        self.headers_first = 1 #option set in "config.txt"

        self.dir_order = [] #will contain order of directories set in "config.txt"
        self.file_order = [] #will contain order of files set in "config.txt"

        self.print_order = [] #will contain paths of all print items in order




    @classmethod
    def __get_file_type(cls, path):
        """
        takes a file converted to postscript and finds what file type it
        is (.cpp, .hpp, .sh)

        @params = path name of file to get the type of
        @return = type string of extension (ex. ".cpp")
                  type string of root path (ex. no directories and file type (CPP) cut off)
                  type string of path without root path
        """
        valid_extensions = [ #files with this extension will be allowed to
                             #be printed
            "PY",
            "C",
            "CPP",
            "HPP",
            "H",
            "SH",
            "TXT",
            "JSON",
            "MD"
            ]

        filename, _ = os.path.splitext(path) #remove extension
        filename = filename.split("/") #remove other directories
        filename = filename[len(filename) - 1]

        one_char = filename[-1:]
        two_char = filename[-2:]
        three_char = filename[-3:]
        four_char = filename[-4:]

        root_name = ""

        if one_char in valid_extensions and two_char not in valid_extensions:
            #added checking for two char as well to fix cases SH and H
            extension = one_char
        elif two_char in valid_extensions:
            extension = two_char
        elif three_char in valid_extensions:
            extension = three_char
        elif four_char in valid_extensions:
            extension = four_char
        else:
            extension = "invalid"

        if extension != "invalid":
            root_name = filename.split(extension)[0]

        root_path = path.split(root_name + extension)[0]

        return extension, root_name, root_path




    def __get_rules(self):
        """
        gets rules based on "config.txt"

        @params = None
        @return = None
        """
        #extract parent from "config.txt"
        with open("config.txt", "r") as config: #file closed at end of block
            for line in config.readlines():
                #makes sure all required elements are in the line
                #and that it is not a comment
                if "parent" in line and "=" in line and "#" not in line:
                    self.parent = line.split("parent")[1].strip().strip('\"')
                    self.parent = self.parent.split("=")[1].strip().strip('\"')

        #extract other parameters from "config.txt"
        param_names = ["HEADERS_FIRST", "ONLY_HEADERS"]
        with open("config.txt", "r") as config: #file closed at end of block
            for line in config.readlines():
                #makes sure all required elements are in the line
                #and that it is not a comment
                if param_names[0] in line and "#" not in line:
                    line = line.split(param_names[0])[1].strip()
                    self.headers_first = int(line)

                elif param_names[1] in line and "#" not in line:
                    line = line.split(param_names[1])[1].strip()
                    self.headers_only = int(line.strip())

        #read rules
        with open("config.txt", "r") as config: #file closed at end of block
            for line in config.readlines():
                if '#' not in line:
                    if "dir" in line:
                        #convert to standard os path
                        directory = self.parent + "/" + (line.split("dir ")[1].rstrip())
                        directory = os.path.normpath(directory)
                        self.dir_order.append(directory)
                    elif "file" in line:
                        #convert to standard os path
                        file = self.parent + "/" + (line.split("file ")[1].rstrip())
                        file = os.path.normpath(file)
                        self.file_order.append(file)




    def __order_directories(self):
        """
        orders directories in "self.directory_tree"
        based on settigns in "config.txt"
        makes new tree stored in "self.swap_tree"

        @params = None
        @return = None
        """
        #iterate through each branch of the tree and sort it
        #moving directories to needed spot in "self.swap_tree"
        #standard path will be made from os module
        #so that no conflicts occur from directory separators
        #a two number string is added to all the tags in the node so that
        #the order is maintained. The order for nodes is in
        #alphabetical order, so no changes would occur if the numbers were
        #not added

        #finds nodes in tree that have children
        parent_nodes = []
        for node in self.directory_tree.all_nodes():
            if not node.is_leaf(): #a leaf has no children
                parent_nodes.append(node)

        #adds the root node to the swap tree
        self.swap_tree.create_node("00" + parent_nodes[0].tag,
                                   parent_nodes[0].identifier)
        parent_nodes.pop(0)

        #sorts childrent of parent node
        #
        #a string of two numbers is added to the tag so that the tree is
        #in order, the id is not affected however
        #this means the max amount of directories in one node can only be 99
        #this should probably not be exceded
        for node in parent_nodes:
            num = 0
            children = self.directory_tree.children(node.identifier)
            try: #adds parent of the node to the tree if it is not already there
                self.swap_tree.create_node("00" + node.tag,
                                           node.identifier,
                                           self.directory_tree.parent(node.identifier).identifier)
            except treelib.exceptions.DuplicatedNodeIdError: #node already exists
                pass

            #get applicable rules
            applicable_rules = [] #contains identifier for nodes in order to
                                  #be sorted
            for rule in self.dir_order:
                dir_length = len(rule.split("/")) #checks to see if amount of
                                                  #directories in path is the
                                                  #same as the amount in the
                                                  #tree
                                                  #if it is, it is added to
                                                  #applicable rules

                offset = len(self.parent.split("/")) #offset is added to the
                                                     #tree level because the
                                                     #rules in "config.txt" do
                                                     #not contain the parent
                                                     #directory
                children_names = list(map(lambda x: x.identifier, children))
                tree_level = self.directory_tree.level(node.identifier) + offset
                if rule in children_names and dir_length == tree_level:
                    applicable_rules.append(rule)

            for child in applicable_rules: #add nodes to the tree that appear
                                           #in the given rules so that the order
                                           #wanted is kept
                num = str(num) #makes sure that "num" is two characters
                if len(num) < 2:
                    num = "0" + str(num)

                nid = self.directory_tree.get_node(child)
                self.swap_tree.create_node(str(num) + nid.tag,
                                           nid.identifier,
                                           node.identifier)
                num = int(num) + 1
                children.remove(self.directory_tree.get_node(child))

            while children: #adds other children to the new tree
                            #order does not matter so they are added in order
                            #of appearance
                num = str(num) #makes sure that "num" is two characters
                if len(num) < 2:
                    num = "0" + num

                self.swap_tree.create_node(str(num) + children[0].tag,
                                           children[0].identifier,
                                           parent=node.identifier)
                num = int(num) + 1
                children.pop(0)




    def __order_headers(self, files):
        """
        takes a list of postscript files and orders it so that headers
        appear before implementation files in the list

        @params = list of postscript files to sort putting headers before
                  implementation file
        @return = sorted list of postscript files
        """
        #create dictionary with key of the root file name and then a value of
        #a list of the extensions with that root name
        file_dict = {}

        for file in files:
            extension, root_name, root_path = self.__get_file_type(file)
            if root_name not in file_dict.keys():
                file_dict.update({root_name : [extension]})
            else: #if key exists then add value to the list
                file_dict[root_name].append(extension)

        #iterate through dictionary placing hpp files ahead of cpp
        for extension_list in file_dict.values():
            extension_list.sort(reverse=self.headers_first)

        #re construct list
        ordered_files = []
        for item in file_dict:
            extensions = file_dict.get(item)
            for extension in extensions:
                if extension != "invalid":
                    if not self.headers_only:
                        file = root_path + item + extension + ".ps"
                        ordered_files.append(file)
                    else: #if only headers are to be printed
                        if extension != "CPP" or len(extensions) == 1:
                            file = root_path + item + extension + ".ps"
                            ordered_files.append(file)

        return ordered_files




    def __order_files(self):
        """
        creates a list of file names in the order that they are to be printed

        @params = None
        @return = None
        """
        #get directories to look through
        directories = list(self.swap_tree.expand_tree())
        ordered_files = [] #holds list of all ordered files

        for directory in directories:
            ordered_directory = [] #holds list of ordered files in a directory

            #gets the files in a given directory
            files = list([f.path for f in os.scandir(directory) if not f.is_dir()])
            files.sort() #makes paths in aphabetical order

            #gets rules for files in a directory
            applicable_rules = []

            for rule in self.file_order:
                rule_directory = rule
                rule_directory = rule_directory.split("/")
                rule_directory.pop(-1)
                rule_directory = "/".join(rule_directory)

                rule_dirs = len(rule.split("/")) - 1

                #add one to the tree level because the level starts at 0 instead
                #of 1 like the method to find the height of other directories
                tree_level = self.swap_tree.level(self.swap_tree.get_node(directory).identifier) + 1

                #if rule applies
                if rule_directory == directory and rule_dirs == tree_level:
                    applicable_rules.append(rule)

            #adds files where a specified order has been given
            for file in applicable_rules:
                ordered_directory.append(file)
                files.remove(file)

            #adds rest of files
            while files:
                ordered_directory.append(files[0])
                files.pop(0)

            #sorts so that headers are first if that option
            #has been given
            #also checks if only headers will be printed
            ordered_directory = self.__order_headers(ordered_directory)

            #adds item in the ordered files in a directory to all the ordered
            #files
            for item in ordered_directory:
                ordered_files.append(item)


        #adds items in ordered_files to the final print order if the
        #extension is .ps
        for item in ordered_files:
            _, extension = os.path.splitext(item)
            if extension == ".ps":
                self.print_order.append(item)



    def order(self):
        """
        orders code based on "config.txt" with all the parameters set in
        it

        @params = None
        @return = type list of files to print in order
        """
        #reset attributes
        self.directory_tree = treelib.Tree() #will contain all directories ordered according
                                             #to "config.txt"
        self.swap_tree = treelib.Tree() #used to re-arrange directories in
                                        #other node
        self.headers_only = 0 #option set in "config.txt"
        self.headers_first = 1 #option set in "config.txt"
        self.dir_order = [] #will contain order of directories set in "config.txt"
        self.file_order = [] #will contain order of files set in "config.txt"
        self.print_order = [] #will contain paths of all print items in order

        #order code functions
        self.__get_rules()
        self.directory_tree, _ = self.return_directory_tree(self.parent)

        self.__order_directories()
        self.__order_files()

        return self.print_order




    def print_code(self, files=None):
        """
        prints code by file and also barriers if they are in the list

        @params = (optional) type list of path of files to print
        @return = None
        """

        if files:
            self.print_order = files

        for file in self.print_order: #adds print job to the printer queue
                                      #with shell command
            print("queuing ", file)
            command = "lpr -P HP-Color-LaserJet-M553 -o sides=two-sided-long-edge " + str(file)
            os.system(command)

    def output_pdf(self, files=None):
        """
        merges code by file and also barriers if they are in the list

        @params = (optional) type list of path of files to print
        @return = None
        """     
        
        if not files:
            files - self.print_order
            
        command = "psmerge " + " ".join(files) + " > all.ps; ps2pdf all.ps; rm all.ps"
        print(command)
        #os.system(command)
        

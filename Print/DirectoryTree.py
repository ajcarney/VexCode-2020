#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Jun  7 10:22:26 2019

@author: aiden
"""

import os
import treelib


class DirectoryTree:
    """
    contains methods for making a treelib tree structure of a
    given directory tree
    meant to be inherited but can be a stand alone class

    @params = None
    @return = None
    """

    def __init__(self):
        self.__directories = [] #will contain all directories and subdirectories
        self.__directory_tree = treelib.Tree() #will contain all directories ordered according
                                             #to "config.txt"



    def __get_dirs(self, parent_directory, storage_list):
        """
        recursively prints all files in parent directory
        put in list given

        @params = directory to start at, directory to write output to
        @return = None
        """

        dirs = [f.path for f in os.scandir(parent_directory) if f.is_dir()]

        for directory in dirs:
            storage_list.append(directory)
            self.__get_dirs(directory, storage_list)




    def __get_all(self, parent_directory, storage_list):
        """
        recursively prints all files in parent directory
        put in list given

        @params = directory to start at, directory to write output to
        @return = None
        """

        dirs = [f.path for f in os.scandir(parent_directory) if f.is_dir()]
        files = [f.path for f in os.scandir(parent_directory)]

        for file in files:
            storage_list.append(file)

        for directory in dirs:
            storage_list.append(directory)
            self.__get_all(directory, storage_list)



    def __make_tree(self):
        """
        add directories found to a treelib structure
        the parent directory is set in "config.txt"

        @params = None
        @return = None
        """
        depth = self.__directory_tree.depth()

        #splits path of directory and makes nodes of partial paths until
        #it reaches the end of the path given
        #the resulting tree is saved in "self.__directory_tree"
        for path in self.__directories:
            path_split = path.split("/")

            i = depth

            while i < len(path_split):
                try:
                    if i > 0:
                        name = path_split[i]
                        node_id = "/".join(path_split[0:i + 1])
                        parent = "/".join(path_split[0:i])
                    else:
                        name = path_split[i]
                        node_id = path_split[i]
                        parent = None

                    self.__directory_tree.create_node(tag=name, identifier=node_id, parent=parent)

                except treelib.exceptions.NodeIDAbsentError:
                    pass
                except treelib.exceptions.DuplicatedNodeIdError:
                    pass

                i = i + 1



    def return_directory_tree(self, parent_directory):
        """
        makes and returns a treelib tree structure of a directory
        tree based on a parent directory given only directories are
        included

        @params = string of parent directory to start node at
        @return = treelib tree structure of directory tree,
                  list of directories found
        """
        self.__directory_tree = treelib.Tree()
        self.__directories = []

        self.__get_dirs(parent_directory, self.__directories)
        self.__make_tree()

        return self.__directory_tree, self.__directories



    def return_tree(self, parent_directory):
        """
        makes and returns a treelib tree structure of a directory
        tree based on a parent directory given
        everything is included

        @params = string of parent directory to start node at
        @return = treelib tree structure of directory tree,
                  list of directories found
        """
        self.__directory_tree = treelib.Tree()
        self.__directories = []

        self.__get_all(parent_directory, self.__directories)
        self.__make_tree()

        return self.__directory_tree, self.__directories

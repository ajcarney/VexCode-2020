#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Jun 11 14:26:00 2019

@author: aiden
"""

import copy
import os

import PostScript
import Print
import DirectoryTree


print("starting print process")
print("please be very careful because there is a lot of code")
print()


help_dict = {
    "help":"print help",
    "postscript":"convert files to postscript",
    "page_count":"prints amount of pages that code will be",
    "order":"prepare order to print in",
    "show_order":"shows current order of files to be printed in",
    "tree":"add cover sheet of tree to files",
    "print":"prints code",
    "cancel":"cancels all printer jobs",
    "status":"shows current print jobs",
    "exit":"ends session"
    }


ps = PostScript.PostScript()
printer = Print.Print()
Tree = DirectoryTree.DirectoryTree()


print_order = []


while 1:
    command = input("enter command ")
    
    if command.upper() == "HELP":
        print()

        for item in help_dict:
            string = item
            while len(string) < 20: #aligns help options with each other
                string = string + "."
            print(string, help_dict.get(item), sep=".")


    elif command.upper() == "POSTSCRIPT":
        ps.prepare_code()

    elif command.upper() == "PAGE_COUNT":
        if not print_order:
            pages = ps.get_page_count()
        else:
            pages = ps.get_page_count(print_order)

        print("number of pages: ", pages)

    elif command.upper() == "ORDER":
        if len(print_order) > 1:
            print_order = []
            
        to_add = printer.order()
        
        for item in to_add:
            print_order.append(item)
        print()
        print("please review this order carefully before printing")
        print("do not waste paper")
        print()

        for item in print_order:
            print(item)


    elif command.upper() == "SHOW_ORDER":
        for item in print_order:
            print(item)


    elif command.upper() == "TREE":
        file_tree, _ = Tree.return_tree("..")
        filtered_tree = copy.deepcopy(file_tree)
        
        #sorts out nodes that are not being printed in three ways
        #Tree is copies to a new tree so that the tree does not
        #change during iteration
        #original is looked through and changes are made to the copy
        for node in file_tree.all_nodes():
            nid = node.identifier
            if os.path.isdir(nid):  #sort out nodes by directory
                if nid in ps.directory_exceptions and filtered_tree.get_node(nid):
                    filtered_tree.remove_node(nid)
            else:
                _, extension = os.path.splitext(nid)
                
                #sort out nodes by file
                if nid in ps.file_exceptions and filtered_tree.get_node(nid):
                    filtered_tree.remove_node(nid)
                    
                #sort out nodes by extension
                elif extension not in ps.valid_extensions and filtered_tree.get_node(nid):
                    filtered_tree.remove_node(nid)
                    
        filtered_tree.get_node("..").tag = "VexCode-2019"
        filtered_tree.show()
        os.remove("tree.rtf")

        filtered_tree.save2file("tree.rtf")
        filtered_tree.save2file("tree.txt")
        print_order.insert(0, "tree.rtf")


    elif command.upper() == "PRINT":
        printer.print_code(print_order)

    elif command.upper() == "CANCEL":
        os.system("lprm -P HP-Color-LaserJet-M553 -")

    elif command.upper() == "STATUS":
        os.system("lpstat -P HP-Color-LaserJet-M553")

    elif command.upper() == "EXIT":
        break

    else:
        print("invalid command")
        
    print()
        

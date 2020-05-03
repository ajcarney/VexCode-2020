#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Aug 15 10:16:03 2019

@author: aiden
"""
import inspect
import colorama
import fcntl
import os
import readline
import struct
import termios

import cpp_types
import config
import exceptions


class HeaderGen:
    """
    Wrapper class for working with cpp types and header files
    used to easily generate code based on user input
    """
    def __init__(self, header_obj):
        self.header = header_obj
        self.children = header_obj.get_children()
        self.focus = header_obj
        self.current_type = "header"
        self.loc = "./" + self.header.file_name

        self.commands = {
            "ls":self.__ls,
            "view":self.__view,
            "cd":self.__change_focus,
            "exit":self.__exit,
            "new":self.__new,
            "write":self.__write,
            "add":self.__add,
            "help":self.__help
            }


    @classmethod
    def __exit(cls, *_):
        """
        exit function for shell
        """
        raise exceptions.Exit

    def __ls(self, *_):
        """
        lists data on focused object
        """
        print(self.focus.list_data() + "\n")

    def __view(self, *args):
        """
        shows the generated text of the focused object
        param bool header_text sets the view to either
        the generated header text or the generated implementation
        file text
        """
        if args[0] == []:
            header_text = True
        elif str(args[0][0]).upper() == "HEADER":
            header_text = True
        elif str(args[0][0]).upper() == "IMPLEMENTATION":
            header_text = False
        else:
            raise exceptions.UnknownOption

        text = self.focus.gen_header_text() if header_text else self.focus.gen_impl_text()
        print(text + "\n")

    def __change_focus(self, *args):
        """
        changes focus to user specified input
        no return
        """
        name = args[0][0]
        if name == "..":
            self.focus = self.focus.parent
            self.update_type(self.focus)
            self.children = self.focus.get_children()
        elif self.focus.has_children:
            if name in self.children.keys():
                self.focus = self.children.get(name)
                self.update_type(self.focus)
                self.children = self.focus.get_children()
            else:
                print("invalid selection")
        else:
            print("focused object has no children")


        path = [self.focus]
        while len(path) == len(set(path)):
            path.insert(0, path[0].parent)
        path.pop(0)
        path.pop(0)
        self.loc = "./" + self.header.file_name + "/" + "/".join(x.name for x in path)



    def __new_class(self, name):
        """
        adds a class object to a header file object
        param name = type str of name of the class

        throws invalid addition if not currently focused on header object
        """
        if self.current_type == "header":
            obj = cpp_types.cpp_class(name, self.focus)
            self.header.classes.append(obj)
        else:
            raise exceptions.InvalidAddition


    def __new_func(self, loc, return_type, name, static=False):
        """
        adds a function object to a header file or class object
        param name = type str of name of the function
        param return_type = type of function
        param static = is the function static or not

        throws invalid addition if incorrect params are passed
        """
        obj = cpp_types.cpp_func(name, return_type, static, self.focus)

        if self.current_type == "header":
            self.focus.funcs.append(obj)
        elif self.current_type == "class":
            if static:
                if loc.upper() == "PUBLIC":
                    self.focus.public["static_func"].append(obj)
                elif loc.upper() in ["PROT", "PROTECTED"]:
                    self.focus.protected["static_func"].append(obj)
                else:
                    self.focus.private["static_func"].append(obj)
            else:
                if loc.upper() == "PUBLIC":
                    self.focus.public["func"].append(obj)
                elif loc.upper() in ["PROT", "PROTECTED"]:
                    self.focus.protected["func"].append(obj)
                else:
                    self.focus.private["func"].append(obj)
        else:
            raise exceptions.InvalidAddition

    def __new_var(self, loc, var_type, name, static=False):
        """
        adds a function object to a header file or class object
        param name = type str of name of the function
        param var_type = type of variable
        param static = is the variable static or not

        throws invalid addition if incorrect params are passed
        """
        obj = cpp_types.cpp_variable(name, var_type, static, self.focus)

        if self.current_type == "header":
            self.focus.static_vars.append(obj)
        elif self.current_type == "class":
            if static == "static":
                if loc.upper() == "PUBLIC":
                    self.focus.public["static_var"].append(obj)
                elif loc.upper() in ["PROT", "PROTECTED"]:
                    self.focus.protected["static_var"].append(obj)
                else:
                    self.focus.private["static_var"].append(obj)
            else:
                if loc.upper() == "PUBLIC":
                    self.focus.public["var"].append(obj)
                elif loc.upper() in ["PROT", "PROTECTED"]:
                    self.focus.protected["var"].append(obj)
                else:
                    self.focus.private["var"].append(obj)
        else:
            raise exceptions.InvalidAddition



    def __new_include(self, include_type, name):
        """
        adds an include to a header file
        param type (lib, user) - type of include
        param name - name of include

        throws invalid addition if incorrect params are passed
        """
        if include_type == "user":
            self.header.user_includes.append(name.strip())
        elif include_type == "lib":
            self.header.lib_includes.append(name.strip())
        else:
            raise exceptions.InvalidAddition



    def __new(self, *args):
        """
        adds a type of object to another type of object
        checks to see if addition is valid
        ex. if creating a class the parent must be of type header

        throws InvalidAddition if the addition failed
        """
        func_dict = {
            "class":self.__new_class,
            "var":self.__new_var,
            "func":self.__new_func,
            "function":self.__new_func,
            "include":self.__new_include
            }

        if not args[0]:
            raise exceptions.InvalidAddition("invalid parameters were passed")

        obj_type = args[0][0].strip()
        params = list(map(lambda x: x.strip(), args[0][1:]))

        func = func_dict.get(obj_type)
        if func:
            num_args = len(inspect.signature(func).parameters)
            if num_args > len(params):
                params.insert(0, "")
            while num_args > len(params):
                params.append("")

        elif func_dict.get(args[0][1].strip()): #if argument in second position
                                                #is a valid command then switch
                                                #param in first position to front
                                                #of params list
            func = func_dict.get(args[0][1].strip())
            params = list(map(lambda x: x.strip(), args[0][2:]))

            num_args = len(inspect.signature(func).parameters)
            if num_args > len(params):
                params.insert(0, args[0][0].strip())
            while num_args > len(params):
                params.append("")

        else:
            raise exceptions.InvalidAddition("invalid function call")

        func(*params[:num_args])
        self.children = self.focus.get_children()



    def __add_function_param(self, param_type, param_name):
        """
        adds parameters to a function
        @param param_type - type str of the cpp type
        @param param_name - name of the parameter
        throws Invalid Addition on failure or invalid params
        """
        if self.current_type != "function":
            raise exceptions.InvalidAddition

        param = param_type.strip() + " " + param_name.strip()
        self.focus.params.append(param)


    def __add(self, *args):
        """
        used to add attributes such as parameters to a function

        throws invalid addition if the addition failed
        """
        func_dict = {
            "param":self.__add_function_param
            }

        if not args[0]:
            raise exceptions.InvalidAddition("invalid parameters were passed")

        obj_type = args[0][0].strip()
        params = list(map(lambda x: x.strip(), args[0][1:]))

        func = func_dict.get(obj_type)

        if func:
            num_args = len(inspect.signature(func).parameters)
            while num_args > len(params):
                params.append("")

            func(*params[:num_args])
            self.children = self.focus.get_children()

        else:
            raise exceptions.InvalidAddition("invalid function call")




    def __write(self, *_):
        """
        writes the text generated from the header file into an actual file
        as well as generating and writing the text for an implementation
        file
        """
        header_file_name = self.header.file_name
        impl_file_name, _ = os.path.splitext(self.header.file_name)
        impl_file_name += ".cpp"

        with open(header_file_name, "a") as file:
            file.write(self.header.gen_header_text())

        with open(impl_file_name, "a") as file:
            file.write(self.header.gen_impl_text())




    def __help(self, *_):
        """
        prints docstrings for each function
        """
        #get terminal size to set max chars per line
        _, columns, _, _ = struct.unpack('HHHH',
                                         fcntl.ioctl(0, termios.TIOCGWINSZ,
                                         struct.pack('HHHH', 0, 0, 0, 0)))

        max_chars = columns - 5
        help_msg = ""
        spaces = len(max(list(self.commands.keys()), key=len))

        for key in self.commands:
            doc_str = str(self.commands.get(key).__doc__).strip().replace("\n", " ")
            words = doc_str.split(" ")
            words = list(filter(lambda a: a != "", words))

            line = key + (" " * (spaces - len(key))) + "  -  "
            indentation = len(line) * " "
            for word in words:
                if (len(word) + len(line)) < max_chars:
                    line += word + " "
                else:
                    help_msg += line + "\n"
                    line = indentation + word + " "
            help_msg += line + "\n\n"

        print(help_msg)




    def update_type(self, obj):
        """
        updates self.current_type to the type of obj
        """
        types = {cpp_types.cpp_class:"class",
                 cpp_types.cpp_func:"function",
                 cpp_types.HeaderFile:"header",
                 cpp_types.cpp_variable:"variable"
                }
        self.current_type = types.get(type(obj))




    def execute_command(self, command):
        """
        executes a command from the given api
        api commands are stored in self.commands
        """
        cmd = self.commands.get(command.split(" ")[0].strip())
        args = command.split(" ")[1:]
        for arg in args:
            arg.strip()

        try:
            if not cmd:
                raise exceptions.UnknownOption
            cmd(args)
        except exceptions.UnknownOption:
            pass
        except exceptions.InvalidAddition:
            pass



#TODO: add dynamically changing autocomplete
class Shell:
    """
    contains shell like interface for generating code
    """
    def __init__(self):
        self.loc = "./"
        self.functions = sorted(["new",
                                 "class",
                                 "edit",
                                 "ls",
                                 "include",
                                 "user",
                                 "lib",
                                 "func",
                                 "view",
                                 "exit",
                                 "add"])


    def auto_complete(self, text, state):
        """
        function that will attempt to autocomplete user input
        on <tab> to s member in self.functions
        returns type str of first matched string
        """
        if state == 0:  # on first trigger, build possible matches
            if text:  # cache matches (entries that start with entered text)
                matches = [s for s in self.functions if s and s.startswith(text)]
            else:  # no text entered, all matches possible
                matches = self.functions[:]

        # return match indexed by state
        try:
            return matches[state]
        except IndexError:
            return None


    def get_command(self):
        """
        gets command from user
        returns type str of command
        """
        print("")

        colorama.init()
        readline.set_completer(self.auto_complete)
        readline.parse_and_bind('tab: complete')

        command = input((config.SHELL_COLOR
                         + "\n"
                         + config.CURSOR_UP_ONE
                         + config.ERASE_LINE
                         + self.loc + " <command> "
                         + colorama.Fore.RESET))
        return command



#def unit_test():
#    h = cpp_types.HeaderFile("MyHeader.hpp")
#
#    c = cpp_types.cpp_class("MyClass")
#    c.protected["func"].append(cpp_types.cpp_func("my_func", "int", 1))
#    c.public["var"].append("int x")
#    h.static_vars.append("int y")
#    h.classes.append(c)
#
#    t = h.gen_header_text()
#    print(t)




file_name = input(config.SHELL_COLOR
                  + "\n"
                  + config.CURSOR_UP_ONE
                  + config.ERASE_LINE
                  + "Enter name of Header File to Create "
                  + colorama.Fore.RESET)

if not any(s in file_name and s[-len(s):] == file_name[-len(s):] for s in ['.hpp', '.h']):
    raise exceptions.InvalidFileName



header = cpp_types.HeaderFile(file_name)
s = Shell()
s.loc += file_name + "/"
header_gen = HeaderGen(header)


while 1:
    try:
        try:
            usr_command = s.get_command()
        except KeyboardInterrupt:
            print()
            raise exceptions.Exit

        header_gen.execute_command(usr_command)
        s.loc = header_gen.loc

    except exceptions.Exit:
        break



#unit_test()

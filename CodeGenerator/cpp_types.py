#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Aug 17 19:52:41 2019

@author: aiden
"""
import os
import config

class cpp_class:
    """
    contains methods and data for a cpp class
    generates text for header and implementation files
    """
    def __init__(self, name, parent):
        self.name = name

        self.has_children = True
        self.parent = parent

        self.private = {
            "func":[],
            "static_func":[],
            "var":[],
            "static_var":[]
            }
        self.protected = {
            "func":[],
            "static_func":[],
            "var":[],
            "static_var":[]
            }
        self.public = {
            "func":[],
            "static_func":[],
            "var":[],
            "static_var":[]
            }



    @classmethod
    def __keys_empty(cls, section):
        """
        checks if all keys in a dictionary have no value
        returns True if empty, False otherwise
        """
        for key in section:
            if section.get(key):
                return False

        return True




    def list_data(self):
        """
        lists the data in the class by category
        returns type str of data
        """
        text = ""
        sections = [self.private,
                    self.protected,
                    self.public]
        section_names = ["private:\n",
                         "protected:\n",
                         "public:\n"]

        for i, section in enumerate(sections):
            text += section_names[i] + "\tstatic variables\n"
            for static_var in section.get("static_var"):
                text += "\t\t" + static_var.var_type + " " + static_var.name + "\n"

            text += "\tvariables\n"
            for var in section.get("var"):
                text += "\t\t" + var.var_type + " " + var.name + "\n"

            text += "\tstatic functions\n"
            for static_func in section.get("static_func"):
                text += "\t\t" + static_func.type + " " + static_func.name + "\n"

            text += "\tfunctions\n"
            for func in section.get("func"):
                text += "\t\t" + func.type + " " + func.name + "\n"

        return text




    def get_children(self):
        """
        returns a dict of children names and their object
        ex. {name1:obj1,
             name2:obj2,
             name3:obj3,
             ...}
        """
        children = {}
        sections = [self.private, self.protected, self.public]

        for section in sections:
            for static_var in section.get("static_var"):
                children.update({static_var.name:static_var})

            for var in section.get("var"):
                children.update({var.name:var})

            for static_func in section.get("static_func"):
                children.update({static_func.name:static_func})

            for func in section.get("func"):
                children.update({func.name:func})

        return children


    def gen_header_text(self):
        """
        takes data in the class and generates text for a class in
        a header file
        returns type str of text
        """
        text = "class " + self.name + "\n{\n\tprivate:\n"

        #TODO: condense, function too long, too many branches
        for member in self.private.get("static_func"):
            text += "\t\t" + member.gen_header_text()
        if self.private.get("static_func"):
            text += "\n\n"

        for member in self.private.get("func"):
            text += "\t\t" + member.gen_header_text()
        if self.private.get("func"):
            text += "\n\n"

        for member in self.private.get("static_var"):
            text += "\t\t" + member.gen_header_text()
        if self.private.get("static_var"):
            text += "\n\n"

        for member in self.private.get("var"):
            text += "\t\t" + member.gen_header_text()
        if self.private.get("var"):
            text += "\n\n"


        prot_txt = ""
        if not self.__keys_empty(self.protected):
            #if not empty
            prot_txt = "\tprotected:\n"
            for member in self.protected.get("static_func"):
                prot_txt += "\t\t" + member.gen_header_text()
            if self.protected.get("static_func"):
                prot_txt += "\n\n"

            for member in self.protected.get("func"):
                prot_txt += "\t\t" + member.gen_header_text()
            if self.protected.get("func"):
                prot_txt += "\n\n"

            for member in self.protected.get("static_var"):
                prot_txt += "\t\t" + member.gen_header_text()
            if self.protected.get("static_var"):
                prot_txt += "\n\n"

            for member in self.protected.get("var"):
                prot_txt += "\t\t" + member.gen_header_text()
            if self.protected.get("var"):
                prot_txt += "\n\n"
        elif self.__keys_empty(self.protected) and not config.REMOVE_PROTECTED_IF_EMPTY:
            #if empty and dont remove protected
            prot_txt = "\tprotected:\n"

        text += prot_txt + "\tpublic:\n\t\t" + self.name + "();\n\t\t~" + self.name + "();\n\n"


        for member in self.public.get("static_func"):
            text += "\t\t" + member.gen_header_text()
        if self.public.get("static_func"):
            text += "\n\n"

        for member in self.public.get("func"):
            text += "\t\t" + member.gen_header_text()
        if self.public.get("func"):
            text += "\n\n"

        for member in self.public.get("static_var"):
            text += "\t\t" + member.gen_header_text()
        if self.public.get("static_var"):
            text += "\n\n"

        for member in self.public.get("var"):
            text += "\t\t" + member.gen_header_text()
        if self.public.get("var"):
            text += "\n\n"

        text += "};\n"

        tab = ""
        for _ in range(config.TAB_SIZE):
            tab += " "
        text = text.replace("\t", tab)

        return text




    def gen_impl_text(self):
        """
        generates text for a class in an implementation file
        returns type str of text
        """
        text = ""
        
        sections = [self.private, self.protected, self.public]
        for section in sections:
            for static_var in section.get("static_var"):
                text += static_var.gen_impl_text()

        text += "\n\n\n" + self.name + "::" + self.name + "( )\n{\n\n}\n"
        text += "\n\n\n" + self.name + "::~" + self.name + "( )\n{\n\n}\n\n\n\n"

        for section in sections:
            for static_func in section.get("static_func"):
                text += static_func.gen_impl_text() + "\n\n\n"
                
            for func in section.get("func"):
                text += func.gen_impl_text() + "\n\n\n"

        return text




class cpp_func:
    """
    contains data about a cpp function type
    can be either static or not and has methods to
    generate text in both header and implementation file
    """
    def __init__(self, name, return_type, static, parent):
        self.type = return_type
        self.name = name
        self.static = bool(static)

        self.has_children = False
        self.parent = parent

        self.params = []




    def get_children(self):
        """
        returns a dict of children names and their object
        ex. {name1:obj1,
             name2:obj2,
             name3:obj3,
             ...}
        since there are no accesible children, an empty
        list is returned
        """
        return {}



    def list_data(self):
        """
        lists the data in the function by category
        returns type str of data
        """
        text = "type:\n\t" + self.type + "\nname:\n\t" + self.name + "\nstatic:\n\t"
        text += "yes\n" if self.static else "no\n"
        text += "parameters:\n"
        for param in self.params:
            text += "\t" + param + "\n"

        return text




    def gen_header_text(self):
        """
        generates text for a function with given dataset to be placed
        in a header file
        returns type str of text for header file
        """
        text = ""
        if self.static:
            text += "static "

        text += self.type + " "
        text += self.name + "( "
        for param in self.params:
            text += param + ", "
        k = text.rfind(",")
        if k > 0:
            text = text[:k] + text[k+1:]
        text += ");\n"

        return text




    def gen_impl_text(self, class_name=""):
        """
        generates text for a function with a given dataset to be placed
        in an implementation file
        returns type str of text for implementation file
        """
        text = ""

        text += self.type + " "
        if class_name:
            text += class_name + "::" + self.name + "( "
        else:
            text += self.name + "( "

        for param in self.params:
            text += param + ", "
        k = text.rfind(",")
        if k > 0:
            text = text[:k] + text[k+1:]

        text += ")\n{\n\n}"

        return text




class cpp_variable:
    """
    contains data about a cpp variable type
    can be either static or not and has methods to
    generate text in both header and implementation file
    """
    def __init__(self, name, var_type, static, parent):
        self.var_type = var_type
        self.name = name
        self.static = bool(static)

        self.has_children = False
        self.parent = parent



    def get_children(self):
        """
        returns a dict of children names and their object
        ex. {name1:obj1,
             name2:obj2,
             name3:obj3,
             ...}
        since there are no accesible children, an empty
        list is returned
        """
        return {}




    def list_data(self):
        """
        lists the data in the variable by category
        returns type str of data
        """
        text = "type:\n\t" + self.var_type + "\nname:\n\t" + self.name + "\nstatic:\n\t"
        text += "yes\n" if self.static else "no\n"

        return text




    def gen_header_text(self):
        """
        generates text for a variable with given dataset to be placed
        in a header file
        returns type str of text for header file
        """
        text = "static " if self.static else ""

        text += self.var_type + " " + self.name + ";"

        return text




    def gen_impl_text(self):
        """
        generates text for a variable with a given dataset to be placed
        in an implementation file
        returns type str of text for implementation file
        """
        return self.var_type + " " + self.name + " = ;\n"





class HeaderFile:
    """
    contains data for a cpp header file
    data can also be used to generate an implementation file
    """
    def __init__(self, file_name):
        self.file_name = file_name

        self.parent = self
        self.has_children = True

        self.user_includes = []
        self.lib_includes = []
        self.classes = []
        self.static_vars = []
        self.funcs = []




    def list_data(self):
        """
        lists the data in the header file by category
        returns type str of data
        """
        text = "file name:\n\t" + self.file_name + "\nlibrary includes:\n"
        for include in self.lib_includes:
            text += "\t" + include + "\n"

        text += "user includes:\n"
        for include in self.user_includes:
            text += "\t" + include + "\n"

        text += "classes:\n"
        for cpp_cls in self.classes:
            text += "\t" + cpp_cls.name + "\n"

        text += "variables:\n"
        for static_var in self.static_vars:
            text += "\t" + static_var.gen_header_text() + "\n"

        text += "functions:\n"
        for func in self.funcs:
            text += "\t" + func.type + " " + func.name + "\n"

        return text





    def get_children(self):
        """
        returns a dict of children names and their object
        ex. {name1:obj1,
             name2:obj2,
             name3:obj3,
             ...}
        """
        children = {}

        for cpp_cls in self.classes:
            children.update({cpp_cls.name:cpp_cls})

        for static_var in self.static_vars:
            children.update({static_var.name:static_var})

        for func in self.funcs:
            children.update({func.name:func})

        return children




    def gen_header_text(self):
        """
        generates text for a header file
        returns type str of text
        """
        #add header guards
        guard, _ = os.path.splitext(self.file_name)
        guard = guard.split("/")[-1]

        text = "#ifndef __" + guard.upper() + "_HPP__\n"
        text += "#define __" + guard.upper() + "_HPP__\n\n"

        for include in sorted(self.lib_includes):
            text += "#include <" + include + ">\n"

        text += '\n#include "main.h"\n\n'

        for include in sorted(self.user_includes):
            text += '#include "' + include + '"\n'

        text += "\n\n"

        for c in self.classes:
            text += c.gen_header_text() + "\n\n\n\n"

        for func in self.funcs:
            text += func.gen_header_text() + "\n"
        if self.funcs:
            text += "\n\n\n\n"

        for var in self.static_vars:
            text += var.gen_header_text()
        if self.static_vars:
            text += "\n\n\n\n"

        text += "#endif\n"

        return text




    def gen_impl_text(self):
        """
        generates text for an implementation file
        returns type str of text
        """
        text = ""
        for include in sorted(self.lib_includes):
            text += "#include <" + include + ">\n"

        text += '\n#include "main.h"\n\n'

        text += '#include "' + self.file_name + '"\n'

        for include in sorted(self.user_includes):
            text += '#include "' + include + '"\n'

        text += "\n\n"

        for var in self.static_vars:
            text += var.gen_impl_text()
        if self.static_vars:
            text += "\n\n\n\n"

        for c in self.classes:
            text += c.gen_impl_text() + "\n\n\n\n"

        for func in self.funcs:
            text += func.gen_impl_text()
        if self.funcs:
            text += "\n\n\n\n"

        return text

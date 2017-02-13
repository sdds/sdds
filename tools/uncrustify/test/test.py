"""
This File tests the uncrustify conf
1. takes all files in the "1" folder and indents it with gnu-indent to the "2" folder
2. takes all filed in the "2" folder and indents it with the uncrustify conf to the "3" folder
"""

from os import walk
import subprocess
import os


def files_in_dir(path):
    f = []
    for (dirpath, dirnames, filenames) in walk(path):
        f.extend(filenames)
    return f

def indent(file, method):
    os.system('indent ' + method + ' ./1/' + file + ' -o ./2/' + method[1:3] + file)

def uncrustify(file):
    os.system('uncrustify -c sdds_style.cfg -f ./2/' + file + ' > ./3/' + file)

def file_equals(a, b):
    texta = open(a, 'r').read()
    textb = open(b, 'r').read()
    return texta == textb

def main():
    os.system("rm ./2/* ./3/*")
    files = files_in_dir("./1")
    for file in files:
        indent(file, "-gnu")
        indent(file, "-kr")
        indent(file, "-orig")
        os.system("cp ./1/" + file + " ./2/cp" + file)

    files = files_in_dir("./2")
    for file in files:
        uncrustify(file)

    files = files_in_dir("./3")
    for file in files:
        if not file_equals("./1/" + file[2:], "./3/" + file):
            print ("File not equals: " + file)
            print ("$ diff ./1/" + file[2:] + " ./3/" + file)

if __name__ == "__main__":
    main()


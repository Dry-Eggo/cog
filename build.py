#! /usr/bin/python3

from pathlib import Path
import os
import subprocess
from sys import argv as args


CC = "clang"
CXX = "clang++"
SRCDIR = Path("compiler")
JUVEDIR = Path("juve")
INCDIR = Path("compiler/includes")
CFILES = [f for f in SRCDIR.rglob("*.c")]
JUVECFILES = [f for f in JUVEDIR.rglob("*.c")]
JUVECPPFILES = [f for f in JUVEDIR.rglob("*.cpp")]
JUVESRC = JUVECFILES + JUVECPPFILES
OUTDIR = Path("bin")
JUVE   = "libjuve.a"
LIBJUVE = Path(f"bin/{JUVE}")
OBJECTS = []
FLAGS = "-Wall -Wextra -pedantic -Wno-gnu-zero-variadic-macro-arguments -ggdb"
TARGET = Path("bin/kudoc")


BUILD_ALL = False

def build_juve():
    for src in JUVESRC:
        obj_file = OUTDIR / (src.stem + ".o")
        OBJECTS.append(str(obj_file))
        if (src.stat().st_mtime > obj_file.stat().st_mtime) or BUILD_ALL:
            if (src.suffix == ".c"):            
                command = [CC, "-c", str(src), "-o", str(obj_file), f"-I{str(INCDIR)}", f"-I{str(SRCDIR)}", "-I."]
                print(f"Running: {" ".join(command)}")
                subprocess.run(command)
            elif (src.suffix == ".cpp"):
                command = [CXX, "-c", str(src), "-o", str(obj_file), f"-I{str(INCDIR)}", f"-I{str(SRCDIR)}", "-I."]
                print(f"Running: {" ".join(command)}")
                subprocess.run(command)                
    if (src.stat().st_mtime > obj_file.stat().st_mtime) or BUILD_ALL:
        command = f"ar -rcs {str(LIBJUVE)} {" ".join(OBJECTS)}"
        print(f"Building {LIBJUVE}")
        subprocess.run(command, shell=True)
    
def build_obj_files():
    for src in CFILES:
        basename = src.stem
        obj_file = OUTDIR / (src.stem + ".o")
        OBJECTS.append(str(obj_file))
        if BUILD_ALL or (src.stat().st_mtime > obj_file.stat().st_mtime):
            if (src.suffix == ".c"):            
                command =  [CC, "-c" , str(src), "-o", str(obj_file), f"-I{str(INCDIR)}", f"-I{str(SRCDIR)}", "-I."];
                subprocess.run(command)
                print(f"Running: {" ".join(command)}")                
            elif (src.suffix == ".cpp"):
                command =  [CXX, "-c" , str(src), "-o", str(obj_file), f"-I{str(INCDIR)}", f"-I{str(SRCDIR)}", "-I."];
                subprocess.run(command)
                print(f"Running: {command}")                
def build():

    if (len(args) > 1 and args[1] == "all"):
        global BUILD_ALL
        BUILD_ALL = True
        
    build_juve()
    build_obj_files()

    command = f"{CXX} {" ".join(OBJECTS)} -o {TARGET} {FLAGS} -I{str(INCDIR)} -I{str(SRCDIR)} -I."
    print(f"Building {TARGET}")
    subprocess.run(command, shell=True)
    
    pass

if __name__ == "__main__":
    build()

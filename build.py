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
FLAGS = f"-I{str(INCDIR)} -I{str(SRCDIR)} -I. -Wall -Wextra -pedantic -Wno-gnu-zero-variadic-macro-arguments -ggdb"
TARGET = Path("bin/cogc")


BUILD_ALL = False

def build_juve():
    for src in JUVESRC:
        obj_file = OUTDIR / (src.stem + ".o")
        OBJECTS.append(str(obj_file))
        try:
            if (src.stat().st_mtime > obj_file.stat().st_mtime) or BUILD_ALL:
                if (src.suffix == ".c"):            
                    command = f"{CC} -c {str(src)} -o {str(obj_file)} {FLAGS}"
                    print(f"{command}")
                    subprocess.run(command, shell=True)
                elif (src.suffix == ".cpp"):
                    command = f"{CXX} -c {str(src)} -o {str(obj_file)} {FLAGS}"
                    print(f"{command}")
                    subprocess.run(command, shell=True)
        except FileNotFoundError:
                if (src.suffix == ".c"):            
                    command = f"{CC} -c {str(src)} -o {str(obj_file)} {FLAGS}"
                    print(f"{command}")
                    subprocess.run(command, shell=True)
                elif (src.suffix == ".cpp"):
                    command = f"{CXX} -c {str(src)} -o {str(obj_file)} {FLAGS}"
                    print(f"{command}")
                    subprocess.run(command, shell=True)
                    
    if (src.stat().st_mtime > obj_file.stat().st_mtime) or BUILD_ALL:
        command = f"ar -rcs {str(LIBJUVE)} {" ".join(OBJECTS)}"
        print(f"Building {LIBJUVE}")
        subprocess.run(command, shell=True)
    
def build_obj_files():
    for src in CFILES:
        basename = src.stem
        obj_file = OUTDIR / (src.stem + ".o")
        OBJECTS.append(str(obj_file))
        try:
            if BUILD_ALL or (src.stat().st_mtime > obj_file.stat().st_mtime):
                if (src.suffix == ".c"):            
                    command = f"{CC} -c {str(src)} -o {str(obj_file)} {FLAGS}"
                    print(f"{command}")
                    subprocess.run(command, shell=True)
                elif (src.suffix == ".cpp"):
                    command = f"{CXX} -c {str(src)} -o {str(obj_file)} {FLAGS}"
                    print(f"{command}")
                    subprocess.run(command, shell=True)
        except FileNotFoundError:
                if (src.suffix == ".c"):            
                    command = f"{CC} -c {str(src)} -o {str(obj_file)} {FLAGS}"
                    print(f"{command}")
                    subprocess.run(command, shell=True)
                elif (src.suffix == ".cpp"):
                    command = f"{CXX} -c {str(src)} -o {str(obj_file)} {FLAGS}"
                    print(f"{command}")
                    subprocess.run(command, shell=True)
def build():
    if (len(args) > 1):
        if (args[1] == "all"):
            global BUILD_ALL
            BUILD_ALL = True
        elif (args[1] == "test"):
            testpath  = Path("tests")
            testfiles = [f for f in testpath.rglob("*.cg")]
            for test in testfiles:
                basename = test.stem
                result   = testpath / Path("out") / (basename + ".test")
                valgrind = testpath / Path("out") / (basename + ".vlgrind")
                command  = f"{TARGET} {str(test)} --test > {str(result)} 2>&1"
                subprocess.run(command, shell=True)
                
                command  = f"valgrind --log-file={valgrind} {TARGET} {str(test)}"
                print(command)
                subprocess.run(command, shell=True)
        
    build_juve()
    build_obj_files()

    command = f"{CXX} {" ".join(OBJECTS)} -o {TARGET} {FLAGS} -I{str(INCDIR)} -I{str(SRCDIR)} -I. {FLAGS}"
    print(f"Building {TARGET}")
    subprocess.run(command, shell=True)
    
    pass

if __name__ == "__main__":
    build()

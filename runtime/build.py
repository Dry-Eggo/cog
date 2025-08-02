#! /usr/bin/python3

from pathlib import Path
import subprocess
from sys import argv as args

def run_command(command):
    print(command)
    subprocess.run(command, shell=True)

SRCFILES = [f for f in Path(".").rglob("*.c")]
TARGET   = Path("../bin/libcog.a")
BIN      = Path("bin")
OBJFILES = []
CC = "clang"

def build():
    for src in SRCFILES:
        basename = src.stem
        objfile = BIN / (basename + ".o")
        run_command(f"{CC} -c -o {str(objfile)} {str(src)}")
        OBJFILES.append(str(objfile))

    run_command(f"ar -rcs {str(TARGET)} {" ".join(OBJFILES)}")

if __name__ == "__main__":
    build()

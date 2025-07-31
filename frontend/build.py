from pathlib import Path
import subprocess

from sys import argv as args

TARGET = Path("../bin/cog")
OUTDIR = Path("../bin/frontend")
SRCDIR = Path("./source")
INCDIR = Path("./source")
SRCFILES = [f for f in SRCDIR.rglob("*.c")]
OBJFILES = []
FLAGS    = f"-I{INCDIR} -Wall -Wextra -pedantic -std=c23"

CC = "clang"

def build():
    for src in SRCFILES:
        basename = src.stem;
        objfile  = OUTDIR / (basename + ".o")
        OBJFILES.append(str(objfile))

        command = f"{CC} -c {str(src)} -o {objfile} {FLAGS}"
        print(f"Running: {command}")
        subprocess.run(command, shell=True)

    command = f"{CC} -o {TARGET} {" ".join(OBJFILES)}  {FLAGS}"
    print(f"Running: {command}")
    subprocess.run(command, shell=True)

    
if __name__ == "__main__":
    build()

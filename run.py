import os
from pathlib import Path

def COM(command, directory = None):
    if directory != None:
        os.chdir(directory)
    os.system(command)
    if directory != None:
        os.chdir(os.getcwd())

Path("build\\").mkdir(parents=True, exist_ok=True)
if os.name == 'nt':
    COM("cmake .. -G \"Visual Studio 16 2019\" -T \"Intel(R) oneAPI DPC++ Compiler\"", "build\\")

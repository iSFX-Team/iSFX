#!/usr/bin/env python3
import os
from sys import platform
if platform == 'darwin' or platform == 'linux':
  os.system("rm -rf ./build/lib.* &> /dev/null")
  os.system("rm -rf ./build/temp.* &> /dev/null")
  if os.system("python3 setup.py build") == 0:
    os.system("cp -f `find ./build | grep \.so` ./build &> /dev/null")
    os.system("rm -rf ./build/lib.* &> /dev/null")
    os.system("rm -rf ./build/temp.* &> /dev/null")
  else: print ("Build unsuccessful.")

elif platform[0:3] == 'win':
  os.system("del /q /f .\\build\\lib.win-amd64-3.2 >nul 2>nul")
  os.system("del /q /f .\\build\\temp.win-amd64-3.2 >nul 2>nul")
  os.system("COPY /B pyiSFX.cpp +,,")
  if os.system(".\\setup.py build") == 0:
    os.system("move /y .\\build\\lib.win-amd64-3.2\\iSFX.pyd .\\build\\ >nul 2>nul")
    os.system("del /q /f .\\build\\lib.win-amd64-3.2\\* >nul 2>nul")
    os.system("rmdir /q .\\build\\lib.win-amd64-3.2 >nul 2>nul")
    os.system("del /q /f .\\build\\temp.win-amd64-3.2\\Release\\* >nul 2>nul")
    os.system("rmdir /q .\\build\\temp.win-amd64-3.2\\Release >nul 2>nul")
    os.system("del /q /f .\\build\\temp.win-amd64-3.2\\* >nul 2>nul")
    os.system("rmdir /q .\\build\\temp.win-amd64-3.2 >nul 2>nul")
  else: print ("Build unsucessful.")

else:
  print("Build script cannot handle your platform.")
  print("\"",platform,"\"")
  print("Please add your platform to the build script.")

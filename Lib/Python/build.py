#!/usr/bin/env python3
import os
from sys import platform
if platform == 'darwin' or platform == 'linux':
  os.system("rm -rf ./build/lib.* &> /dev/null")
  os.system("rm -rf ./build/temp.* &> /dev/null")
  if os.system("python3 setup.py build") == 0:
    os.system("mv -f `find ./build | grep \.so` ./build &> /dev/null")
    os.system("rm -rf ./build/lib.* &> /dev/null")
    os.system("rm -rf ./build/temp.* &> /dev/null")
  else: print ("Build unsuccessful.")

if platform[0:3] == 'win':
  os.system("setup.py build")
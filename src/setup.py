from distutils.core import setup
import sys

import py2exe
print("Hello")
setup(console=[sys.path[0] + "/../ext/CuteR/CuteR/CuteR.py"])
#!/usr/bin/env python3

import os
import subprocess

SOURCE_DIR = "main/build/"

script_dir = os.path.dirname(os.path.abspath(__file__))

parent_dir = os.path.dirname(script_dir)
os.chdir(parent_dir)

if os.path.exists(SOURCE_DIR) and os.path.isdir(SOURCE_DIR):
    os.chdir(SOURCE_DIR)
else:
    subprocess.run(["./scripts/build.py"], check=True)
    os.chdir(SOURCE_DIR)

try:
    subprocess.run(["west", "flash"], check=True)
except subprocess.CalledProcessError as e:
    print(f"An error occurred while running 'west flash': {e}")
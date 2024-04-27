#!/usr/bin/env python3

import os
import shutil

folder_path = "/workdir/project/main/build"

if os.path.exists(folder_path) and os.path.isdir(folder_path):
    shutil.rmtree(folder_path)
    print(f"Deleted folder: {folder_path}")
else:
    print(f"Folder {folder_path} does not exist.")
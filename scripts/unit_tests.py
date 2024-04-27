#!/usr/bin/env python3

import subprocess
import os
import sys

TEST_SOURCE_DIR = "/workdir/project/tests"
TEST_OUTPUT_DIR = "/workdir/project/tests/build/"
BUILD_OUTPUT_DIR = "/workdir/project/main/build"


def build_project():
    os.chdir(TEST_SOURCE_DIR)
    if os.path.exists(TEST_SOURCE_DIR) and os.path.isdir(TEST_SOURCE_DIR):
        try:
            if not (
                os.path.exists(BUILD_OUTPUT_DIR) and os.path.isdir(BUILD_OUTPUT_DIR)
            ):
                os.mkdir(BUILD_OUTPUT_DIR)
            if not (os.path.exists(TEST_OUTPUT_DIR) and os.path.isdir(TEST_OUTPUT_DIR)):
                os.mkdir(TEST_OUTPUT_DIR)
            os.chdir(TEST_OUTPUT_DIR)
            subprocess.check_call(["cmake", TEST_SOURCE_DIR])
            subprocess.check_call(["cmake", "--build", "."])
        except subprocess.CalledProcessError:
            sys.exit(1)


def run_tests():
    try:
        subprocess.check_call(["ctest", "--test-dir", TEST_OUTPUT_DIR])
        print(
            "Tests generated at /workdir/project/smart_door_lock/build/tests/Testing/Temporary/LastTest.log"
        )
    except subprocess.CalledProcessError:
        print("Unit tests failed")
        sys.exit(1)


if __name__ == "__main__":
    build_project()
    run_tests()

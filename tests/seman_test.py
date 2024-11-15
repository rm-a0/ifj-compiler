#!/usr/bin/env python3
# Michal Repcik (xrepcim00)

import subprocess
import sys
import os

test_cases = {
    "ret3": [
        "seman_tests/test_ret3_valid_01.zig"
        "seman_tests/test_ret3_invalid_01.zig"
        "seman_tests/test_ret3_invalid_02.zig"
        "seman_tests/test_ret3_invalid_03.zig"
    ],
    "ret4": [
        "seman_tests/test_ret4_valid_01.zig"
    ],
}

def run_test(test_case):
    

def main():
    compiler = "../main" # path to compiler

    if len(sys.argv) < 2:
        run_all_tests()
    elif len(sys.argv) == 2:
        test_case = sys.argv[1]
        run_test(test_case)
    else:
        print("More then one argument is not supported");
        sys.exit(1)

if __name__ == "__main__":
    main()
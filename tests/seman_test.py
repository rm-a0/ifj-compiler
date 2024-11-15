#!/usr/bin/env python3
# Michal Repcik (xrepcim00)

import subprocess
import sys
import os

test_cases = {
    "ret3": [
        {"test_file": "seman_tests/test_ret3_valid_01.zig", "expected_return_code": 0},
        {"test_file": "seman_tests/test_ret3_invalid_01.zig", "expected_return_code": 3},
        {"test_file": "seman_tests/test_ret3_invalid_02.zig", "expected_return_code": 3},
        {"test_file": "seman_tests/test_ret3_invalid_03.zig", "expected_return_code": 3},
    ],
    "ret4": [
        {"test_file": "seman_tests/test_ret4_valid_01.zig", "expected_return_code": 0},
        {"test_file": "seman_tests/test_ret4_valid_02.zig", "expected_return_code": 0},
        {"test_file": "seman_tests/test_ret4_invalid_01.zig", "expected_return_code": 4},
        {"test_file": "seman_tests/test_ret4_invalid_02.zig", "expected_return_code": 4},
        {"test_file": "seman_tests/test_ret4_invalid_03.zig", "expected_return_code": 4},
        {"test_file": "seman_tests/test_ret4_invalid_04.zig", "expected_return_code": 4},
        {"test_file": "seman_tests/test_ret4_invalid_05.zig", "expected_return_code": 4},
    ],
    "ret5": [
        {"test_file": "seman_tests/test_ret5_valid_01.zig", "expected_return_code": 0},
        {"test_file": "seman_tests/test_ret5_invalid_01.zig", "expected_return_code": 5},
        {"test_file": "seman_tests/test_ret5_invalid_02.zig", "expected_return_code": 5},
        {"test_file": "seman_tests/test_ret5_invalid_03.zig", "expected_return_code": 5},
        {"test_file": "seman_tests/test_ret5_invalid_04.zig", "expected_return_code": 5},
    ],
    "ret6": [
        {"test_file": "seman_tests/test_ret6_valid_01.zig", "expected_return_code": 0},
        {"test_file": "seman_tests/test_ret6_valid_02.zig", "expected_return_code": 0},
        {"test_file": "seman_tests/test_ret6_invalid_01.zig", "expected_return_code": 6},
        {"test_file": "seman_tests/test_ret6_invalid_02.zig", "expected_return_code": 6},
        {"test_file": "seman_tests/test_ret6_invalid_03.zig", "expected_return_code": 6},
        {"test_file": "seman_tests/test_ret6_invalid_04.zig", "expected_return_code": 6},
    ],
}

# Runs specified test case
def run_test(test_case):
    print(f"Running test: {test_case}")
    compiler = "../main"

    test_files = test_cases.get(test_case, [])
    for test_info in test_files:
        test_file = test_info["test_file"]
        expected_return_code = test_info["expected_return_code"]
        print(f"Running test: {test_file}")

        if not os.path.exists(test_file):
            print(f"Test file {test_file} does not exist. Skipping.")
            continue
        try:
            result = subprocess.run(
                f"{compiler} < {test_file}",
                shell = True,
                stdout = subprocess.PIPE,
                stderr = subprocess.PIPE,
                text = True,
            )
        
            if result.returncode == expected_return_code:
                print(f"Test Passed: {test_file}")
            else:
                print(f"Test Failed: {test_file}")
                print(f"Expected return code: {expected_return_code}, but got: {result.returncode}")
                print("Compiler Errors:\n", result.stderr)
        except Exception as e:
            print(f"Error running test {test_file}: {e}")

# Runs all tests
def run_all_tests():
    for test_case in test_cases:
        run_test(test_case)

# Main function 
def main():
    if len(sys.argv) < 2:
        run_all_tests()
    elif len(sys.argv) == 2:
        test_case = sys.argv[1]
        if test_case not in test_cases:
            print("Invalid test name")
            print("Avaliable test names:", ", ".join(test_cases.keys()))
            sys.exit(1)
        run_test(test_case)
    else:
        print("More then one argument is not supported");
        sys.exit(1)

if __name__ == "__main__":
    main()
#!/bin/bash

# Associative array for test validity (Bash 4.0+ required)
declare -A validity=(
    [1]=0
    [2]=0
    [3]=0
    [4]=1
    [5]=0
    [6]=0
    [7]=0
    [8]=0
    [9]=0
    [10]=1
    [11]=1
    [12]=0
    [13]=0
    [14]=0
    [15]=1
    [16]=0
    [17]=0
    [18]=0
    [19]=0
    [20]=0
    [21]=1
    [22]=1
    [23]=0
    [24]=0
    [25]=0
    [26]=0
    [27]=1
    [28]=1
    [29]=1
    [30]=0
    [31]=0
    [32]=1
    [33]=0
    [34]=0
    [35]=0
    [36]=1
)

# Run make
echo "Running 'make'..."
if ! make; then
    echo "Make command failed. Exiting..."
    exit 1
fi

# Initialize counters
total_tests=36
passed_tests=0

# Run tests and check validity
echo "Running tests..."
for i in {1..36}; do
    # Run the test
    ./main < ./tests/seman_tests/$i.zig > test_output.tmp 2>&1
    if [[ $? -eq 0 && ${validity[$i]} -eq 1 ]]; then
        # Test passed and is valid
        echo -e "TEST $i \033[0;32mPASS\033[0m"
        passed_tests=$((passed_tests + 1))
    elif [[ $? -ne 0 && ${validity[$i]} -eq 0 ]]; then
        # Test failed and is invalid
        echo -e "TEST $i \033[0;32mPASS\033[0m"
        passed_tests=$((passed_tests + 1))
    else
        # Mismatch in validity
        echo -e "TEST $i \033[0;31mFAIL\033[0m"
    fi
done

# Cleanup temporary files
rm -f test_output.tmp

# Calculate and display pass ratio
echo "-----------------------------"
echo -e "Passed Tests: $passed_tests / $total_tests"
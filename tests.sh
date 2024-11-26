#!/bin/bash

declare -A validity=(
    [1]=3
    [2]=3
    [3]=3
    [4]=0
    [5]=4
    [6]=4
    [7]=4
    [8]=4
    [9]=4
    [10]=0
    [11]=0
    [12]=5
    [13]=5
    [14]=5
    [15]=0
    [16]=5
    [17]=6
    [18]=6
    [19]=6
    [20]=6
    [21]=0
    [22]=0
    [23]=7
    [24]=7
    [25]=7
    [26]=7
    [27]=0
    [28]=0
    [29]=0
    [30]=8
    [31]=8
    [32]=0
    [33]=9
    [34]=9
    [35]=9
    [36]=0
    [37]=7
    [38]=99
    [39]=99
    [40]=0
    [41]=10
    [42]=3
    [43]=5
    [44]=3
    [45]=10
    [46]=7
    [47]=3
    [48]=0
    [49]=0
    [50]=0
    [51]=7
    [52]=7
    [53]=0
    [54]=7
    [55]=0
    [56]=0
    [57]=0
    [58]=0
    [59]=7
    [60]=0
    [61]=7
    [62]=0
    [63]=0
    [64]=7
    [65]=0
    [66]=7
    [67]=0
    [68]=7
    [69]=0
    [70]=7
    [71]=7
    [72]=7
    [73]=7
    [74]=0
    [75]=7
)

# Run make
echo "Running 'make'..."
if ! make; then
    echo "Make command failed. Exiting..."
    exit 1
fi

# Initialize counters
total_tests=51
passed_tests=0

# Run tests and check validity
echo "Running tests..."
for i in {1..51}; do
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
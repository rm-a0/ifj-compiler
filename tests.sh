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
    [15]=5
    [16]=4
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
    [35]=0
    [36]=0
    [37]=7
    [38]=0
    [39]=0
    [40]=0
    [41]=10
    [42]=3
    [43]=0
    [44]=3
    [45]=10
    [46]=0
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
    [58]=2 //fix it
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
    [76]=0
    [77]=9
    [78]=5
    [79]=0
    [80]=0
    [81]=0
    [82]=0
    [83]=0
    [84]=0
    [85]=0
    [86]=9
    [87]=0
    [88]=0
    [89]=0
    [90]=0
    [91]=0
    [92]=0
    [93]=0
    [94]=0
    [95]=0
    [96]=1
    [97]=1
    [98]=0
    [99]=0
    [100]=0
    [101]=2
    [102]=2
    [103]=4
    [104]=0
    [105]=0
    [106]=0
    [107]=0
    [108]=0
    [109]=0
    [110]=0
    [111]=7
    [112]=7
    [113]=7
    [114]=6
    [115]=8
    [116]=7
    [117]=0
    [118]=5
    [119]=7
    [120]=5
    [121]=5
    [122]=0
    [123]=9
    [124]=9
    [125]=0
    [126]=6
    [127]=4
    [128]=6

)

# Run make
echo "Running 'make'..."
if ! make; then
    echo "Make command failed. Exiting..."
    exit 1
fi

# Initialize counters
total_tests=128
passed_tests=0

# Run tests and check exit codes
echo "Running tests..."
for i in {1..128}; do
    # Check if validity is defined for test $i
    if [[ -z "${validity[$i]+_}" ]]; then
        echo -e "TEST $i \033[0;33mSKIPPED\033[0m (Expected exit code not defined)"
        continue
    fi

    # Run the test
    ./main < ./tests/seman_tests/$i.zig > test_output.tmp 2>&1
    exit_code=$?
    expected_exit_code=${validity[$i]}

    if [[ $exit_code -eq $expected_exit_code ]]; then
        # Test passed
        echo -e "TEST $i \033[0;32mPASS\033[0m"
        passed_tests=$((passed_tests + 1))
    else
        # Test failed
        echo -e "TEST $i \033[0;31mFAIL\033[0m (Expected exit code $expected_exit_code, got $exit_code)"
    fi
done

# Cleanup temporary files
rm -f test_output.tmp

# Calculate and display pass ratio
echo "-----------------------------"
echo -e "Passed Tests: $passed_tests / $total_tests"
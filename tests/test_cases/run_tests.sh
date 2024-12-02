#!/bin/bash

TEST_DIR="test_cases"
CODE_GENERATOR="./code_generator"   # Replace with your code generator executable
INTERPRETER="ic24int"               # Interpreter provided for IFJcode24

TOTAL_TESTS=0
PASSED_TESTS=0

for TEST_FILE in $TEST_DIR/*.ifj24; do
    ((TOTAL_TESTS++))
    BASENAME=$(basename "$TEST_FILE" .ifj24)
    EXPECTED_IC="$TEST_DIR/$BASENAME.expected.ifjcode24"
    EXPECTED_OUTPUT="$TEST_DIR/$BASENAME.expected.output"
    INPUT_FILE="$TEST_DIR/$BASENAME.input"
    ACTUAL_IC="$TEST_DIR/$BASENAME.actual.ifjcode24"
    ACTUAL_OUTPUT="$TEST_DIR/$BASENAME.actual.output"

    echo "Running test: $BASENAME"

    # Run code generator
    $CODE_GENERATOR < "$TEST_FILE" > "$ACTUAL_IC" 2> "$TEST_DIR/$BASENAME.cg.log"
    CG_EXIT_CODE=$?

    if [ $CG_EXIT_CODE -ne 0 ]; then
        echo "Code generator failed with exit code $CG_EXIT_CODE"
        cat "$TEST_DIR/$BASENAME.cg.log"
        continue
    fi

    # Optional: Compare generated intermediate code with expected
    diff "$EXPECTED_IC" "$ACTUAL_IC" > /dev/null
    if [ $? -ne 0 ]; then
        echo "Intermediate code differs from expected for $BASENAME"
        echo "Differences:"
        diff -u "$EXPECTED_IC" "$ACTUAL_IC"
        continue
    fi

    # Run interpreter
    if [ -f "$INPUT_FILE" ]; then
        $INTERPRETER "$ACTUAL_IC" < "$INPUT_FILE" > "$ACTUAL_OUTPUT" 2> "$TEST_DIR/$BASENAME.int.log"
    else
        $INTERPRETER "$ACTUAL_IC" > "$ACTUAL_OUTPUT" 2> "$TEST_DIR/$BASENAME.int.log"
    fi
    INT_EXIT_CODE=$?

    if [ $INT_EXIT_CODE -ne 0 ]; then
        echo "Interpreter failed with exit code $INT_EXIT_CODE"
        cat "$TEST_DIR/$BASENAME.int.log"
        continue
    fi

    # Compare output
    diff "$EXPECTED_OUTPUT" "$ACTUAL_OUTPUT" > /dev/null
    if [ $? -eq 0 ]; then
        echo "Test $BASENAME passed."
        ((PASSED_TESTS++))
    else
        echo "Test $BASENAME failed."
        echo "Differences:"
        diff -u "$EXPECTED_OUTPUT" "$ACTUAL_OUTPUT"
    fi

    echo ""
done

echo "Total tests: $TOTAL_TESTS, Passed: $PASSED_TESTS, Failed: $((TOTAL_TESTS - PASSED_TESTS))"

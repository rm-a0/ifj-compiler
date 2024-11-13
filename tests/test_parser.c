/**
 * @file test_parser.c
 * @brief Test suite for parser.c
 * 
 * This test suite initializes a lexer with provided input strings,
 * invokes the parser, and checks for expected outcomes.
 * 
 * Ensure that all necessary source files are included in the Makefile.
 */

#define _POSIX_C_SOURCE 200809L // Enable POSIX functions like strdup

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../inc/parser.h"
#include "../inc/lexer.h"
#include "../inc/ast.h"
#include "../inc/error.h"

// Function prototypes
void run_test(const char* test_name, const char* input, ErrorType expected_error);
void print_test_result(const char* test_name, int passed);

// Helper function to create a FILE* from a string using fmemopen
FILE* string_to_file(const char* input) {
    // fmemopen is POSIX-specific. If unavailable, consider alternative methods.
    size_t len = strlen(input);
    return fmemopen((void*)input, len, "r");
}

// Main function to execute all tests
int main() {
    printf("Starting Parser Tests...\n\n");

    // Test Case 1: Valid Prolog
    run_test("Test 1: Valid Prolog",
             "const ifj = import(\"ifj24.zig\");",
             NO_ERROR);

    // Test Case 2: Invalid Prolog (missing semicolon)
    run_test("Test 2: Invalid Prolog (missing semicolon)",
             "const ifj = import(\"ifj24.zig\")",
             SYNTAX_ERROR);

    // Test Case 3: Valid Constant Declaration
    run_test("Test 3: Valid Constant Declaration",
             "const PI: f64 = 3.1415;",
             NO_ERROR);

    // Test Case 4: Invalid Constant Declaration (missing value)
    run_test("Test 4: Invalid Constant Declaration (missing value)",
             "const PI: f64 = ;",
             SYNTAX_ERROR);

    // Test Case 5: Valid Variable Declaration
    run_test("Test 5: Valid Variable Declaration",
             "var count: i32 = 10;",
             NO_ERROR);

    // Test Case 6: Invalid Variable Declaration (unknown type)
    run_test("Test 6: Invalid Variable Declaration (unknown type)",
             "var name: string = \"Test\";",
             SYNTAX_ERROR);

    // Test Case 7: Valid Function Declaration
    run_test("Test 7: Valid Function Declaration",
             "fn add(a: i32, b: i32) i32 { return a + b; }",
             NO_ERROR);

    // Test Case 8: Invalid Function Declaration (missing return type)
    run_test("Test 8: Invalid Function Declaration (missing return type)",
             "fn add(a: i32, b: i32) { return a + b; }",
             SYNTAX_ERROR);

    // Test Case 9: Valid If-Else Statement
    run_test("Test 9: Valid If-Else Statement",
             "if (x > 0) { var y = x; } else { var y = -x; }",
             NO_ERROR);

    // Test Case 10: Invalid If-Else Statement (missing brace)
    run_test("Test 10: Invalid If-Else Statement (missing brace)",
             "if (x > 0) var y = x; else { var y = -x; }",
             SYNTAX_ERROR);

    // Test Case 11: Valid While Loop
    run_test("Test 11: Valid While Loop",
             "while (i < 10) { i = i + 1; }",
             NO_ERROR);

    // Test Case 12: Invalid While Loop (incorrect condition)
    run_test("Test 12: Invalid While Loop (incorrect condition)",
             "while i < 10) { i = i + 1; }",
             SYNTAX_ERROR);

    // Test Case 13: Valid Function Call
    run_test("Test 13: Valid Function Call",
             "add(5, 3);",
             NO_ERROR);

    // Test Case 14: Invalid Function Call (missing parenthesis)
    run_test("Test 14: Invalid Function Call (missing parenthesis)",
             "add 5, 3;",
             SYNTAX_ERROR);

    // Test Case 15: Complex Expression with Operator Precedence
    run_test("Test 15: Complex Expression with Operator Precedence",
             "var result = a + b * c - (d / e);",
             NO_ERROR);

    // Test Case 16: Invalid Expression (unexpected token)
    run_test("Test 16: Invalid Expression (unexpected token)",
             "var result = a + * b;",
             SYNTAX_ERROR);

    // Test Case 17: Empty Program (should be allowed)
    run_test("Test 17: Empty Program",
             "",
             NO_ERROR);

    // Test Case 18: Program with Only Prolog
    run_test("Test 18: Program with Only Prolog",
             "const ifj = import(\"ifj24.zig\");",
             NO_ERROR);

    // Test Case 19: Program with Multiple Declarations and Functions
    run_test("Test 19: Program with Multiple Declarations and Functions",
             "const max = 100;\n"
             "var count: i32 = 0;\n"
             "fn increment(x: i32) i32 { return x + 1; }\n"
             "count = increment(count);",
             NO_ERROR);

    // Test Case 20: Mismatched Parentheses
    run_test("Test 20: Mismatched Parentheses",
             "fn test() i32 { return (1 + 2; }",
             SYNTAX_ERROR);

    printf("\nParser Tests Completed.\n");
    return 0;
}

/**
 * @brief Runs a single test case.
 * 
 * @param test_name Name of the test case.
 * @param input Input string to parse.
 * @param expected_error Expected error code (NO_ERROR for successful parse).
 */
void run_test(const char* test_name, const char* input, ErrorType expected_error) {
    printf("Running %s...\n", test_name);

    // Convert input string to FILE* using fmemopen
    FILE* input_fp = string_to_file(input);
    if (!input_fp) {
        printf("  [FAIL] Failed to create input FILE*.\n\n");
        return;
    }

    // Initialize Lexer
    Lexer lexer;
    if (init_lexer(&lexer, input_fp) != 0) {
        printf("  [FAIL] Lexer initialization failed.\n\n");
        fclose(input_fp);
        return;
    }

    // Close the FILE* as the lexer has its own copy or handles it
    fclose(input_fp);

    // Reset any previous errors
    reset_error();

    // Parse tokens and generate AST
    ASTNode* ast = parse_tokens(&lexer); // parse_tokens takes Lexer*

    ErrorType actual_error = get_error();

    // Verify the result
    int passed = 0;
    if (expected_error == NO_ERROR) {
        if (ast != NULL && actual_error == NO_ERROR) {
            passed = 1;
            // Optionally, print the AST for visual verification
            print_ast(ast, 0);
        }
    } else {
        if (ast == NULL && actual_error == expected_error) {
            passed = 1;
        }
    }

    print_test_result(test_name, passed);

    // Clean up
    if (ast != NULL) {
        free_ast_node(ast);
    }

    destroy_lexer(&lexer);
}

/**
 * @brief Prints the result of a test case.
 * 
 * @param test_name Name of the test case.
 * @param passed Whether the test passed (1) or failed (0).
 */
void print_test_result(const char* test_name, int passed) {
    if (passed) {
        printf("  [PASS] %s\n\n", test_name);
    } else {
        printf("  [FAIL] %s\n\n", test_name);
    }
}

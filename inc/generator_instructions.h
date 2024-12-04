/**
 * @file parser.h
 * @brief Header file for generator_instructions.c
 * @authors Martin Kandera(xkande00)
*/

#ifndef IFJ_COMPILER_GENERATOR_INSTRUCTIONS_H
#define IFJ_COMPILER_GENERATOR_INSTRUCTIONS_H
#include "ast.h"
#include "error.h"
#include "generator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Escapes special characters in a string (e.g., newline, tab) to ensure proper formatting in IFJcode24.
 * @param input Original string.
 * @return Escaped string.
 */
char* escape_string(const char* input);

/**
 * Determines the frame prefix (GF, LF, or TF) based on variable scoping.
 * @param var Name of the variable.
 * @return The frame prefix as a string.
 */
const char* frame_prefix(const char* var);

/**
 * Prints text with appropriate indentation for better readability in debugging.
 * @param text The text to be printed.
 */
void print_with_indent(const char* text);

/**
 * Prints a newline character in the generated code.
 */
void print_new_line();

/**
 * Generates a label in the code for control flow or function definition.
 * @param label_name Name of the label.
 */
void label(const char* label_name);

/**
 * Creates a temporary frame in IFJcode24.
 */
void gen_create_frame();

/**
 * Pushes the temporary frame onto the frame stack, making it the active frame.
 */
void gen_push_frame();

/**
 * Pops the active frame from the frame stack, reverting to the previous frame.
 */
void gen_pop_frame();

/**
 * Declares a variable in the current frame.
 * @param var Name of the variable to be declared.
 */
void def_var(const char* var);

/**
 * Generates a call to a user-defined or built-in function.
 * @param func Name of the function to call.
 */
void call(const char* func);

/**
 * Generates a return statement, restoring the previous frame and returning control to the caller.
 */
void return_f();

/**
 * Pushes a symbol (variable, constant, etc.) onto the data stack.
 * @param symb The symbol to be pushed.
 */
void pushs(const char* symb);

/**
 * Pops the top of the data stack into a specified variable.
 * @param var Name of the variable to store the popped value.
 */
void pops(const char* var);

/**
 * Generates an AND operation between two symbols and stores the result in a variable.
 * @param var The result variable.
 * @param symb1 The first operand.
 * @param symb2 The second operand.
 */
void and(const char* var, const char* symb1, const char* symb2);

/**
 * Generates an OR operation between two symbols and stores the result in a variable.
 * @param var The result variable.
 * @param symb1 The first operand.
 * @param symb2 The second operand.
 */
void or(const char* var, const char* symb1, const char* symb2);

/**
 * Generates a NOT operation on a symbol and stores the result in a variable.
 * @param var The result variable.
 * @param symb The operand.
 */
void not(const char* var, const char* symb);

/**
 * Concatenates two strings and stores the result in a variable.
 * @param var The result variable.
 * @param symb1 The first string.
 * @param symb2 The second string.
 */
void concat(const char* var, const char* symb1, const char* symb2);

/**
 * Determines the type of a symbol (e.g., int, float, string) and stores it in a variable.
 * @param var The result variable to store the type.
 * @param symb The symbol to determine the type of.
 */
void type(const char* var, const char* symb);


#endif //IFJ_COMPILER_GENERATOR_INSTRUCTIONS_H

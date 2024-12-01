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

char* escape_string(const char* input);
const char* frame_prefix(const char* var);
void print_with_indent(const char* text);
void print_new_line();
void label(const char* label_name);
void gen_create_frame();
void gen_push_frame();
void gen_pop_frame();
void def_var(const char* var);
void call(const char* func);
void return_f();
void pushs(const char* symb);
void pops(const char* var);

void and(const char* var, const char* symb1, const char* symb2);
void or(const char* var, const char* symb1, const char* symb2);
void not(const char* var, const char* symb);

void concat(const char* var, const char* symb1, const char* symb2);
void type(const char* var, const char* symb);


#endif //IFJ_COMPILER_GENERATOR_INSTRUCTIONS_H

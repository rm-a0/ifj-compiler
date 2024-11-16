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

void print_with_indent(const char* text);
void print_new_line();
void label(const char* label_name);
void create_frame();
void push_frame();
void pop_frame();
void def_var(const char* var, const char* frame_type);
void move(const char* var, const char* symb);
void call(const char* func);
void return_f();
void pushs(const char* symb);
void pops(const char* var);
void clears();
void add(const char* var, const char* symb1, const char* symb2);
void sub(const char* var, const char* symb1, const char* symb2);
void mul(const char* var, const char* symb1, const char* symb2);
void div_f(const char* var, const char* symb1, const char* symb2);
void idiv(const char* var, const char* symb1, const char* symb2);
void add_s(const char* var);
void sub_s(const char* var);
void mul_s(const char* var);
void div_s(const char* var);
void idiv_s(const char* var);
void lt(const char* var, const char* symb1, const char* symb2);
void gt(const char* var, const char* symb1, const char* symb2);
void eq(const char* var, const char* symb1, const char* symb2);
void lts(const char* var);
void gts(const char* var);
void eqs(const char* var);
void and(const char* var, const char* symb1, const char* symb2);
void or(const char* var, const char* symb1, const char* symb2);
void not(const char* var, const char* symb);
void ands(const char* var);
void ors(const char* var);
void nots(const char* var);
void int2float(const char* var, const char* symb);
void float2int(const char* var, const char* symb);
void int2char(const char* var, const char* symb);
void stri2int(const char* var, const char* symb1, const char* symb2);
void int2floats(const char* var, const char* symb);
void float2ints(const char* var, const char* symb);
void int2chars(const char* var, const char* symb);
void stri2ints(const char* var, const char* symb1, const char* symb2);
void read(const char* var, const char* type);
void write(const char* symb);
void concat(const char* var, const char* symb1, const char* symb2);
void str_len(const char* var, const char* symb);
void get_char(const char* var, const char* symb1, const char* symb2);
void set_char(const char* var, const char* symb1, const char* symb2);
void type(const char* var, const char* symb);
void jump(const char* label);
void jump_eq(const char* label, const char* symb1, const char* symb2);
void jump_neq(const char* label, const char* symb1, const char* symb2);
void jump_eqs(const char* label, const char* symb1, const char* symb2);
void jump_neqs(const char* label, const char* symb1, const char* symb2);
void exit_f();
void break_f();
void dprint(const char* symb);

#endif //IFJ_COMPILER_GENERATOR_INSTRUCTIONS_H

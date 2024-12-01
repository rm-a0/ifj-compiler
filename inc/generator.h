/**
 * @file parser.h
 * @brief Header file for generator.c
 * @authors Martin Kandera(xkande00)
*/

#ifndef IFJ_COMPILER_GENERATOR_H
#define IFJ_COMPILER_GENERATOR_H
#include "ast.h"
#include "error.h"
#include "generator_instructions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * @brief Generates code from the provided AST.
 * @param root Pointer to the root node of the AST.
 * @return 0 on success, non-zero on failure.
 */
int generate_code(ASTNode* root);
void generate_assignment(ASTNode* node);
void generate_expression(ASTNode* node);
void generate_if(ASTNode* node);
void generate_loop(ASTNode* node);

//while functions
void add_while_stack(int while_number);
void remove_while_stack();
int actual_while();
int while_stack_size();
int get_while_at_index(int index);


int get_tmp_counter();
void increment_tmp_counter();

typedef struct WhileStackNode { // Uzol zásobníka
    int while_number;
    struct WhileStackNode *next;
} WhileStackNode;


typedef struct WhileStack {     // Hlavná štruktúra zásobníka
    WhileStackNode *top;
} WhileStack;

extern WhileStack while_stack;
extern int tmp_counter;

//GF & LF functions
bool is_it_global(const char* var_name);
void add_to_global(const char* var_name);
void free_var_arrays();
bool is_it_local(const char* var_name);
void add_to_local(const char* var_name);

#endif //IFJ_COMPILER_GENERATOR_H

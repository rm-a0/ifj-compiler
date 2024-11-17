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

//GF functions
bool is_it_global(char* var_name);
void add_to_global(char* var_name);
void free_global_vars();

#endif //IFJ_COMPILER_GENERATOR_H

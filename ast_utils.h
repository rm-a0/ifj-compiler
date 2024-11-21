/**
 * @file ast_utils.h
 * @brief Header file for ast_utils.c
 * @authors Simon Bobko (xbobkos00)
*/

#ifndef AST_UTILS_H
#define AST_UTILS_H

#include <stdbool.h>
#include "ast.h"
#include "ast_node_stack.h"

void save_ast_to_file(ASTNode* ast, const char* filename);

const char* token_type_to_string(int token_type);

/**
 * @brief Recursively prints the details of an ASTNode.
 * @param node Pointer to the ASTNode to print.
 * @param indent_level Current indentation level for pretty printing.
 */
void print_ast_node(ASTNode* node, int indent_level);

/**
 * @brief Prints the entire ASTNodeStack from bottom to top.
 * @param stack Pointer to the ASTNodeStack.
 */
void print_ast_node_stack(ASTNodeStackPtr stack);

#endif // AST_UTILS_H

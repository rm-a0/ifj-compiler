/**
 * @file semantic_analysis.h
 * @brief Header file for implementation of semantic analysis
 * @authors Alex Marinica (xmarina00)
*/

#ifndef SEMANTIC_ANALYSIS_H
#define SEMANTIC_ANALYSIS_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <stdint.h>
#include "ast.h"
#include "symtable.h"
#include "stack.h"

/**
 * @brief Performs semantic analysis on the given abstract syntax tree (AST).
 *
 * Recursively traverses the AST and performs semantic checks, such as type compatibility,
 * symbol declarations, and scope resolution.
 *
 * @param node Pointer to the root AST node.
 * @param global_table Pointer to the global symbol table.
 * @param local_stack Pointer to the local scope stack.
 */
void semantic_analysis(ASTNode *node, SymbolTable *global_table, ScopeStack *local_stack);

/**
 * @brief Checks the validity of the 'main' function in the global symbol table.
 *
 * Ensures that a 'main' function is declared, has zero parameters, and is correctly defined.
 *
 * @param global_table Pointer to the global symbol table.
 */
void check_main_function(SymbolTable *global_table);

/**
 * @brief Evaluates the type of a binary operator expression.
 *
 * Ensures type compatibility between the operands and determines the resulting type.
 *
 * @param node Pointer to the AST node representing the binary operation.
 * @param global_table Pointer to the global symbol table.
 * @param local_stack Pointer to the local scope stack.
 * @return The resulting data type of the binary operation.
 */
DataType evaluate_binary_operator_type(ASTNode *node, SymbolTable *global_table, ScopeStack *local_stack, Frame *local_frame);

/**
 * @brief Evaluates the type of an expression node.
 *
 * Determines the data type of the given expression, checking symbol declarations
 * and ensuring type compatibility.
 *
 * @param node Pointer to the AST node representing the expression.
 * @param global_table Pointer to the global symbol table.
 * @param local_stack Pointer to the local scope stack.
 * @return The data type of the evaluated expression.
 */
DataType evaluate_expression_type(ASTNode *node, SymbolTable *global_table, ScopeStack *local_stack, Frame *local_frame);

#endif // SEMANTIC_ANALYSIS_H

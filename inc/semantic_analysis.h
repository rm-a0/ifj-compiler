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
#include <limits.h>
#include <math.h>
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
 * @param local_frame Pointer to the current local frame.
 * @return The resulting data type of the binary operation.
 */
DataType evaluate_operator_type(ASTNode *node, SymbolTable *global_table, ScopeStack *local_stack, Frame *local_frame);

/**
 * @brief Evaluates the type of an expression node.
 *
 * Determines the data type of the given expression, checking symbol declarations
 * and ensuring type compatibility.
 *
 * @param node Pointer to the AST node representing the expression.
 * @param global_table Pointer to the global symbol table.
 * @param local_stack Pointer to the local scope stack.
 * @param local_frame Pointer to the current local frame.
 * @return The data type of the evaluated expression.
 */
DataType evaluate_expression_type(ASTNode *node, SymbolTable *global_table, ScopeStack *local_stack, Frame *local_frame);

/**
 * @brief Processes a variable or constant declaration.
 *
 * Checks for redefinition, evaluates the assigned expression, and ensures type compatibility.
 *
 * @param global_table Pointer to the global symbol table.
 * @param local_stack Pointer to the local scope stack.
 * @param name Name of the variable or constant.
 * @param data_type_declared Declared data type of the variable or constant.
 * @param expression Pointer to the AST node for the initializing expression.
 * @param is_constant Indicates if the declaration is a constant.
 * @param is_nullable Indicates if the variable or constant can be null.
 * @param value Initial value of the variable or constant.
 */
void process_declaration(
    SymbolTable *global_table,
    ScopeStack *local_stack,
    const char *name,
    DataType data_type_declared,
    ASTNode *expression,
    bool is_constant,
    bool is_nullable,
    double value
);

/**
 * @brief Checks the compatibility of assigned types in an assignment or initialization.
 *
 * Ensures that the declared and assigned types match or are implicitly convertible
 * based on the provided rules.
 *
 * @param data_type_declared Declared data type of the variable.
 * @param data_type_stored Data type of the assigned value.
 * @param is_nullable Indicates if the variable can hold a null value.
 * @param value The numeric value being assigned.
 * @param expression_is_literal Indicates if the assigned value is a literal.
 */
void check_type_compatibility(
    DataType data_type_declared,
    DataType data_type_stored,
    bool is_nullable,
    double value,
    bool expression_is_literal
);

/**
 * @brief Evaluates the type of a function call expression.
 *
 * Checks the return type of the function being called and validates the arguments
 * against the expected parameter types.
 *
 * @param expression Pointer to the AST node representing the function call.
 * @param global_table Pointer to the global symbol table.
 * @param local_stack Pointer to the local scope stack.
 * @return The data type of the function call's return value.
 */
DataType evaluate_fn_call_type(ASTNode *expression, SymbolTable *global_table, ScopeStack *local_stack);

/**
 * @brief Checks the nullability of an identifier in the current scope.
 *
 * Determines if the specified identifier can hold null values based on its declaration.
 *
 * @param node Pointer to the AST node representing the identifier.
 * @param global_table Pointer to the global symbol table.
 * @param local_stack Pointer to the local scope stack.
 * @param local_frame Pointer to the current local frame.
 * @return True if the identifier is nullable, false otherwise.
 */
bool evaluate_nullable_operand(SymbolTable *global_table, ASTNode *node, ScopeStack *local_stack, Frame *local_frame);

/**
 * @brief Processes the initialization and binding of elements in control statements.
 *
 * Handles variable binding and ensures compatibility with the assigned expression type.
 *
 * @param expression Pointer to the AST node for the expression being bound.
 * @param global_table Pointer to the global symbol table.
 * @param local_stack Pointer to the local scope stack.
 * @param current_frame Pointer to the current local frame.
 * @param bind_name Name of the variable being bound.
 * @param condition_type Data type of the condition being evaluated.
 * @param has_literal Indicates if the expression has a literal value.
 */
void process_binding(
    ASTNode *expression,
    SymbolTable *global_table,
    ScopeStack *local_stack,
    Frame *current_frame,
    const char *bind_name,
    DataType condition_type,
    bool has_literal
);

/**
 * @brief Populates the global symbol table with functions declared in the program.
 *
 * Adds all function declarations found in the AST root to the global symbol table.
 *
 * @param root Pointer to the AST root node.
 * @param global_table Pointer to the global symbol table.
 */
void populate_global_table_with_functions(ASTNode *root, SymbolTable *global_table);

#endif // SEMANTIC_ANALYSIS_H

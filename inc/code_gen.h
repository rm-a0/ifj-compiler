#ifndef CODE_GEN_H
#define CODE_GEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "symtable.h"


void generate_local_constant(ASTNode* const_decl_node);
/**
 * @brief Generates code for the entire program.
 * 
 * This function initiates the code generation process from the root of the AST.
 * It traverses top-level declarations and generates code for functions and global variables.
 * 
 * @param[in] program_node The root AST node representing the program.
 * @param[in] symbol_table The symbol table containing symbol information.
 */
void generate_program(ASTNode* program_node, SymbolTable* symbol_table);

/**
 * @brief Generates code for a block of statements.
 * 
 * Traverses the statements within a block and generates code for each.
 * 
 * @param[in] block_node The AST node representing the block.
 */
void generate_block(ASTNode* block_node);

/**
 * @brief Generates code for a single statement.
 * 
 * Dispatches to the appropriate code generation function based on the statement type.
 * 
 * @param[in] stmt_node The AST node representing the statement.
 */
void generate_statement(ASTNode* stmt_node);

/**
 * @brief Generates code for an expression.
 * 
 * Recursively generates code for an expression node, handling literals, identifiers, and operations.
 * 
 * @param[in] expr_node The AST node representing the expression.
 */
void generate_expression(ASTNode* expr_node);

/**
 * @brief Generates code for a local variable declaration.
 * 
 * Declares a local variable in the appropriate frame and initializes it if an expression is provided.
 * 
 * @param[in] var_decl_node The AST node representing the variable declaration.
 */
void generate_local_variable(ASTNode* var_decl_node);

/**
 * @brief Generates code for an assignment statement.
 * 
 * Assigns the result of an expression to a variable.
 * 
 * @param[in] assignment_node The AST node representing the assignment.
 */
void generate_assignment(ASTNode* assignment_node);

/**
 * @brief Generates code for an if-else statement.
 * 
 * Handles the condition evaluation and code generation for both the true and false branches.
 * 
 * @param[in] if_else_node The AST node representing the if-else statement.
 */
void generate_if_else(ASTNode* if_else_node);

/**
 * @brief Generates code for a while loop.
 * 
 * Handles the loop condition and the loop body code generation.
 * 
 * @param[in] while_node The AST node representing the while loop.
 */
void generate_while_loop(ASTNode* while_node);

/**
 * @brief Generates code for a function call.
 * 
 * Prepares the arguments, calls the function, and handles the return value.
 * 
 * @param[in] fn_call_node The AST node representing the function call.
 */
void generate_function_call(ASTNode* fn_call_node);

/**
 * @brief Generates code for a return statement.
 * 
 * Evaluates the return expression if present and handles function exit.
 * 
 * @param[in] return_node The AST node representing the return statement.
 */
void generate_return(ASTNode* return_node);

/**
 * @brief Generates code for a binary operation.
 * 
 * Applies the appropriate operation based on the operator type.
 * 
 * @param[in] operator The operator type of the binary operation.
 */
void generate_binary_operation(OperatorType operator);

/**
 * @brief Escapes special characters in a string for code generation.
 * 
 * Converts special characters in a string to their escaped representations suitable for IFJcode24.
 * 
 * @param[in] str The original string to escape.
 * @return A newly allocated string with escaped characters (must be freed by the caller).
 */
char* escape_string(const char* str);

/**
 * @brief Generates code for a function declaration.
 * 
 * Handles code generation for a function, including parameters, local variables, and the function body.
 * 
 * @param[in] fn_decl_node The AST node representing the function declaration.
 */
void generate_function_declaration(ASTNode* fn_decl_node);

#endif // CODE_GEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "symtable.h"

void generate_return(ASTNode* return_node);
void generate_if_else(ASTNode* if_else_node);
void generate_while_loop(ASTNode* while_node);
void generate_return(ASTNode* return_node);
char* escape_string(const char* str);

// Function prototypes
void generate_program(ASTNode* program_node, SymbolTable* symbol_table);
void generate_block(ASTNode* block_node);
void generate_statement(ASTNode* stmt_node);
void generate_expression(ASTNode* expr_node);
void generate_local_variable(ASTNode* var_decl_node);
void generate_assignment(ASTNode* assignment_node);
void generate_if_else(ASTNode* if_else_node);
void generate_while_loop(ASTNode* while_node);
void generate_function_call(ASTNode* fn_call_node);
void generate_return(ASTNode* return_node);
void generate_binary_operation(OperatorType operator);
char* escape_string(const char* str);

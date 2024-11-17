// ast_node_stack.h

#ifndef AST_NODE_STACK_H
#define AST_NODE_STACK_H

#include <stdlib.h>
#include <stdbool.h>
#include "ast.h" // Include your AST node definitions

#define AST_NODE_STACK_INITIAL_CAPACITY 16

typedef struct ASTNodeStack {
    ASTNode **arr;     // Array to hold ASTNode pointers
    int top;           // Index of the top element
    int capacity;      // Current maximum capacity of the stack
} *ASTNodeStackPtr;

/**
 * @brief Prints the entire ASTNodeStack from bottom to top.
 * @param stack Pointer to the ASTNodeStack.
 */
void print_ast_node_stack(ASTNodeStackPtr stack);

// Function declarations
ASTNodeStackPtr init_ast_node_stack();
void resize_ast_node_stack(ASTNodeStackPtr stack);
bool is_empty_ast_node_stack(ASTNodeStackPtr stack);
bool is_full_ast_node_stack(ASTNodeStackPtr stack);
void push_ast_node(ASTNodeStackPtr stack, ASTNode* value);
ASTNode* pop_ast_node(ASTNodeStackPtr stack);
ASTNode* top_ast_node(ASTNodeStackPtr stack);
void free_ast_node_stack(ASTNodeStackPtr stack);

#endif // AST_NODE_STACK_H

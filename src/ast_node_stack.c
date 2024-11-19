/**
 * @file ast_node_stack.c
 * @brief Implementation of stack for ASTNodes
 * @authors Simon Bobko (xbobkos00)
*/
#include "ast_node_stack.h"
#include <stdio.h>

ASTNodeStackPtr init_ast_node_stack() {
    ASTNodeStackPtr stack = malloc(sizeof(struct ASTNodeStack));
    if (!stack) {
        // Handle memory allocation failure
        return NULL;
    }
    stack->capacity = AST_NODE_STACK_INITIAL_CAPACITY;
    stack->top = -1;
    stack->arr = malloc(stack->capacity * sizeof(ASTNode*));
    if (!stack->arr) {
        // Handle memory allocation failure
        free(stack);
        return NULL;
    }
    return stack;
}

void resize_ast_node_stack(ASTNodeStackPtr stack) {
    stack->capacity *= 2;
    ASTNode** new_arr = realloc(stack->arr, stack->capacity * sizeof(ASTNode*));
    if (!new_arr) {
        // Handle memory allocation failure
        // Possibly exit or set an error code
        // For now, we'll exit
        fprintf(stderr, "Failed to resize ASTNodeStack\n");
        exit(EXIT_FAILURE);
    }
    stack->arr = new_arr;
}

bool is_empty_ast_node_stack(ASTNodeStackPtr stack) {
    return stack->top == -1;
}

bool is_full_ast_node_stack(ASTNodeStackPtr stack) {
    return stack->top + 1 >= stack->capacity;
}

void push_ast_node(ASTNodeStackPtr stack, ASTNode* value) {
    if (is_full_ast_node_stack(stack)) {
        resize_ast_node_stack(stack);
    }
    stack->arr[++stack->top] = value;
}

ASTNode* pop_ast_node(ASTNodeStackPtr stack) {
    if (is_empty_ast_node_stack(stack)) {
        // Handle underflow
        fprintf(stderr, "ASTNodeStack underflow\n");
        return NULL;
    }
    return stack->arr[stack->top--];
}

ASTNode* top_ast_node(ASTNodeStackPtr stack) {
    if (is_empty_ast_node_stack(stack)) {
        // Handle empty stack
        return NULL;
    }
    return stack->arr[stack->top];
}

void free_ast_node_stack(ASTNodeStackPtr stack) {
    if (stack != NULL) {
        // Free any ASTNodes remaining in the stack
        while (!is_empty_ast_node_stack(stack)) {
            ASTNode* node = pop_ast_node(stack);
            if (node) {
                free_ast_node(node);  // Ensure this function correctly frees the AST node
            }
        }

        // Free the array of ASTNode* pointers
        if (stack->arr != NULL) {
            free(stack->arr);
            stack->arr = NULL; // Optional: Set to NULL to avoid dangling pointer
        }

        // Free the stack structure itself
        free(stack);
        stack = NULL; // Optional: Set to NULL to avoid dangling pointer
    }
}


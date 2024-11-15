#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "symtable.h"

/**
 * @brief Resizes the RootStack by doubling its capacity.
 * @param rootStack Pointer to the RootStack to resize.
 */
void resize_root_stack(RootStack *rootStack) {
    rootStack->capacity *= 2;
    rootStack->functionNames = realloc(rootStack->functionNames, rootStack->capacity * sizeof(char *));
    rootStack->functions = realloc(rootStack->functions, rootStack->capacity * sizeof(ScopeStack *));
}

/**
 * @brief Resizes the ScopeStack by doubling its capacity.
 * @param scopeStack Pointer to the ScopeStack to resize.
 */
void resize_scope_stack(ScopeStack *scopeStack) {
    scopeStack->capacity *= 2;
    scopeStack->frames = realloc(scopeStack->frames, scopeStack->capacity * sizeof(Frame *));
}

/**
 * @brief Initializes the RootStack for global function management.
 * @return Pointer to the initialized RootStack, or NULL if memory allocation fails.
 */
RootStack *init_root_stack() {
    RootStack *root = malloc(sizeof(RootStack));
    if (root) {
        root->functionNames = malloc(10 * sizeof(char *));
        root->functions = malloc(10 * sizeof(ScopeStack *));
        root->functionCount = 0;
        root->capacity = 10;
    }
    return root;
}

/**
 * @brief Adds a new function ScopeStack to the RootStack.
 * @param rootStack Pointer to the RootStack where the function will be added.
 * @param functionName Name of the function to add.
 */
void add_function(RootStack *rootStack, const char *functionName) {
    if (rootStack->functionCount >= rootStack->capacity) {
        resize_root_stack(rootStack); // Resize if capacity is reached
    }
    rootStack->functionNames[rootStack->functionCount] = strdup(functionName);
    rootStack->functions[rootStack->functionCount] = init_scope_stack();
    rootStack->functionCount++;
}

/**
 * @brief Retrieves the ScopeStack of a specific function by its name.
 * @param rootStack Pointer to the RootStack.
 * @param functionName Name of the function to retrieve.
 * @return Pointer to the ScopeStack of the specified function, or NULL if not found.
 */
ScopeStack *get_function_stack(RootStack *rootStack, const char *functionName) {
    for (int i = 0; i < rootStack->functionCount; i++) {
        if (strcmp(rootStack->functionNames[i], functionName) == 0) {
            return rootStack->functions[i];
        }
    }
    return NULL; // Not found
}

/**
 * @brief Initializes a new ScopeStack for managing frames within a function.
 * @return Pointer to the initialized ScopeStack, or NULL if memory allocation fails.
 */
ScopeStack *init_scope_stack() {
    ScopeStack *scopeStack = malloc(sizeof(ScopeStack));
    if (scopeStack) {
        scopeStack->frames = malloc(10 * sizeof(Frame *));
        scopeStack->top = -1;
        scopeStack->capacity = 10;
    }
    return scopeStack;
}

/**
 * @brief Pushes a new frame onto the ScopeStack, resizing if needed.
 * @param scopeStack Pointer to the ScopeStack where the frame will be pushed.
 */
void push_frame(ScopeStack *scopeStack, FrameType type) {
    if (scopeStack->top + 1 >= scopeStack->capacity) {
        resize_scope_stack(scopeStack); // Resize if capacity is reached
    }
    Frame *frame = init_frame();
    frame->type = type;  // Set frame type
    scopeStack->frames[++scopeStack->top] = frame;
}

/**
 * @brief Removes the top frame from the ScopeStack.
 * @param scopeStack Pointer to the ScopeStack to pop from.
 */
void pop_frame(ScopeStack *scopeStack) {
    if (scopeStack->top >= 0) {
        free(scopeStack->frames[scopeStack->top--]);
    }
}

/**
 * @brief Retrieves the top frame of the ScopeStack without removing it.
 * @param scopeStack Pointer to the ScopeStack.
 * @return Pointer to the top Frame, or NULL if the stack is empty.
 */
Frame *top_frame(ScopeStack *scopeStack) {
    if (scopeStack->top >= 0) {
        return scopeStack->frames[scopeStack->top];
    }
    return NULL;
}

/**
 * @brief Initializes a Frame with a new symbol table.
 * @return Pointer to the initialized Frame, or NULL if memory allocation fails.
 */
Frame *init_frame() {
    Frame *frame = malloc(sizeof(Frame));
    if (frame) {
        frame->symbolTable = init_symbol_table();
    }
    return frame;
}

#include <stdio.h>

/**
 * @brief Prints the contents of a function's ScopeStack for debugging.
 * 
 * @param scopeStack Pointer to the ScopeStack to print.
 */
void print_scope_stack(ScopeStack *scopeStack) {
    for (int i = 0; i <= scopeStack->top; i++) {
        Frame *frame = scopeStack->frames[i];
        printf("Frame %d: Type = ", i);
        if (frame->type == IF_COND_STATEMENT) {
            printf("IF_COND_STATEMENT\n");
        } else if (frame->type == WHILE_STATEMENT) {
            printf("WHILE_STATEMENT\n");
        }
        print_symbol_table(frame->symbolTable);
    }
}

void print_root_stack(RootStack *rootStack) {
    for (int i = 0; i < rootStack->functionCount; i++) {
        printf("Function: %s\n", rootStack->functionNames[i]);
        print_scope_stack(rootStack->functions[i]);
    }
}

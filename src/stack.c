#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "stack.h"
#include "symtable.h"

// Resize the RootStack by doubling its capacity
void resize_root_stack(RootStack *root_stack) {
    root_stack->capacity *= 2;
    root_stack->function_names = realloc(root_stack->function_names, root_stack->capacity * sizeof(char *));
    root_stack->functions = realloc(root_stack->functions, root_stack->capacity * sizeof(ScopeStack *));
}

// Resize the ScopeStack by doubling its capacity
void resize_scope_stack(ScopeStack *scope_stack) {
    scope_stack->capacity *= 2;
    scope_stack->frames = realloc(scope_stack->frames, scope_stack->capacity * sizeof(Frame *));
}

// Initialize the RootStack for global function management
RootStack *init_root_stack() {
    RootStack *root = malloc(sizeof(RootStack));
    if (root) {
        root->global_symbol_table = init_symbol_table(); // Initialize global symbol table
        root->function_names = malloc(10 * sizeof(char *));
        root->functions = malloc(10 * sizeof(ScopeStack *));
        root->function_count = 0;
        root->capacity = 10;
    }
    return root;
}

// Add a new function ScopeStack to the RootStack
void add_function(RootStack *root_stack, const char *function_name) {
    if (root_stack->function_count >= root_stack->capacity) {
        resize_root_stack(root_stack); // Resize if capacity is reached
    }
    root_stack->function_names[root_stack->function_count] = strdup(function_name);
    root_stack->functions[root_stack->function_count] = init_scope_stack();
    root_stack->function_count++;
}

// Retrieve the ScopeStack of a specific function by name
ScopeStack *get_function_stack(RootStack *root_stack, const char *function_name) {
    for (int i = 0; i < root_stack->function_count; i++) {
        if (strcmp(root_stack->function_names[i], function_name) == 0) {
            return root_stack->functions[i];
        }
    }
    return NULL; // Not found
}

// Initialize a new ScopeStack for managing frames within a function
ScopeStack *init_scope_stack() {
    ScopeStack *scope_stack = malloc(sizeof(ScopeStack));
    if (scope_stack) {
        scope_stack->frames = malloc(10 * sizeof(Frame *));
        scope_stack->top = -1;
        scope_stack->capacity = 10;
    }
    return scope_stack;
}

// Push a new frame onto the ScopeStack
void push_frame(ScopeStack *scope_stack, FrameType type) {
    if (scope_stack->top + 1 >= scope_stack->capacity) {
        resize_scope_stack(scope_stack); // Resize if capacity is reached
    }
    Frame *frame = init_frame();
    frame->type = type;  // Set frame type
    scope_stack->frames[++scope_stack->top] = frame;
}

// Pop the top frame from the ScopeStack
void pop_frame(ScopeStack *scope_stack) {
    if (scope_stack->top >= 0) {
        free(scope_stack->frames[scope_stack->top--]);
    }
}

// Retrieve the top frame of the ScopeStack
Frame *top_frame(ScopeStack *scope_stack) {
    if (scope_stack->top >= 0) {
        return scope_stack->frames[scope_stack->top];
    }
    return NULL;
}

// Initialize a Frame with a new symbol table
Frame *init_frame() {
    Frame *frame = malloc(sizeof(Frame));
    if (frame) {
        frame->symbol_table = init_symbol_table();
    }
    return frame;
}

// Add a global variable to the RootStack's global symbol table
void add_global_variable(RootStack *root_stack, const char *name, int value) {
    add_symbol(root_stack->global_symbol_table, name, value);
}

// Lookup a global variable in the RootStack's global symbol table
Symbol *lookup_global_variable(RootStack *root_stack, const char *name) {
    return lookup_symbol(root_stack->global_symbol_table, name);
}

// Print the contents of the ScopeStack for debugging
void print_scope_stack(ScopeStack *scope_stack) {
    for (int i = 0; i <= scope_stack->top; i++) {
        Frame *frame = scope_stack->frames[i];
        printf("Frame %d: Type = ", i);
        if (frame->type == IF_COND_STATEMENT) {
            printf("IF_COND_STATEMENT\n");
        } else if (frame->type == WHILE_STATEMENT) {
            printf("WHILE_STATEMENT\n");
        }
        print_symbol_table(frame->symbol_table);
    }
}

// Print the contents of the RootStack for debugging
void print_root_stack(RootStack *root_stack) {
    printf("Global Variables:\n");
    print_symbol_table(root_stack->global_symbol_table);

    for (int i = 0; i < root_stack->function_count; i++) {
        printf("\nFunction: %s\n", root_stack->function_names[i]);
        print_scope_stack(root_stack->functions[i]);
    }
}

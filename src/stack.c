/**
 * @file stack.h
 * @brief Source files implementing stack data structure
 * @authors Alex Marinica (xmarina00)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "symtable.h"
#include "error.h"

#define INITIAL_STACK_CAPACITY 10

/**
 * @brief Resizes the ScopeStack by doubling its capacity.
 *
 * @param scopeStack Pointer to the ScopeStack to resize.
 */
void resize_scope_stack(ScopeStack *scopeStack) {
    scopeStack->capacity *= 2;
    scopeStack->frames = realloc(scopeStack->frames, scopeStack->capacity * sizeof(Frame *));
}

/**
 * @brief Initializes a new ScopeStack for managing frames within a function.
 *
 * @return Pointer to the initialized ScopeStack, or NULL if memory allocation fails.
 */
ScopeStack *init_scope_stack() {
    ScopeStack *stack = malloc(sizeof(ScopeStack));
    if (!stack) {
        fprintf(stderr, "Error: Failed to allocate memory for scope stack.\n");
        exit(INTERNAL_ERROR);
    }
    stack->top = -1; // Initialize stack as empty
    stack->capacity = INITIAL_STACK_CAPACITY; // Ensure capacity is set
    stack->frames = malloc(sizeof(Frame *) * stack->capacity);
    if (!stack->frames) {
        fprintf(stderr, "Error: Failed to allocate memory for stack frames.\n");
        exit(INTERNAL_ERROR);
    }
    return stack;
}

/**
 * @brief Looks up a symbol in the local stack, global table, or provided frame.
 *
 * This function searches for a symbol starting from the specified local frame,
 * falling back to the local stack, and then the global table if not found.
 *
 * @param local_stack Pointer to the local scope stack.
 * @param name Name of the symbol to look for.
 * @param local_frame Pointer to a specific local frame to search first, or NULL to skip.
 * @return Pointer to the found symbol or NULL if not found.
 */
Symbol *lookup_symbol_in_scope(ScopeStack *local_stack, const char *name, Frame *local_frame) {
    printf("Looking up symbol: %s\n", name);

    // 1. Check if local_frame is explicitly provided
    if (local_frame) {
        printf("Checking local frame:\n");
        print_symbol_table(local_frame->symbol_table);

        Symbol *symbol = lookup_symbol(local_frame->symbol_table, name);
        if (symbol) {
            printf("Found symbol '%s' in the provided local frame.\n", name);
            return symbol;
        }
    }

    // 2. If local_stack exists, search from top to bottom
    if (local_stack) {
        printf("Searching local stack frames:\n");
        for (int i = local_stack->top; i >= 0; i--) {
            Frame *frame = local_stack->frames[i];
            printf("Checking frame %d:\n", i);
            print_symbol_table(frame->symbol_table);

            Symbol *symbol = lookup_symbol(frame->symbol_table, name);
            if (symbol) {
                printf("Found symbol '%s' in frame %d.\n", name, i);
                return symbol;
            }
        }
    }

    // 3. Symbol not found
    printf("Symbol '%s' not found in any scope.\n", name);
    return NULL;
}

/**
 * @brief Pushes a new frame onto the ScopeStack, resizing if needed.
 *
 * Allocates memory for a new frame and its symbol table, pushing it onto the stack.
 *
 * @param stack Pointer to the ScopeStack where the frame will be pushed.
 */
void push_frame(ScopeStack *stack) {
    if (!stack) {
        fprintf(stderr, "Error: Null stack passed to push_frame.\n");
        exit(INTERNAL_ERROR);
    }
    if (stack->top + 1 >= stack->capacity) {
        stack->capacity *= 2;
        stack->frames = realloc(stack->frames, sizeof(Frame *) * stack->capacity);
        if (!stack->frames) {
            fprintf(stderr, "Error: Failed to reallocate memory for stack frames.\n");
            exit(INTERNAL_ERROR);
        }
    }
    stack->top++;
    printf("stack->top: %d\n", stack->top);
    stack->frames[stack->top] = malloc(sizeof(Frame));
    if (!stack->frames[stack->top]) {
        fprintf(stderr, "Error: Failed to allocate memory for frame.\n");
        exit(INTERNAL_ERROR);
    }
    stack->frames[stack->top]->symbol_table = init_symbol_table();
}

/**
 * @brief Removes the top frame from the ScopeStack.
 *
 * Frees the memory associated with the top frame and removes it from the stack.
 *
 * @param scope_stack Pointer to the ScopeStack to pop from.
 */
void pop_frame(ScopeStack *scope_stack) {
    if (scope_stack->top >= 0) {
        free(scope_stack->frames[scope_stack->top--]);
    }
}

/**
 * @brief Retrieves the top frame of the ScopeStack without removing it.
 *
 * @param scope_stack Pointer to the ScopeStack.
 * @return Pointer to the top Frame, or NULL if the stack is empty.
 */
Frame *top_frame(ScopeStack *scope_stack) {
    if (scope_stack->top >= 0) {
        return scope_stack->frames[scope_stack->top];
    }
    return NULL;
}

/**
 * @brief Frees the memory associated with a single frame.
 *
 * Cleans up the symbol table and the frame structure itself.
 *
 * @param frame Pointer to the Frame to be freed.
 */
void free_frame(Frame *frame) {
    if (!frame) return;

    // Free the symbol table within the frame
    if (frame->symbol_table) {
        free_symbol_table(frame->symbol_table);
    }

    // Free the frame itself
    free(frame);
}

/**
 * @brief Frees the memory associated with a ScopeStack.
 *
 * Cleans up all frames within the stack and then the stack structure itself.
 *
 * @param stack Pointer to the ScopeStack to be freed.
 */
void free_scope_stack(ScopeStack *stack) {
    if (!stack) return;

    // Free each frame in the stack
    for (int i = 0; i <= stack->top; i++) {
        free_frame(stack->frames[i]);
    }

    // Free the frames array
    free(stack->frames);

    // Free the stack itself
    free(stack);
}

/**
 * @brief Initializes a Frame with a new symbol table.
 *
 * Allocates memory for the frame and its associated symbol table.
 *
 * @return Pointer to the initialized Frame, or NULL if memory allocation fails.
 */
Frame *init_frame() {
    Frame *frame = malloc(sizeof(Frame));
    if (frame) {
        frame->symbol_table = init_symbol_table();
    }
    return frame;
}

/**
 * @brief Prints the contents of a function's ScopeStack for debugging.
 *
 * Iterates through all frames in the stack and prints their symbol tables.
 *
 * @param scope_stack Pointer to the ScopeStack to print.
 */
void print_scope_stack(ScopeStack *scope_stack) {
    for (int i = 0; i <= scope_stack->top; i++) {
        Frame *frame = scope_stack->frames[i];
        printf("Frame %d\n", i);
        print_symbol_table(frame->symbol_table);
    }
}

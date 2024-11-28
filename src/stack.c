/**
 * @file stack.h
 * @brief Source files implementating stack data structure
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
 * @param scopeStack Pointer to the ScopeStack to resize.
 */
void resize_scope_stack(ScopeStack *scopeStack) {
    scopeStack->capacity *= 2;
    scopeStack->frames = realloc(scopeStack->frames, scopeStack->capacity * sizeof(Frame *));
}

/**
 * @brief Initializes a new ScopeStack for managing frames within a function.
 * @return Pointer to the initialized ScopeStack, or NULL if memory allocation fails.
 */
ScopeStack *init_scope_stack() {
    ScopeStack *stack = malloc(sizeof(ScopeStack));
    if (!stack) {
        exit(INTERNAL_ERROR);
    }
    stack->top = -1; // Initialize stack as empty
    stack->capacity = INITIAL_STACK_CAPACITY; // Ensure capacity is set
    stack->frames = malloc(sizeof(Frame *) * stack->capacity);
    if (!stack->frames) {
        exit(INTERNAL_ERROR);
    }
    return stack;
}


/**
 * @brief Looks up a symbol in the local stack and global table.
 * 
 * This function searches for a symbol starting from the top frame of the local stack 
 * and then falls back to the global table if not found in the local scopes.
 * 
 * @param global_table Pointer to the global symbol table.
 * @param local_stack Pointer to the local scope stack.
 * @param name Name of the symbol to look for.
 * @return Pointer to the found symbol or NULL if not found.
 */
Symbol *lookup_symbol_in_scopes(SymbolTable *global_table, ScopeStack *local_stack, const char *name, Frame *local_frame) {

    // 1. Check if local_frame is explicitly provided
    if (local_frame) {

        Symbol *symbol = lookup_symbol(local_frame->symbol_table, name);
        if (symbol) {
            return symbol;
        }
    }

    // 2. If local_stack exists, search from top to bottom
    if (local_stack) {
        for (int i = local_stack->top; i >= 0; i--) {
            Frame *frame = local_stack->frames[i];

            Symbol *symbol = lookup_symbol(frame->symbol_table, name);
            if (symbol) {
                return symbol;
            }
        }
    }

    // 3. Fall back to global table if symbol not found in local frames
    if (global_table) {

        Symbol *symbol = lookup_symbol(global_table, name);
        if (symbol) {
            return symbol;
        }
    }

    // 4. Symbol not found
    return NULL;
}


/**
 * @brief Pushes a new frame onto the ScopeStack, resizing if needed.
 * @param scope_stack Pointer to the ScopeStack where the frame will be pushed.
 */
void push_frame(ScopeStack *stack) {
    if (!stack) {
        exit(INTERNAL_ERROR);
    }
    if (stack->top + 1 >= stack->capacity) {
        stack->capacity *= 2;
        stack->frames = realloc(stack->frames, sizeof(Frame *) * stack->capacity);
        if (!stack->frames) {
            exit(INTERNAL_ERROR);
        }
    }
    stack->top++;
    stack->frames[stack->top] = malloc(sizeof(Frame));
    if (!stack->frames[stack->top]) {
        exit(INTERNAL_ERROR);
    }
    stack->frames[stack->top]->symbol_table = init_symbol_table();
}


/**
 * @brief Removes the top frame from the ScopeStack.
 * @param scope_stack Pointer to the ScopeStack to pop from.
 */
void pop_frame(ScopeStack *scope_stack) {
    if (scope_stack->top >= 0) {
        free(scope_stack->frames[scope_stack->top--]);
    }
}

/**
 * @brief Retrieves the top frame of the ScopeStack without removing it.
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
 * @return Pointer to the initialized Frame, or NULL if memory allocation fails.
 */
Frame *init_frame() {
    Frame *frame = malloc(sizeof(Frame));
    if (frame) {
        frame->symbol_table = init_symbol_table();
    }
    return frame;
}


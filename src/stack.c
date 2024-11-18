#include <stdlib.h>
#include <string.h>
#include "../inc/stack.h"
#include "../inc/symtable.h"

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
    ScopeStack *scope_stack = malloc(sizeof(ScopeStack));
    if (scope_stack) {
        scope_stack->frames = malloc(10 * sizeof(Frame *));
        scope_stack->top = -1;
        scope_stack->capacity = 10;
    }
    return scope_stack;
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
Symbol *lookup_symbol_in_scopes(SymbolTable *global_table, ScopeStack *local_stack, const char *name) {
    // Check local stack first
    if (local_stack != NULL) {
        for (int i = local_stack->top; i >= 0; i--) {
            Frame *frame = local_stack->frames[i];
            Symbol *symbol = lookup_symbol(frame->symbol_table, name);
            if (symbol != NULL) {
                return symbol; // Found in local scope
            }
        }
    }

    // Fall back to global table
    if (global_table != NULL) {
        Symbol *symbol = lookup_symbol(global_table, name);
        if (symbol != NULL) {
            return symbol; // Found in global scope
        }
    }

    // Symbol not found
    return NULL;
}

/**
 * @brief Pushes a new frame onto the ScopeStack, resizing if needed.
 * @param scope_stack Pointer to the ScopeStack where the frame will be pushed.
 */
void push_frame(ScopeStack *scope_stack) {
    if (scope_stack->top + 1 >= scope_stack->capacity) {
        resize_scope_stack(scope_stack); // Resize if capacity is reached
    }
    Frame *frame = init_frame();
    scope_stack->frames[++scope_stack->top] = frame;
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

#include <stdio.h>

/**
 * @brief Prints the contents of a function's ScopeStack for debugging.
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

/**
 * @brief Prints the contents of the root_stack for debugging.
 * 
 * @param root_stack Pointer to the root_stack to print.
 */
// void print_root_stack(RootStack *root_stack) {
//     for (int i = 0; i < root_stack->declerations_count; i++) {
//         printf("Decl: %s\n", root_stack->function_names[i]);
//         print_scope_stack(root_stack->declerations[i]);
//     }
// }


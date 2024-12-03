/**
 * @file stack.h
 * @brief Header file for implementation of stack data structure
 * @authors Alex Marinica (xmarina00)
*/

#ifndef STACK_H
#define STACK_H

#include "symtable.h"
#include <stdbool.h>

struct SymbolTable;
typedef struct SymbolTable SymbolTable;

struct Symbol;
typedef struct Symbol Symbol;

/**
 * @struct Frame
 * @brief Struct representing a frame that holds a symbol table.
 *
 * A frame corresponds to a specific scope in the program (e.g., function, loop, or conditional block).
 */
typedef struct Frame {
    SymbolTable *symbol_table;
} Frame;


/**
 * @struct ScopeStack
 * @brief Represents a stack of frames within a function or local scope.
 *
 * The stack is used to manage nested scopes (e.g., function bodies, if/while blocks).
 */
typedef struct ScopeStack {
    Frame **frames;      /**< Dynamic array of pointers to `Frame` elements. */
    int top;             /**< Index of the top frame on the stack. */
    int capacity;        /**< Current maximum capacity of the stack. */
} ScopeStack;

/**
 * @brief Initializes a new ScopeStack for managing frames within a function or scope.
 *
 * Allocates memory for the stack and initializes its capacity.
 * 
 * @return Pointer to the newly initialized `ScopeStack`, or `NULL` if memory allocation fails.
 */
ScopeStack *init_scope_stack();

/**
 * @brief Looks up a symbol in the local stack and global table.
 * 
 * This function searches for a symbol starting from the top frame of the local stack 
 * and then falls back to the global table if not found in the local scopes.
 * 
 * @param local_stack Pointer to the local scope stack.
 * @param name Name of the symbol to look for.
 * @param local_frame Pointer to a specific local frame to search first, or NULL to skip.
 * @return Pointer to the found symbol or NULL if not found.
 */
Symbol *lookup_symbol_in_scope(ScopeStack *local_stack, const char *name, Frame *local_frame);

/**
 * @brief Pushes a new frame onto the ScopeStack, resizing if needed.
 *
 * Creates a new frame with an empty symbol table and adds it to the top of the stack.
 *
 * @param scope_stack Pointer to the `ScopeStack` to push a new frame onto.
 */
void push_frame(ScopeStack *scope_stack);

/**
 * @brief Removes the top frame from the ScopeStack.
 *
 * Frees memory allocated for the top frame and its associated symbol table.
 *
 * @param scope_stack Pointer to the `ScopeStack` to pop from.
 */
void pop_frame(ScopeStack *scope_stack);

/**
 * @brief Frees the memory associated with a single frame.
 * 
 * @param frame Pointer to the Frame to be freed.
 */
void free_frame(Frame *frame);

/**
 * @brief Frees the memory associated with a ScopeStack.
 * 
 * @param stack Pointer to the ScopeStack to be freed.
 */
void free_scope_stack(ScopeStack *stack);

/**
 * @brief Retrieves the top frame of the ScopeStack without removing it.
 *
 * Returns the frame at the top of the stack, or `NULL` if the stack is empty.
 *
 * @param scope_stack Pointer to the `ScopeStack`.
 * @return Pointer to the top `Frame`, or `NULL` if the stack is empty.
 */
Frame *top_frame(ScopeStack *scope_stack);

/**
 * @brief Resizes the ScopeStack when the capacity is reached.
 *
 * Doubles the current capacity of the stack and reallocates memory for frames.
 *
 * @param scope_stack Pointer to the `ScopeStack` to resize.
 */
void resize_scope_stack(ScopeStack *scope_stack);

/**
 * @brief Initializes a new frame with an empty symbol table.
 *
 * Allocates memory for the frame and initializes its symbol table.
 *
 * @return Pointer to the initialized `Frame`, or `NULL` if memory allocation fails.
 */
Frame *init_frame();

/**
 * @brief Prints the contents of a ScopeStack for debugging.
 *
 * Displays information about each frame in the stack, including its symbol table.
 *
 * @param scope_stack Pointer to the `ScopeStack` to print.
 */
void print_scope_stack(ScopeStack *scope_stack);

#endif // STACK_H

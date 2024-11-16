#ifndef STACK_H
#define STACK_H

#include "symtable.h"

typedef enum {
    IF_COND_STATEMENT,
    WHILE_STATEMENT
} FrameType;

/**
 * @struct Frame
 * @brief Struct representing a frame that holds a symbol table.
 */
typedef struct Frame {
    SymbolTable *symbol_table; /**< Each frame has its own symbol table */
    FrameType type;            /**< Type of the frame (IF_COND_STATEMENT, WHILE_STATEMENT) */
} Frame;

/**
 * @struct ScopeStack
 * @brief Struct representing a stack of frames within a function scope.
 */
typedef struct ScopeStack {
    Frame **frames;      /**< Array to hold pointers to Frame elements */
    int top;             /**< Index of the top element */
    int capacity;        /**< Current maximum capacity of the stack */
} ScopeStack;

/**
 * @struct RootStack
 * @brief Struct representing a global stack of functions, each with its own ScopeStack.
 */
typedef struct RootStack {
    SymbolTable *global_symbol_table; /**< Global symbol table for global variables and constants */
    char **function_names;            /**< Names of functions */
    ScopeStack **functions;           /**< Each function has its own ScopeStack */
    int function_count;               /**< Current number of functions in the stack */
    int capacity;                     /**< Maximum capacity of the RootStack */
} RootStack;

// Function prototypes for stack and scope management

RootStack *init_root_stack();
void add_function(RootStack *root_stack, const char *function_name);
ScopeStack *get_function_stack(RootStack *root_stack, const char *function_name);
void resize_root_stack(RootStack *root_stack);

ScopeStack *init_scope_stack();
void push_frame(ScopeStack *scope_stack, FrameType type);
void pop_frame(ScopeStack *scope_stack);
Frame *top_frame(ScopeStack *scope_stack);
void resize_scope_stack(ScopeStack *scope_stack);

Frame *init_frame();

// Global symbol table operations
void add_global_variable(RootStack *root_stack, const char *name, int value);
Symbol *lookup_global_variable(RootStack *root_stack, const char *name);

// Debugging functions
void print_root_stack(RootStack *root_stack);

#endif // STACK_H

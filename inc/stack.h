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
    SymbolTable *symbolTable; // Each frame has its own symbol table
    FrameType type;           // Type of the frame (IF_COND_STATEMENT, WHILE_STATEMENT)
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
    char **functionNames;   /**< Names of functions */
    ScopeStack **functions; /**< Each function has its own ScopeStack */
    int functionCount;      /**< Current number of functions in the stack */
    int capacity;           /**< Maximum capacity of the RootStack */
} RootStack;

// Function prototypes for stack and scope management

/**
 * @brief Initializes the root stack for global function management.
 * @return Pointer to the initialized RootStack.
 */
RootStack *init_root_stack();

/**
 * @brief Adds a new function ScopeStack to the RootStack.
 * @param rootStack Pointer to the RootStack.
 * @param functionName Name of the function to add.
 */
void add_function(RootStack *rootStack, const char *functionName);

/**
 * @brief Retrieves the ScopeStack of a specific function by name.
 * @param rootStack Pointer to the RootStack.
 * @param functionName Name of the function to retrieve.
 * @return Pointer to the corresponding ScopeStack, or NULL if not found.
 */
ScopeStack *get_function_stack(RootStack *rootStack, const char *functionName);

/**
 * @brief Resizes the RootStack when the capacity is reached.
 * @param rootStack Pointer to the RootStack to resize.
 */
void resize_root_stack(RootStack *rootStack);

/**
 * @brief Initializes a ScopeStack for managing frames within a function.
 * @return Pointer to the initialized ScopeStack.
 */
ScopeStack *init_scope_stack();

/**
 * @brief Pushes a new frame onto a ScopeStack, resizing if needed.
 * @param scopeStack Pointer to the ScopeStack to push a new frame onto.
 */
void push_frame(ScopeStack *scopeStack, FrameType type);

/**
 * @brief Removes the top frame from a ScopeStack.
 * @param scopeStack Pointer to the ScopeStack to pop from.
 */
void pop_frame(ScopeStack *scopeStack);

/**
 * @brief Retrieves the top frame of a ScopeStack without removing it.
 * @param scopeStack Pointer to the ScopeStack.
 * @return Pointer to the top Frame, or NULL if the stack is empty.
 */
Frame *top_frame(ScopeStack *scopeStack);

/**
 * @brief Resizes the ScopeStack when the capacity is reached.
 * @param scopeStack Pointer to the ScopeStack to resize.
 */
void resize_scope_stack(ScopeStack *scopeStack);

/**
 * @brief Initializes a new frame with an empty symbol table.
 * @return Pointer to the initialized Frame.
 */
Frame *init_frame();

// Add the print function here
void print_root_stack(RootStack *rootStack);

#endif // STACK_H

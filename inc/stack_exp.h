/**
 * @file stack.h
 * @brief Header file for stack_exp.c
 * @authors Alex Marinica (xmarina00)
*/

#include <stdbool.h>

/**
 * @struct Stack
 * @brief Struct representing a stack data structure.
 */
typedef struct Stack {
    int *arr;       // Array to hold stack elements
    int top;        // Index of the top element
    int capacity;   // Current maximum capacity of the stack
} *StackPtr;

/**
 * @fn StackPtr init_stack()
 * @brief Initializes a new stack with a predefined initial capacity.
 *
 * @return Pointer to the initialized stack structure.
 * @retval NULL if memory allocation fails.
 */
StackPtr init_stack();

/**
 * @fn void resize(StackPtr stack)
 * @brief Resizes the stack to double its current capacity.
 *
 * @param[in, out] stack Pointer to the stack structure to resize.
 * 
 * @note Exits with an error code if memory allocation fails or the maximum stack capacity is exceeded.
 */
void resize(StackPtr stack);

/**
 * @fn bool is_empty(StackPtr stack)
 * @brief Checks if the stack is empty.
 *
 * @param[in] topIndex variable storing head index of a stack.
 * @return True if the stack is empty, false otherwise.
 */
bool is_empty(int topIndex);

/**
 * @fn bool is_full(StackPtr stack)
 * @brief Checks if the stack is full.
 *
 * @param[in] stack Pointer to the stack structure.
 * @return True if the stack is full, false otherwise.
 */
bool is_full(StackPtr stack);

/**
 * @fn void push(StackPtr stack, int value)
 * @brief Pushes a new value onto the stack, resizing if necessary.
 *
 * @param[in, out] stack Pointer to the stack structure.
 * @param[in] value The integer value to be pushed onto the stack.
 */
void push(StackPtr stack, int value);

/**
 * @fn void pop(StackPtr stack)
 * @brief Removes the top element from the stack.
 *
 * @param[in, out] stack Pointer to the stack structure.
 */
void pop(StackPtr stack);

/**
 * @fn int top(StackPtr stack)
 * @brief Retrieves the top element of the stack without removing it.
 *
 * @param[in] stack Pointer to the stack structure.
 * @return The top element of the stack, or an error code if the stack is empty.
 */
int top(StackPtr stack);

/**
 * @fn int free_resources(StackPtr stack)
 * @brief Frees the memory allocated for the stack and its elements.
 *
 * @param[in, out] stack Pointer to the stack structure.
 * @return 0 on successful resource release.
 */
int free_resources(StackPtr stack);
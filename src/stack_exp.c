/**
 * @file stack_exp.c
 * @brief Implementation of stack data structure
 * @authors Alex Marinica (xmarina00)
*/

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "stack_exp.h"
#include "error.h"

#define INITIAL_CAPACITY 16

StackPtr init_stack() {
    // Allocate memory for the stack structure
    StackPtr stack = (StackPtr)malloc(sizeof(struct Stack));
    
    // Check if memory allocation for the stack failed
    if (stack == NULL) {
        fprintf(stderr, "Failed to allocate memory for stack\n");
        exit(INTERNAL_ERROR);
    }
    
    // Allocate memory for the stack's internal array with initial capacity
    stack->arr = (int*)malloc(INITIAL_CAPACITY * sizeof(int));

    // Check if memory allocation for the stack array failed
    if (stack->arr == NULL) {
        free(stack); // Free the stack structure before exiting
        fprintf(stderr, "Failed to allocate memory for stack array\n");
        exit(INTERNAL_ERROR);
    }

    // Initialize stack properties
    stack->top = -1;                 // Stack is initially empty
    stack->capacity = INITIAL_CAPACITY; // Set initial capacity
    return stack;
}

void resize_stack(StackPtr stack) {
    if (stack != NULL) {
        
        // Check for overflow: avoid doubling capacity if it would exceed INT_MAX
        if (stack->capacity > (INT_MAX / 2)) {
            fprintf(stderr, "Maximum stack capacity reached\n");
            exit(INTERNAL_ERROR);
        }

        // Double the current capacity
        stack->capacity *= 2;

        // Attempt to reallocate memory for the array with the new capacity
        int *newArr = (int*)realloc(stack->arr, stack->capacity * sizeof(int));
        
        // Check if reallocation failed
        if (newArr == NULL) {
            fprintf(stderr, "Memory re-allocation failed\n");
            exit(INTERNAL_ERROR);
        }

        // Update the array pointer to the newly allocated memory
        stack->arr = newArr;
    }
}

bool is_empty(int topIndex) {
    // Returns true if topIndex is -1, indicating no elements in the stack
    return (topIndex == -1 ? true : false);
}

bool is_full(StackPtr stack) {
    // Returns true if top index is one less than the capacity
    return (stack->top + 1 == stack->capacity ? true : false);
}

void push(StackPtr stack, int value) {
    if (stack != NULL) {
        // If stack is full, resize to double its capacity
        if (is_full(stack)) {
            resize_stack(stack);
        }

        // Increment top index and add the value to the new top position
        stack->arr[++stack->top] = value;
    }
}

void pop(StackPtr stack) {
    // Only decrement top index if stack is not empty
    if (stack != NULL && stack->top != -1) {
        stack->top -= 1;
    }
}

int top(StackPtr stack) {
    // Check if stack is empty; return INTERNAL_ERROR if so, else return top element
    return (is_empty(stack->top) ? INTERNAL_ERROR : stack->arr[stack->top]);
}

int free_resources(StackPtr stack) {
    // Free the array holding the stack elements
    free(stack->arr);
    
    // Free the stack structure itself
    free(stack);
    return 0;
}
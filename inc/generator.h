/**
 * @file parser.h
 * @brief Header file for generator.c
 * @authors Martin Kandera (xkande00)
*/

#ifndef IFJ_COMPILER_GENERATOR_H
#define IFJ_COMPILER_GENERATOR_H
#include "ast.h"
#include "error.h"
#include "generator_instructions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * @brief Generates code from the provided AST.
 * @param root Pointer to the root node of the AST.
 * @return 0 on success, non-zero on failure.
 */
int generate_code(ASTNode* root);
/**
 * @brief Generates code for assignment statements.
 * @param node The AST node representing the assignment.
 */
void generate_assignment(ASTNode* node);

/**
 * @brief Generates code for evaluating expressions.
 * @param node The AST node representing the expression.
 */
void generate_expression(ASTNode* node);

/**
 * @brief Generates code for if-else statements.
 * @param node The AST node representing the conditional structure.
 */
void generate_if(ASTNode* node);

/**
 * @brief Generates code for loop structures (e.g., while loops).
 * @param node The AST node representing the loop.
 */
void generate_loop(ASTNode* node);


/**
 * @brief Adds a new while loop identifier to the stack.
 * @param while_number The unique identifier for the while loop.
 */
void add_while_stack(int while_number);

/**
 * @brief Removes the topmost while loop identifier from the stack.
 */
void remove_while_stack();

/**
 * @brief Retrieves the identifier of the current (topmost) while loop.
 * @return The identifier of the current while loop.
 */
int actual_while();

/**
 * @brief Gets the number of while loops currently in the stack.
 * @return The size of the while loop stack.
 */
int while_stack_size();

/**
 * @brief Retrieves the identifier of the while loop at a specific index in the stack.
 * @param index The index of the desired while loop (0 is the bottom of the stack).
 * @return The identifier of the while loop at the specified index.
 */
int get_while_at_index(int index);


/**
 * @brief Retrieves the current temporary variable counter.
 * @return The current value of the temporary variable counter.
 */
int get_tmp_counter();

/**
 * @brief Increments the temporary variable counter by 1.
 */
void increment_tmp_counter();

/* Stack structures for while loops */

/**
 * @brief Structure representing a node in the while loop stack.
 */
typedef struct WhileStackNode {
    int while_number;                  ///< Unique identifier for the while loop.
    struct WhileStackNode *next;       ///< Pointer to the next node in the stack.
} WhileStackNode;

/**
 * @brief Structure representing the stack of while loops.
 */
typedef struct WhileStack {
    WhileStackNode *top;               ///< Pointer to the top of the stack.
} WhileStack;

/**
 * @brief Global stack for managing nested while loops.
 */
extern WhileStack while_stack;

/**
 * @brief Counter for generating unique temporary variable names.
 */
extern int tmp_counter;


/**
 * @brief Checks if a variable is declared in the local frame.
 * @param var_name The name of the variable.
 * @return \c true if the variable is in the local frame, \c false otherwise.
 */
bool is_it_local(const char* var_name);

/**
 * @brief Adds a variable to the local frame.
 * @param var_name The name of the variable to add.
 */
void add_to_local(const char* var_name);

#endif //IFJ_COMPILER_GENERATOR_H

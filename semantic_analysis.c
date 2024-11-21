/**
 * @file semantic_analysis.c
 * @brief File implementing semantic analysis through recursive descent
 * @authors Alex Marinica (xmarina00)
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include "parser.h"
#include "ast.h"
#include "stack.h"
#include "error.h"
#include "semantic_analysis.h"

typedef struct {
    const char *name;         // Name of the built-in function
    int param_count;          // Number of parameters (-1 for variable arguments)
    DataType expected_arg_types[3]; // Array of expected argument types (up to 3 for simplicity)
    DataType return_type;     // Return type of the function
} BuiltInFunction;

BuiltInFunction built_in_functions[] = {
    {"ifj.write", -1, {AST_UNSPECIFIED, AST_UNSPECIFIED, AST_UNSPECIFIED}, AST_VOID},
    {"ifj.readstr", 0, {AST_UNSPECIFIED, AST_UNSPECIFIED, AST_UNSPECIFIED}, AST_SLICE},
    {"ifj.readi32", 0, {AST_UNSPECIFIED, AST_UNSPECIFIED, AST_UNSPECIFIED}, AST_I32},
    {"ifj.readf64", 0, {AST_UNSPECIFIED, AST_UNSPECIFIED, AST_UNSPECIFIED}, AST_F64},
    {"ifj.i2f", 1, {AST_I32, AST_UNSPECIFIED, AST_UNSPECIFIED}, AST_F64},
    {"ifj.f2i", 1, {AST_F64, AST_UNSPECIFIED, AST_UNSPECIFIED}, AST_I32},
    {"ifj.length", 1, {AST_SLICE, AST_UNSPECIFIED, AST_UNSPECIFIED}, AST_I32},
    {"ifj.concat", 2, {AST_SLICE, AST_SLICE, AST_UNSPECIFIED}, AST_SLICE},
    {"ifj.substring", 3, {AST_SLICE, AST_I32, AST_I32}, AST_SLICE},
    {"ifj.strcmp", 2, {AST_SLICE, AST_SLICE, AST_UNSPECIFIED}, AST_I32},
    {"ifj.ord", 2, {AST_SLICE, AST_I32, AST_UNSPECIFIED}, AST_I32},
    {"ifj.chr", 1, {AST_I32, AST_UNSPECIFIED, AST_UNSPECIFIED}, AST_SLICE},
    {"ifj.string", 1, {AST_SLICE, AST_UNSPECIFIED, AST_UNSPECIFIED}, AST_SLICE}
};

void print_global_symbol_table(SymbolTable *global_table) {
    if (!global_table) {
        printf("Global symbol table is NULL.\n");
        return;
    }

    printf("Global Symbol Table:\n");
    printf("===============================================================================\n");
    printf("Index | Type     | Name            | Data Type/Return Type | Used | Scope Stack\n");
    printf("-------------------------------------------------------------------------------\n");

    for (int i = 0; i < global_table->capacity; i++) {
        Symbol *symbol = global_table->symbols[i];
        if (!symbol) {
            continue;
        }

        printf("%5d | ", i);

        if (symbol->type == SYMBOL_FUNC) {
            printf("Function | %-15s | %-20d | %4s | %p\n",
                   symbol->func.name,
                   symbol->func.type,
                   symbol->func.used ? "Yes" : "No",
                   (void *)symbol->func.scope_stack);
        } else if (symbol->type == SYMBOL_VAR) {
            printf("Variable | %-15s | %-20d | %4s | (N/A)\n",
                   symbol->var.name,
                   symbol->var.type,
                   symbol->var.used ? "Yes" : "No");
        }
    }

    printf("===============================================================================\n");
}


void check_main_function(SymbolTable *global_table) {
    // Lookup the main() function in the global symbol table
    Symbol *main_symbol = lookup_symbol(global_table, "main");

    // Check if the main function exists
    if (main_symbol == NULL || main_symbol->type != SYMBOL_FUNC) {
        fprintf(stderr, "Semantic Error: Missing 'main' function declaration.\n");
        exit(SEMANTIC_ERROR_UNDEFINED);
    }

    // Check if the main function has zero parameters
    ScopeStack *main_scope_stack = main_symbol->func.scope_stack;

    if (main_scope_stack == NULL || main_scope_stack->frames[0]->symbol_table->count > 0) {
        fprintf(stderr, "Semantic Error: 'main' function must have zero parameters.\n");
        exit(SEMANTIC_ERROR_PARAMS);
    }
}

DataType evaluate_binary_operator_type(ASTNode *node, SymbolTable *global_table, ScopeStack *local_stack) {
    // Ensure the node is of type AST_BIN_OP
    if (node->type != AST_BIN_OP) {
        fprintf(stderr, "Internal Error: Node is not a binary operator.\n");
        exit(INTERNAL_ERROR);
    }

    // Evaluate the left and right operand types
    DataType left_type = evaluate_expression_type(node->BinaryOperator.left, global_table, local_stack);
    DataType right_type = evaluate_expression_type(node->BinaryOperator.right, global_table, local_stack);

    // Determine the operator type
    OperatorType operator = node->BinaryOperator.operator;

    // Check type compatibility based on the operator
    switch (operator) {
        case AST_PLUS:
        case AST_MINUS:
        case AST_MUL:
        case AST_DIV:
            // Arithmetic operators
            if ((left_type == AST_I32 || left_type == AST_F64) &&
                (right_type == AST_I32 || right_type == AST_F64)) {
                // Promote to the higher precision type
                return (left_type == AST_F64 || right_type == AST_F64) ? AST_F64 : AST_I32;
            } else {
                fprintf(stderr, "Semantic Error: Incompatible types for arithmetic operation. Left: %d, Right: %d\n", left_type, right_type);
                exit(SEMANTIC_ERROR_TYPE_COMPAT);
            }

        case AST_GREATER:
        case AST_GREATER_EQU:
        case AST_LESS:
        case AST_LESS_EQU:
        case AST_EQU:
        case AST_NOT_EQU:
            // Relational operators
            if ((left_type == AST_I32 || left_type == AST_F64) &&
                (right_type == AST_I32 || right_type == AST_F64)) {
                // Relational operators always return a boolean result
                return AST_I32; // Representing boolean as an integer type
            } else {
                fprintf(stderr, "Semantic Error: Incompatible types for relational operation. Left: %d, Right: %d\n", left_type, right_type);
                exit(SEMANTIC_ERROR_TYPE_COMPAT);
            }

        default:
            fprintf(stderr, "Semantic Error: Unsupported operator type '%d'.\n", operator);
            exit(SEMANTIC_ERROR_TYPE_COMPAT);
    }
}

DataType evaluate_expression_type(ASTNode *node, SymbolTable *global_table, ScopeStack *local_stack) {

    printf("eval\n");
    printf("node->type: %u\n", node->type);

    switch (node->type) {
        case AST_INT:
            return AST_I32;
        case AST_FLOAT:
            return AST_F64;
        case AST_STRING:
            return AST_SLICE;
        case AST_IDENTIFIER: {
            printf("got_here\n");
            Symbol *symbol = lookup_symbol_in_scopes(global_table, local_stack, node->Identifier.identifier);
            if (!symbol) {
                fprintf(stderr, "Semantic Error: Undeclared variable '%s'.\n", node->Identifier.identifier);
                exit(SEMANTIC_ERROR_UNDEFINED);
            }

            // when var used in fun. mark it as used 
            if (symbol->type == SYMBOL_VAR) {
                symbol->var.used = true;
            }
            return symbol->var.type;
        }
        case AST_BIN_OP:
            return evaluate_binary_operator_type(node, global_table, local_stack);
        case AST_ARG:
            if (!node->Argument.expression) {
                fprintf(stderr, "Semantic Error: Argument expression is missing.\n");
                exit(SEMANTIC_ERROR_UNDEFINED);
            }
            // Recurs. eval. expression
            return evaluate_expression_type(node->Argument.expression, global_table, local_stack);
        default:
            fprintf(stderr, "Semantic Error: Unsupported expression type.\n");
            exit(SEMANTIC_ERROR_RETURN);
    }
}



void semantic_analysis(ASTNode *node, SymbolTable *global_table, ScopeStack *local_stack) {
    printf("node: %u\n", node->type);
    print_global_symbol_table(global_table);

    if (!node) return;
    switch (node->type) {
        case AST_PROGRAM: {

            for (int i = 0; i < node->Program.decl_count; i++) {
                // "node->Program.declarations[i]" can take a shape of fn, var or const
                semantic_analysis(node->Program.declarations[i], global_table, local_stack);
            }

            // Check for the presence of a valid 'main' function after processing all declarations
            check_main_function(global_table);

            break;
        }

        case AST_FN_DECL: {
            const char *fn_name = node->FnDecl.fn_name;
            DataType return_type = node->FnDecl.return_type;

            // Check if the function is already declared
            Symbol *existing_symbol = lookup_symbol(global_table, fn_name);
            if (existing_symbol != NULL) {
                fprintf(stderr, "Semantic Error: Function '%s' is already declared.\n", fn_name);
                exit(SEMANTIC_ERROR_REDEF);
            }

            // Add the function symbol to the global symbol table
            add_function_symbol(global_table, fn_name, return_type);

            // Retrieve the function symbol to access its scope stack
            Symbol *fn_symbol = lookup_symbol(global_table, fn_name);
            if (fn_symbol == NULL || fn_symbol->type != SYMBOL_FUNC) {
                fprintf(stderr, "Internal Error: Failed to retrieve function '%s' symbol.\n", fn_name);
                exit(SEMANTIC_ERROR_UNDEFINED);
            }

            // Initialize the scope stack for the function if not already initialized
            if (!fn_symbol->func.scope_stack) {
                fn_symbol->func.scope_stack = init_scope_stack();
            }
            ScopeStack *function_stack = fn_symbol->func.scope_stack;

            // Push a frame for the function parameters
            push_frame(function_stack);

            // Process function parameters
            for (int i = 0; i < node->FnDecl.param_count; i++) {
                printf("param_i: %d\n", i);
                ASTNode *param_node = node->FnDecl.params[i];

                // Ensure the parameter node is valid
                if (param_node->type != AST_PARAM) {
                    fprintf(stderr, "Semantic Error: Invalid parameter in function '%s'.\n", fn_name);
                    exit(SEMANTIC_ERROR_PARAMS);
                }

                const char *param_name = param_node->Param.identifier;
                DataType param_type = param_node->Param.data_type;

                printf("param_type: %u\n", param_type);

                // Check for duplicate parameters
                Symbol *existing_param = lookup_symbol(function_stack->frames[function_stack->top]->symbol_table, param_name);
                if (existing_param != NULL) {
                    fprintf(stderr, "Semantic Error: Duplicate parameter name '%s' in function '%s'.\n", param_name, fn_name);
                    exit(SEMANTIC_ERROR_PARAMS);
                }

                // Add the parameter to the local scope
                add_variable_symbol(function_stack->frames[function_stack->top]->symbol_table, param_name, param_type);
            }

            printf("done with params\n");
            // Push a new frame for the function body
            push_frame(function_stack);

            // Recursively analyze the function block
            if (node->FnDecl.block) {
                semantic_analysis(node->FnDecl.block, global_table, function_stack);
            }

            // Ensure non-void functions have a return statement
            if (return_type != AST_VOID) {
                bool return_found = false;
                for (int i = 0; i < node->FnDecl.block->Block.node_count; i++) {
                    ASTNode *child_node = node->FnDecl.block->Block.nodes[i];
                    if (child_node->type == AST_RETURN) {
                        return_found = true;
                        break;
                    }
                }

                if (!return_found) {
                    fprintf(stderr, "Semantic Error: Function '%s' must have a return statement.\n", fn_name);
                    exit(SEMANTIC_ERROR_RETURN);
                }
            }

            // Cleanup the function's scope stack (pop all frames)
            // while (function_stack->top >= 0) {
            pop_frame(function_stack);
            // }
            break;
        }
    

        case AST_PARAM: {
            // Retrieve parameter details
            const char *param_name = node->Param.identifier;
            DataType param_type = node->Param.data_type;
            // bool nullable = node->Param.nullable;
            
            // Check if the parameter already exists in the current scope
            if (lookup_symbol(local_stack->frames[local_stack->top]->symbol_table, param_name)) {
                fprintf(stderr, "Semantic Error: Duplicate parameter '%s' in the function scope.\n", param_name);
                exit(SEMANTIC_ERROR_PARAMS);
            }

            // Add the parameter to the local symbol table
            add_variable_symbol(local_stack->frames[local_stack->top]->symbol_table, param_name, param_type);

            // If the parameter is nullable, ensure it is handled properly
            // TODO: Resolve nullability of elements
            // if (nullable) {
            //     Symbol *param_symbol = lookup_symbol(local_stack->frames[local_stack->top]->symbol_table, param_name);
            //     if (param_symbol) {
            //         param_symbol->var.nullable = true;
            //     }
            // }
            
            break;
        }


        case AST_BLOCK: {
            // Push a new frame to the local stack for the block
            push_frame(local_stack);

            // Traverse all nodes within the block
            for (int i = 0; i < node->Block.node_count; i++) {
                ASTNode *child_node = node->Block.nodes[i];

                // Perform semantic analysis on each child node
                semantic_analysis(child_node, global_table, local_stack);

                // Check if the child node is a function call and its return value is discarded
                if (child_node->type == AST_FN_CALL) {
                    Symbol *fn_symbol = lookup_symbol(global_table, child_node->FnCall.fn_name);
                    if (fn_symbol && fn_symbol->type == SYMBOL_FUNC && fn_symbol->func.type != AST_VOID) {
                        fprintf(stderr, "Semantic Error: Function '%s' has a non-void return type, but its value is discarded.\n",
                                child_node->FnCall.fn_name);
                        exit(SEMANTIC_ERROR_TYPE_COMPAT);
                    }
                }
            }
            
            // Check for unused variables in the current frame
            Frame *current_frame = top_frame(local_stack);
            if (current_frame && current_frame->symbol_table) {
                SymbolTable *current_table = current_frame->symbol_table;

                for (int j = 0; j < current_table->capacity; j++) {
                    Symbol *symbol = current_table->symbols[j];
                    if (symbol && symbol->type == SYMBOL_VAR && !symbol->var.used) {
                        fprintf(stderr, "Semantic Error: Variable '%s' declared in the block was never used.\n", symbol->var.name);
                        exit(SEMANTIC_ERROR_UNUSED_VAR);
                    }
                }
            }

            // Pop the frame after exiting the block
            pop_frame(local_stack);
            break;
        }


        case AST_WHILE: {
            // Evaluate the condition expression of the while loop
            DataType condition_type = evaluate_expression_type(node->WhileCycle.expression, global_table, local_stack);

            // Ensure the condition is a boolean-compatible type
            if (condition_type != AST_I32 && condition_type != AST_U8) {
                fprintf(stderr, "Semantic Error: While condition must evaluate to a boolean-compatible type, got '%d'.\n", condition_type);
                exit(SEMANTIC_ERROR_TYPE_COMPAT);
            }

            // Push a new frame onto the local stack for the while loop
            push_frame(local_stack);

            // If element_bind is present, bind it to the new frame's symbol table
            if (node->WhileCycle.element_bind) {
                SymbolTable *current_table = top_frame(local_stack)->symbol_table;

                // Add the bind element to the current frame
                add_variable_symbol(current_table, node->WhileCycle.element_bind, AST_UNSPECIFIED);
            }

            // Recursively analyze the block inside the while loop
            if (node->WhileCycle.block) {
                semantic_analysis(node->WhileCycle.block, global_table, local_stack);
            }

            // Check for unused variables in the frame
            Frame *current_frame = top_frame(local_stack);
            if (current_frame && current_frame->symbol_table) {
                SymbolTable *current_table = current_frame->symbol_table;
                for (int i = 0; i < current_table->capacity; i++) {
                    Symbol *symbol = current_table->symbols[i];
                    if (symbol && symbol->type == SYMBOL_VAR && !symbol->var.used) {
                        fprintf(stderr, "Semantic Error: Variable '%s' declared in the while loop was never used.\n", symbol->var.name);
                        exit(SEMANTIC_ERROR_UNUSED_VAR);
                    }
                }
            }

            // Pop the frame after processing the while loop
            pop_frame(local_stack);
            break;
        }

        case AST_IF_ELSE: {
            printf("Analyzing AST_IF_ELSE...\n");
            // Analyze the condition expression
            if (node->IfElse.expression) {
                DataType condition_type = evaluate_expression_type(node->IfElse.expression, global_table, local_stack);
                if (condition_type != AST_I32) { // True False (1, 0)
                    fprintf(stderr, "Semantic Error: If-Else condition must evaluate to int.\n");
                    exit(SEMANTIC_ERROR_TYPE_COMPAT);
                }
            }

            // Push a frame for the "if" block
            push_frame(local_stack);

            // Handle element_bind in the "if" block, if provided
            if (node->IfElse.element_bind) {
                add_variable_symbol(
                    top_frame(local_stack)->symbol_table,
                    node->IfElse.element_bind,
                    AST_UNSPECIFIED 
                );
            }

            // Analyze the "if" block
            if (node->IfElse.if_block) {
                semantic_analysis(node->IfElse.if_block, global_table, local_stack);
            }

            // Pop the frame after processing the "if" block
            pop_frame(local_stack);

            // Process the "else" block if it exists
            if (node->IfElse.else_block) {
                // Push a frame for the "else" block
                push_frame(local_stack);

                // Analyze the "else" block
                semantic_analysis(node->IfElse.else_block, global_table, local_stack);

                // Pop the frame after processing the "else" block
                pop_frame(local_stack);
            }

            break;
        }

        case AST_FN_CALL: {
            const char *fn_name = node->FnCall.fn_name;

            printf("fn_name: %s\n", fn_name);

            // Check if the function is built-in
            bool is_builtin = false;
            BuiltInFunction *builtin_func = NULL;  // No "struct" required
            for (size_t i = 0; i < sizeof(built_in_functions) / sizeof(built_in_functions[0]); i++) {
                if (strcmp(fn_name, built_in_functions[i].name) == 0) {
                    printf("fn: %s\n", built_in_functions[i].name);
                    is_builtin = true;
                    builtin_func = &built_in_functions[i];
                    break;
                }
            }

            if (is_builtin) {
                printf("is builtin\n");
                // Validate argument count for built-in functions
                if (builtin_func->param_count != -1 && node->FnCall.arg_count != builtin_func->param_count) {
                    fprintf(stderr, "Semantic Error: Built-in function '%s' expects %d arguments, but %d were provided.\n",
                            fn_name, builtin_func->param_count, node->FnCall.arg_count);
                    exit(SEMANTIC_ERROR_PARAMS);
                }

                // Validate argument types for built-in functions
                for (int i = 0; i < node->FnCall.arg_count; i++) {
                    DataType arg_type = evaluate_expression_type(node->FnCall.args[i], global_table, local_stack);

                    if (builtin_func->param_count != -1 && arg_type != builtin_func->expected_arg_types[i]) {
                        fprintf(stderr, "Semantic Error: Argument %d of built-in function '%s' has mismatched type. "
                                        "Expected '%d', but got '%d'.\n",
                                i + 1, fn_name, builtin_func->expected_arg_types[i], arg_type);
                        exit(SEMANTIC_ERROR_TYPE_COMPAT);
                    }
                }

                break;
            }

            // If not a built-in function, check user-defined functions
            Symbol *fn_symbol = lookup_symbol(global_table, fn_name);
            if (!fn_symbol || fn_symbol->type != SYMBOL_FUNC) {
                fprintf(stderr, "Semantic Error: Undefined function '%s'.\n", fn_name);
                exit(SEMANTIC_ERROR_UNDEFINED);
            }

            // Mark the function as used
            fn_symbol->func.used = true;

            // Check argument count for user-defined functions
            ScopeStack *fn_scope_stack = fn_symbol->func.scope_stack;

            if (!fn_scope_stack || fn_scope_stack->top < 0) {
                fprintf(stderr, "Internal Error: Missing scope stack for function '%s'.\n", fn_name);
                exit(INTERNAL_ERROR);
            }

            bool is_stored = true;
            
            // Ensure the return expression is NULL before checking if the return value is discarded.
            // Iterate through the frames in the function's scope stack
            for (int i = 0; i < fn_scope_stack->top + 1; i++) {
                Frame *current_frame = fn_scope_stack->frames[i];
                if (!current_frame || !current_frame->symbol_table) {
                    printf("Frame %d is NULL or has no symbol table.\n", i);
                    continue;
                }
                SymbolTable *current_table = current_frame->symbol_table;
                printf("Inspecting frame %d with symbol table capacity: %d\n", i, current_table->capacity);

                for (int j = 0; j < current_table->capacity; j++) {
                    Symbol *symbol = current_table->symbols[j];
                    if (!symbol) {
                        printf("Symbol at frame %d, index %d: NULL\n", i, j);
                        continue;
                    }

                    // Safely print details based on the symbol type
                    if (symbol->type == SYMBOL_VAR) {
                        printf("Symbol at frame %d, index %d: Variable '%s', type=%u, used=%s\n",
                            i, j,
                            symbol->var.name ? symbol->var.name : "(unnamed)",
                            symbol->var.type,
                            symbol->var.used ? "true" : "false");
                    } else if (symbol->type == SYMBOL_FUNC) {
                        printf("Symbol at frame %d, index %d: Function '%s', return type=%u\n",
                            i, j,
                            symbol->func.name ? symbol->func.name : "(unnamed)",
                            symbol->func.type);
                    } else {
                        printf("Symbol at frame %d, index %d: Unknown symbol type=%u\n", i, j, symbol->type);
                    }
                }
            }


            // Raise an error if the return value is not stored
            if (!is_stored) {
                fprintf(stderr, "Semantic Error: Return value discarded but the return value is not void.\n");
                exit(SEMANTIC_ERROR_TYPE_COMPAT);
            }


            Frame *fn_frame_args = fn_scope_stack->frames[0]; // Function parameters are in the first frame
            
            if (!fn_frame_args || !fn_frame_args->symbol_table) {
                fprintf(stderr, "Internal Error: Function parameter frame or symbol table is null.\n");
                exit(INTERNAL_ERROR);
            }

            SymbolTable *param_table = fn_frame_args->symbol_table;

            int expected_arg_count = param_table->count;
            printf("expected_arg_count: %d\n", expected_arg_count);
            if (node->FnCall.arg_count != expected_arg_count) {
                fprintf(stderr, "Semantic Error: Function '%s' expects %d arguments, but %d were provided.\n",
                        fn_name, expected_arg_count, node->FnCall.arg_count);
                exit(SEMANTIC_ERROR_PARAMS);
            }

            for (int i = 0; i < node->FnCall.arg_count; i++) {
                printf("argument_num: %d\n", i);
                ASTNode *arg = node->FnCall.args[i];
                DataType arg_type = evaluate_expression_type(arg, global_table, local_stack);
                printf("arg_type: %u\n", arg_type);

                // Locate the corresponding parameter in the hash table
                Symbol *param_symbol = NULL;
                int param_index = 0; // Logical parameter index

                for (int j = param_index; j < param_table->capacity; j++) {
                    Symbol *current_symbol = param_table->symbols[j];

                    // Skip NULL entries and non-parameter symbols
                    if (!current_symbol || current_symbol->type != SYMBOL_VAR) {
                        continue;
                    }

                    // Match the logical parameter index with the current argument index
                    if (param_index == i) {
                        param_symbol = current_symbol;
                        break;
                    }

                    // Increment logical parameter index
                    param_index++;
                }

                // Ensure the parameter symbol is found
                if (!param_symbol) {
                    fprintf(stderr, "Internal Error: Missing parameter %d in function '%s'.\n", i + 1, fn_name);
                    exit(INTERNAL_ERROR);
                }

                // Check type compatibility
                if (param_symbol->var.type != arg_type) {
                    fprintf(stderr, "Semantic Error: Argument %d of function '%s' has mismatched type. Expected '%d', but got '%d'.\n",
                            i + 1, fn_name, param_symbol->var.type, arg_type);
                    exit(SEMANTIC_ERROR_TYPE_COMPAT);
                }
            }

            break;
        }

        case AST_ARG: {
            if (!node->Argument.expression) {
                fprintf(stderr, "Semantic Error: Argument expression is missing.\n");
                exit(SEMANTIC_ERROR_UNDEFINED);
            }

            // Evaluate the type of the expression
            DataType arg_type = evaluate_expression_type(node->Argument.expression, global_table, local_stack);
            printf("arg_type: %u\n", arg_type);

            // Ensure the expression is valid (further checks can be added here if needed)
            if (arg_type == AST_UNSPECIFIED) {
                fprintf(stderr, "Semantic Error: Argument expression has an unspecified type.\n");
                exit(SEMANTIC_ERROR_TYPE_COMPAT);
            }

            break;
        }

        case AST_RETURN: {
            // Ensure the function context is valid (local stack must not be NULL)
            if (local_stack == NULL || local_stack->top == -1) {
                fprintf(stderr, "Semantic Error: Return statement outside of a function.\n");
                exit(SEMANTIC_ERROR_RETURN);
            }

            // Retrieve the first frame in the local stack (function scope)
            Frame *current_frame = local_stack->frames[0];
            if (!current_frame || !current_frame->symbol_table) {
                fprintf(stderr, "Internal Error: Function context not properly initialized.\n");
                exit(INTERNAL_ERROR);
            }

            // Find the function name directly from the global symbol table
            Symbol *function_symbol = NULL;
            for (int i = 0; i < global_table->capacity; i++) {
                Symbol *symbol = global_table->symbols[i];
                if (symbol && symbol->type == SYMBOL_FUNC && symbol->func.scope_stack == local_stack) {
                    function_symbol = symbol;
                    break;
                }
            }

            printf("Return Node Type: %d\n", node->Return.expression ? node->Return.expression->type : -1);

            // Ensure the function symbol is found
            if (!function_symbol) {
                fprintf(stderr, "Internal Error: Function context symbol not found in the global table.\n");
                exit(INTERNAL_ERROR);
            }

            DataType expected_return_type = function_symbol->func.type;
            printf("expected_return_type: %u\n", expected_return_type);

            // Check if the return statement has an expression
            if (node->Return.expression) {
                // Evaluate the type of the return expression
                DataType return_type = evaluate_expression_type(node->Return.expression, global_table, local_stack);

                // Check if the return type matches the expected return type
                if (return_type != expected_return_type) {
                    fprintf(stderr, "Semantic Error: Mismatched return type in function '%s'. Expected '%d', got '%d'.\n",
                            function_symbol->func.name, expected_return_type, return_type);
                    exit(SEMANTIC_ERROR_RETURN);
                }
            } else {
                // If no expression is provided, ensure the function's return type is void
                if (expected_return_type != AST_VOID) {
                    fprintf(stderr, "Semantic Error: Missing return value in function '%s' with non-void return type.\n",
                            function_symbol->func.name);
                    exit(SEMANTIC_ERROR_RETURN);
                }
            }

            break;
        }



        // case AST_VAR_DECL: {
        //     const char *var_name = node->VarDecl.var_name;
        //     DataType data_type = node->VarDecl.data_type;
        //     ASTNode *expression = node->VarDecl.expression;

        //     // Check if variable is already declared in the current scope
        //     Symbol *existing_var = lookup_symbol_in_scopes(NULL, local_stack, var_name);
        //     if (existing_var) {
        //         fprintf(stderr, "Semantic Error: Variable '%s' is already declared in the current scope.\n", var_name);
        //         exit(SEMANTIC_ERROR_REDEF);
        //     }

        //     // If data_type is unspecified, deduce it from the expression
        //     if (!expression) {
        //         fprintf(stderr, "Semantic Error: Constant '%s' must have an initializing expression.\n", var_name);
        //         exit(SEMANTIC_ERROR_TYPE_DERIVATION);
        //     }

        //     // If data_type is unspecified, deduce it from the expression
        //     if (data_type == AST_UNSPECIFIED) {
        //         printf("data_type == AST_UNSPECIFIED\n");
        //         // Check if the expression is a function call
        //         if (expression && expression->type == AST_FN_CALL) {

        //             // Perform semantic analysis on the function call
        //             semantic_analysis(expression, global_table, local_stack);

        //             // Optionally, deduce the data type of the function call
        //             Symbol *fn_symbol = lookup_symbol(global_table, expression->FnCall.fn_name);
        //             if (fn_symbol && fn_symbol->type == SYMBOL_FUNC) {
        //                 data_type = fn_symbol->func.type; // Deduce the return type of the function
        //             } else {
        //                 // Check if the function is built-in
        //                 bool is_builtin = false;
        //                 DataType builtin_return_type = AST_UNSPECIFIED; // Default for undefined

        //                 for (int i = 0; i < sizeof(built_in_functions) / sizeof(built_in_functions[0]); i++) {
        //                     if (strcmp(expression->FnCall.fn_name, built_in_functions[i].name) == 0) {
        //                         is_builtin = true;
        //                         builtin_return_type = built_in_functions[i].return_type;
        //                         break;
        //                     }
        //                 }

        //                 if (is_builtin) {
        //                     // Use the return type of the built-in function
        //                     data_type = builtin_return_type;
        //                 } else {
        //                     // If not found in built-in functions or global table, raise an error
        //                     fprintf(stderr, "Semantic Error: Function '%s' is undefined.\n", expression->FnCall.fn_name);
        //                     exit(SEMANTIC_ERROR_UNDEFINED);
        //                 }
        //             }
        //         } else {
        //             data_type = evaluate_expression_type(expression, global_table, local_stack);
        //         }

        //         printf("data_type: %u\n", data_type);

        //         // If still unspecified after deduction, raise an error
        //         if (data_type == AST_UNSPECIFIED) {
        //             fprintf(stderr, "Semantic Error: Cannot deduce type for constant '%s'.\n", var_name);
        //             exit(SEMANTIC_ERROR_TYPE_DERIVATION);
        //         }
        //     }

        //     // Add the var to the appropriate symbol table
        //     if (local_stack && local_stack->top >= 0) {
        //         Frame *current_frame = top_frame(local_stack);
        //         if (current_frame && current_frame->symbol_table) {
        //             printf("here 2\n");
        //             add_variable_symbol(current_frame->symbol_table, var_name, data_type);
        //         } else {
        //             fprintf(stderr, "Internal Error: No valid scope to declare constant '%s'.\n", var_name);
        //             exit(INTERNAL_ERROR);
        //         }
        //     } else {
        //         // Global scope
        //         add_variable_symbol(global_table, var_name, data_type);
        //     }

        //     break;
        // }

        case AST_CONST_DECL:
        case AST_VAR_DECL: {
            const char *const_name = node->ConstDecl.const_name;
            DataType data_type = node->ConstDecl.data_type;
            ASTNode *expression = node->ConstDecl.expression;

            // Check if constant is already declared in the current scope
            Symbol *existing_const = lookup_symbol_in_scopes(NULL, local_stack, const_name);
            if (existing_const) {
                fprintf(stderr, "Semantic Error: Constant '%s' is already declared in the current scope.\n", const_name);
                exit(SEMANTIC_ERROR_REDEF);
            }


            // Constants must have an expression
            if (!expression) {
                fprintf(stderr, "Semantic Error: Constant '%s' must have an initializing expression.\n", const_name);
                exit(SEMANTIC_ERROR_TYPE_DERIVATION);
            }

            // If data_type is unspecified, deduce it from the expression
            if (data_type == AST_UNSPECIFIED) {
                printf("data_type == AST_UNSPECIFIED\n");
                // Check if the expression is a function call
                if (expression && expression->type == AST_FN_CALL) {

                    // Perform semantic analysis on the function call
                    semantic_analysis(expression, global_table, local_stack);

                    // Optionally, deduce the data type of the function call
                    Symbol *fn_symbol = lookup_symbol(global_table, expression->FnCall.fn_name);
                    if (fn_symbol && fn_symbol->type == SYMBOL_FUNC) {
                        data_type = fn_symbol->func.type; // Deduce the return type of the function
                    } else {
                        // Check if the function is built-in
                        bool is_builtin = false;
                        DataType builtin_return_type = AST_UNSPECIFIED; // Default for undefined

                        // Correctly typed loop for iteration
                        size_t built_in_count = sizeof(built_in_functions) / sizeof(built_in_functions[0]);
                        for (size_t i = 0; i < built_in_count; i++) {
                            if (strcmp(expression->FnCall.fn_name, built_in_functions[i].name) == 0) {
                                is_builtin = true;
                                builtin_return_type = built_in_functions[i].return_type;
                                break;
                            }
                        }


                        if (is_builtin) {
                            // Use the return type of the built-in function
                            data_type = builtin_return_type;
                        } else {
                            // If not found in built-in functions or global table, raise an error
                            fprintf(stderr, "Semantic Error: Function '%s' is undefined.\n", expression->FnCall.fn_name);
                            exit(SEMANTIC_ERROR_UNDEFINED);
                        }
                    }
                } else {
                    data_type = evaluate_expression_type(expression, global_table, local_stack);
                }

                printf("data_type: %u\n", data_type);

                // If still unspecified after deduction, raise an error
                if (data_type == AST_UNSPECIFIED) {
                    fprintf(stderr, "Semantic Error: Cannot deduce type for constant '%s'.\n", const_name);
                    exit(SEMANTIC_ERROR_TYPE_DERIVATION);
                }
            }

            // Add the constant to the appropriate symbol table
            if (local_stack && local_stack->top >= 0) {
                Frame *current_frame = top_frame(local_stack);
                if (current_frame && current_frame->symbol_table) {
                    printf("local-stack-const-added\n");
                    add_variable_symbol(current_frame->symbol_table, const_name, data_type);
                } else {
                    fprintf(stderr, "Internal Error: No valid scope to declare constant '%s'.\n", const_name);
                    exit(INTERNAL_ERROR);
                }
            } else {
                // Global scope
                add_variable_symbol(global_table, const_name, data_type);
            }
            printf("arrived at break\n");
            break;
        }

        case AST_FLOAT: {
            // Check if the floating-point value is within the bounds of double-precision float.
            if (node->Float.number < -DBL_MAX || node->Float.number > DBL_MAX) {
                fprintf(stderr, "Semantic Error: Float literal '%f' is out of bounds for type 'f64'.\n", node->Float.number);
                exit(SEMANTIC_ERROR_TYPE_COMPAT);
            }
            break;
        }

        case AST_INT: {
            // Check if the integer is within the bounds of the data type (AST_I32).
            if (node->Integer.number < INT32_MIN || node->Integer.number > INT32_MAX) {
                fprintf(stderr, "Semantic Error: Integer literal '%d' is out of bounds for type 'i32'.\n", node->Integer.number);
                exit(SEMANTIC_ERROR_TYPE_COMPAT);
            }
            break;
        }

        case AST_STRING: {
            const size_t MAX_STRING_LENGTH = 1024; // Example limit
            if (strlen(node->String.string) > MAX_STRING_LENGTH) {
                fprintf(stderr, "Semantic Error: String literal exceeds the maximum allowed length (%zu).\n", MAX_STRING_LENGTH);
                exit(SEMANTIC_ERROR_TYPE_COMPAT);
            }
            break;
        }

        case AST_IDENTIFIER: {
            // Look up the identifier in the current scope stack or global table.
            Symbol *symbol = lookup_symbol_in_scopes(global_table, local_stack, node->Identifier.identifier);

            if (!symbol) {
                // If the identifier is not found, it's an undeclared variable.
                fprintf(stderr, "Semantic Error: Undeclared identifier '%s'.\n", node->Identifier.identifier);
                exit(SEMANTIC_ERROR_UNDEFINED);
            }

            printf("got here\n");
            
            // Mark the symbol as used.
            if (symbol->type == SYMBOL_VAR) {
                symbol->var.used = true;
            } else if (symbol->type == SYMBOL_FUNC) {
                // Ensure that identifiers aren't functions in invalid contexts.
                fprintf(stderr, "Semantic Error: '%s' is a function, not a variable.\n", node->Identifier.identifier);
                exit(SEMANTIC_ERROR_TYPE_COMPAT);
            }

            break;
        
        }

        case AST_ASSIGNMENT: {
            const char *identifier = node->Assignment.identifier;

            // Lookup the identifier in the symbol table (local or global)
            Symbol *symbol = lookup_symbol_in_scopes(global_table, local_stack, identifier);

            if (!symbol) {
                // If the variable/constant is not declared, raise an error
                fprintf(stderr, "Semantic Error: Undeclared variable or constant '%s'.\n", identifier);
                exit(SEMANTIC_ERROR_UNDEFINED);
            }

            // Check if the symbol is a constant
            if (symbol->type == AST_CONST_DECL) {
                // Constants cannot be modified
                fprintf(stderr, "Semantic Error: Cannot assign to constant '%s'.\n", identifier);
                exit(SEMANTIC_ERROR_REDEF);
            }

            // If it's a variable, mark it as redefined
            if (symbol->type == SYMBOL_VAR) {
                symbol->var.redefined = true;
            }

            // Evaluate the expression type for the right-hand side of the assignment
            DataType expression_type = evaluate_expression_type(node->Assignment.expression, global_table, local_stack);

            // Check type compatibility
            if (symbol->var.type != expression_type) {
                fprintf(stderr, "Semantic Error: Type mismatch in assignment to '%s'. Expected '%d', got '%d'.\n", identifier, symbol->var.type, expression_type);
                exit(SEMANTIC_ERROR_TYPE_COMPAT);
            }

            // Mark the variable/constant as used
            if (symbol->type == SYMBOL_VAR) {
                symbol->var.used = true;
            }

            break;
        }

        default: {
            fprintf(stderr, "Error: Unsupported AST node type.\n");
            exit(OTHER_SEMANTIC_ERROR);
        }
    }
}


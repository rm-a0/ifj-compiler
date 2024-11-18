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
#include "../inc/parser.h"
#include "../inc/ast.h"
#include "../inc/stack.h"
#include "../inc/error.h"
#include "../inc/semantic_analysis.h"

#include <stdio.h>

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
    if (main_symbol->func.scope_stack == NULL || main_symbol->func.scope_stack->top != -1) {
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
    switch (node->type) {
        case AST_INT:
            return AST_I32;
        case AST_FLOAT:
            return AST_F64;
        case AST_STRING:
            return AST_SLICE;
        case AST_IDENTIFIER: {
            Symbol *symbol = lookup_symbol_in_scopes(global_table, local_stack, node->Identifier.identifier);
            if (!symbol) {
                fprintf(stderr, "Semantic Error: Undeclared variable '%s'.\n", node->Identifier.identifier);
                exit(SEMANTIC_ERROR_UNDEFINED);
            }
            return symbol->var.type;
        }
        case AST_BIN_OP:
            // Perform recursive evaluation of binary operations
            // For example, int + int = int; int + float = float
            return evaluate_binary_operator_type(node, global_table, local_stack);
        default:
            fprintf(stderr, "Semantic Error: Unsupported expression type.\n");
            exit(SEMANTIC_ERROR_RETURN);
    }
}



void semantic_analysis(ASTNode *node, SymbolTable *global_table, ScopeStack *local_stack) {

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

            // Check if the function is already declared in the global symbol table
            Symbol *existing_symbol = lookup_symbol(global_table, fn_name);
            if (existing_symbol != NULL) {
                fprintf(stderr, "Semantic Error: Function '%s' is already declared.\n", fn_name);
                exit(SEMANTIC_ERROR_REDEF);
            }

            // Add the function symbol to the global symbol table
            add_function_symbol(global_table, fn_name, return_type);

            // Retrieve the function symbol to access its scope stack to check if the function was defined
            Symbol *fn_symbol = lookup_symbol(global_table, fn_name);
            if (fn_symbol == NULL || fn_symbol->type != SYMBOL_FUNC) {
                fprintf(stderr, "Internal Error: Failed to retrieve function '%s' symbol.\n", fn_name);
                exit(SEMANTIC_ERROR_UNDEFINED);
            }

            // Create a local stack for the function
            ScopeStack *function_stack = init_scope_stack();

            // Push a new frame for the function body
            push_frame(function_stack);

            // Process function parameters
            for (int i = 0; i < node->FnDecl.param_count; i++) {
                // ASTNode *param = node->FnDecl.params[i];

                // TODO: check if syntax error or semantic, for example no func. decleration as param.

                // Ensure the parameter node is valid
                // if (param->type != AST_PARAM) {
                //     fprintf(stderr, "Semantic Error: Invalid parameter in function '%s'.\n", fn_name);
                //     exit(SEMANTIC_ERROR_PARAMS);
                // }

                // const char *param_name = param->Param.identifier;
                // DataType param_type = param->Param.data_type;

                // TODO: If two parameters in the same function have the same name is it syntax or semantic error ?

                // Check if the parameter already exists in the local scope
                // Symbol *existing_param = lookup_symbol(function_stack->frames[function_stack->top]->symbol_table, param_name);
                // if (existing_param != NULL) {
                //     fprintf(stderr, "Semantic Error: Duplicate parameter name '%s' in function '%s'.\n", param_name, fn_name);
                //     exit(SEMANTIC_ERROR_PARAMS);
                // }

                // Add the parameter to the current frame's symbol table
                semantic_analysis(node->FnDecl.params[i], global_table, local_stack);
            }

            // Recursively analyze the function block
            // TODO: check if it is error if NULL
            semantic_analysis(node->FnDecl.block, global_table, function_stack);

            // TODO: Check if 2 return statements is semantic or syntax error
            // Check for a return statement
            bool return_found = false;
            for (int i = 0; i < node->FnDecl.block->Block.node_count; i++) {
                ASTNode *child_node = node->FnDecl.block->Block.nodes[i];
                if (child_node->type == AST_RETURN) {
                    semantic_analysis(child_node, global_table, function_stack);
                    return_found = true;
                }
            }

            // If no return statement is found, enforce rules for non-void functions
            if (!return_found && return_type != AST_VOID) {
                fprintf(stderr, "Semantic Error: Function '%s' must have a return statement.\n", fn_name);
                exit(SEMANTIC_ERROR_RETURN);
            }

            // Cleanup the function's scope stack
            while (function_stack->top >= 0) {
                pop_frame(function_stack);
            }

            free_scope_stack(function_stack);
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
            }

            // After analyzing all nodes in the block, check for unused variables
            Frame *current_frame = top_frame(local_stack);
            if (current_frame && current_frame->symbol_table) {
                SymbolTable *current_table = current_frame->symbol_table;

                for (int j = 0; j < current_table->capacity; j++) {
                    Symbol *symbol = current_table->symbols[j];
                    if (symbol && symbol->type == SYMBOL_VAR && !symbol->var.used) {
                        fprintf(stderr, "Semantic Error: Variable '%s' declared in the block was never used.\n", symbol->var.name);
                        exit(SEMANTIC_ERROR_UNUSED_VAR); // Use appropriate error code
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
            // Analyze the condition expression
            if (node->IfElse.expression) {
                DataType condition_type = evaluate_expression_type(node->IfElse.expression, global_table, local_stack);
                if (condition_type != AST_I32) { // Assuming the condition must evaluate to int
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
            print_global_symbol_table(global_table);

            // Lookup the function in the global symbol table
            Symbol *fn_symbol = lookup_symbol(global_table, fn_name);
            if (!fn_symbol || fn_symbol->type != SYMBOL_FUNC) {
                fprintf(stderr, "Semantic Error: Undefined function '%s'.\n", fn_name);
                exit(SEMANTIC_ERROR_UNDEFINED);
            }

            // Mark the function as used
            fn_symbol->func.used = true;

            // Check the argument count
            ScopeStack *fn_scope_stack = fn_symbol->func.scope_stack;
            if (!fn_scope_stack || fn_scope_stack->top < 0) {
                fprintf(stderr, "Internal Error: Missing scope stack for function '%s'.\n", fn_name);
                exit(INTERNAL_ERROR);
            }

            Frame *fn_frame = fn_scope_stack->frames[0]; // Function parameters are in the first frame
            SymbolTable *param_table = fn_frame->symbol_table;

            int expected_arg_count = param_table->count;
            if (node->FnCall.arg_count != expected_arg_count) {
                fprintf(stderr, "Semantic Error: Function '%s' expects %d arguments, but %d were provided.\n", fn_name, expected_arg_count, node->FnCall.arg_count);
                exit(SEMANTIC_ERROR_PARAMS);
            }

            // Validate argument types
            for (int i = 0; i < node->FnCall.arg_count; i++) {
                // Evaluate the type of the argument
                ASTNode *arg = node->FnCall.args[i];
                DataType arg_type = evaluate_expression_type(arg, global_table, local_stack);

                // Lookup the corresponding parameter
                for (int j = 0; j < param_table->capacity; j++) {
                    Symbol *param_symbol = param_table->symbols[j];
                    if (param_symbol && param_symbol->type == SYMBOL_VAR && param_symbol->var.used == false) {
                        // If the types don't match, throw an error
                        if (param_symbol->var.type != arg_type) {
                            fprintf(stderr, "Semantic Error: Argument %d of function '%s' has mismatched type. Expected '%d', but got '%d'.\n",
                                    i + 1, fn_name, param_symbol->var.type, arg_type);
                            exit(SEMANTIC_ERROR_PARAMS);
                        }
                    }
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

            // Retrieve the current function's symbol table and its return type
            Frame *current_frame = local_stack->frames[0]; // The first frame in the stack corresponds to the function's scope
            if (!current_frame || !current_frame->symbol_table) {
                fprintf(stderr, "Internal Error: Function context not properly initialized.\n");
                exit(INTERNAL_ERROR);
            }

            // Lookup the function declaration in the global symbol table
            Symbol *function_symbol = lookup_symbol(global_table, current_frame->symbol_table->symbols[0]->func.name); // Assuming the first symbol is the function name
            if (!function_symbol || function_symbol->type != SYMBOL_FUNC) {
                fprintf(stderr, "Internal Error: Function context symbol not found.\n");
                exit(INTERNAL_ERROR);
            }

            DataType expected_return_type = function_symbol->func.type;

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

        case AST_VAR_DECL: {
            const char *var_name = node->VarDecl.var_name;
            DataType data_type = node->VarDecl.data_type;
            ASTNode *expression = node->VarDecl.expression;

            // If data_type is unspecified, deduce it from the expression
            if (data_type == AST_UNSPECIFIED) {
                if (!expression) {
                    fprintf(stderr, "Semantic Error: Variable '%s' has no type or expression to deduce type.\n", var_name);
                    exit(SEMANTIC_ERROR_TYPE_DERIVATION);
                }

                // Deduce data_type from the expression
                data_type = evaluate_expression_type(expression, global_table, local_stack);

                // If still unspecified after deduction, raise an error
                if (data_type == AST_UNSPECIFIED) {
                    fprintf(stderr, "Semantic Error: Cannot deduce type for variable '%s'.\n", var_name);
                    exit(SEMANTIC_ERROR_TYPE_DERIVATION);
                }
            }

            // Check if variable is already declared in the current scope
            Symbol *existing_var = lookup_symbol_in_scopes(NULL, local_stack, var_name);
            if (existing_var) {
                fprintf(stderr, "Semantic Error: Variable '%s' is already declared in the current scope.\n", var_name);
                exit(SEMANTIC_ERROR_REDEF);
            }

            // Add the variable to the current frame's symbol table
            Frame *current_frame = top_frame(local_stack);
            if (!current_frame || !current_frame->symbol_table) {
                fprintf(stderr, "Internal Error: No valid scope to declare variable '%s'.\n", var_name);
                exit(INTERNAL_ERROR);
            }

            // Add the variable to the symbol table
            add_variable_symbol(local_stack ? top_frame(local_stack)->symbol_table : global_table, var_name, data_type);

            break;
        }

        case AST_CONST_DECL: {
            const char *const_name = node->ConstDecl.const_name;
            DataType data_type = node->ConstDecl.data_type;
            ASTNode *expression = node->ConstDecl.expression;

            // Constants must have an expression
            if (!expression) {
                fprintf(stderr, "Semantic Error: Constant '%s' must have an initializing expression.\n", const_name);
                exit(SEMANTIC_ERROR_TYPE_DERIVATION);
            }

            // If data_type is unspecified, deduce it from the expression
            if (data_type == AST_UNSPECIFIED) {
                data_type = evaluate_expression_type(expression, global_table, local_stack);

                // If still unspecified after deduction, raise an error
                if (data_type == AST_UNSPECIFIED) {
                    fprintf(stderr, "Semantic Error: Cannot deduce type for constant '%s'.\n", const_name);
                    exit(SEMANTIC_ERROR_TYPE_DERIVATION);
                }
            }

            // Check if constant is already declared in the current scope
            Symbol *existing_const = lookup_symbol_in_scopes(NULL, local_stack, const_name);
            if (existing_const) {
                fprintf(stderr, "Semantic Error: Constant '%s' is already declared in the current scope.\n", const_name);
                exit(SEMANTIC_ERROR_REDEF);
            }

            // Add the constant to the current frame's symbol table
            Frame *current_frame = top_frame(local_stack);
            if (!current_frame || !current_frame->symbol_table) {
                fprintf(stderr, "Internal Error: No valid scope to declare constant '%s'.\n", const_name);
                exit(INTERNAL_ERROR);
            }

            // Add the constant as a variable
            add_variable_symbol(local_stack ? top_frame(local_stack)->symbol_table : global_table, const_name, data_type);
            
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

        // case AST_STRING:
        //     const size_t MAX_STRING_LENGTH = 1024; // Example limit
        //     if (strlen(node->String.string) > MAX_STRING_LENGTH) {
        //         fprintf(stderr, "Semantic Error: String literal exceeds the maximum allowed length (%zu).\n", MAX_STRING_LENGTH);
        //         exit(SEMANTIC_ERROR_TYPE_COMPAT);
        //     }
        //     break;

        case AST_IDENTIFIER: {
            // Look up the identifier in the current scope stack or global table.
            Symbol *symbol = lookup_symbol_in_scopes(global_table, local_stack, node->Identifier.identifier);

            if (!symbol) {
                // If the identifier is not found, it's an undeclared variable.
                fprintf(stderr, "Semantic Error: Undeclared identifier '%s'.\n", node->Identifier.identifier);
                exit(SEMANTIC_ERROR_UNDEFINED);
            }

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


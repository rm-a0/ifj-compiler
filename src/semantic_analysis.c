/**
 * @file semantic_analysis.c
 * @brief File implementing semantic analysis through recursive descent
 * @authors Alex Marinica (xmarina00)
*/

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <stdint.h>
#include <math.h>
#include <limits.h>
#include "parser.h"
#include "ast.h"
#include "stack.h"
#include "error.h"
#include "semantic_analysis.h"

void process_declaration(
    SymbolTable *global_table,
    ScopeStack *local_stack,
    const char *name,
    DataType data_type,
    ASTNode *expression,
    bool is_constant,
    bool is_nullable,
    double value
);

typedef struct {
    const char *name;               // Name of the built-in function
    int param_count;                // Number of parameters (-1 for variable arguments)
    DataType expected_arg_types[3]; // Array of expected argument types (up to 3 for simplicity)
    DataType return_type;           // Return type of the function
    bool is_nullable;
} BuiltInFunction;

BuiltInFunction built_in_functions[] = {
    {"ifj.write", -1, {AST_UNSPECIFIED, AST_UNSPECIFIED, AST_UNSPECIFIED}, AST_VOID, false},
    {"ifj.readstr", 0, {AST_UNSPECIFIED, AST_UNSPECIFIED, AST_UNSPECIFIED}, AST_SLICE, true},
    {"ifj.readi32", 0, {AST_UNSPECIFIED, AST_UNSPECIFIED, AST_UNSPECIFIED}, AST_I32, true},
    {"ifj.readf64", 0, {AST_UNSPECIFIED, AST_UNSPECIFIED, AST_UNSPECIFIED}, AST_F64, true},
    {"ifj.i2f", 1, {AST_I32, AST_UNSPECIFIED, AST_UNSPECIFIED}, AST_F64, false},
    {"ifj.f2i", 1, {AST_F64, AST_UNSPECIFIED, AST_UNSPECIFIED}, AST_I32, false},
    {"ifj.length", 1, {AST_SLICE, AST_UNSPECIFIED, AST_UNSPECIFIED}, AST_I32, false},
    {"ifj.concat", 2, {AST_SLICE, AST_SLICE, AST_UNSPECIFIED}, AST_SLICE, false},
    {"ifj.substring", 3, {AST_SLICE, AST_I32, AST_I32}, AST_SLICE, true},
    {"ifj.strcmp", 2, {AST_SLICE, AST_SLICE, AST_UNSPECIFIED}, AST_I32, false},
    {"ifj.ord", 2, {AST_SLICE, AST_I32, AST_UNSPECIFIED}, AST_I32, false},
    {"ifj.chr", 1, {AST_I32, AST_UNSPECIFIED, AST_UNSPECIFIED}, AST_SLICE, false},
    {"ifj.string", 1, {AST_SLICE, AST_UNSPECIFIED, AST_UNSPECIFIED}, AST_SLICE, false}
};

// Function to check if the operator is a valid relational operator
bool isRelationalOperator(OperatorType op) {
    static const OperatorType validOperators[] = {
        AST_GREATER, AST_GREATER_EQU, AST_LESS, AST_LESS_EQU, AST_EQU, AST_NOT_EQU
    };
    for (size_t i = 0; i < sizeof(validOperators) / sizeof(validOperators[0]); ++i) {
        if (op == validOperators[i]) {
            
            return true;
        }
    }
    return false;
}


unsigned int unused_vars_funcs_frame(SymbolTable *current_table) {

    for (int j = 0; j < current_table->capacity; j++) {
        Symbol *symbol = current_table->symbols[j];

        // We skip symbol which has not been assigned a value (null)
        if (!symbol) {
            continue;
        }

        // As stated in documentation, variable shall not stay unused, thereby we oversee its usage
        if (symbol && symbol->type == SYMBOL_VAR && !symbol->var.used) {
            
            return SEMANTIC_ERROR_UNUSED_VAR;
        
        // Checking whether the function has been used within the flow of the program unless the function is main
        } else if (symbol && symbol->type == SYMBOL_FUNC && !symbol->func.used && !(strcmp(symbol->func.name, "main") == 0)) {

            return OTHER_SEMANTIC_ERROR;
        }
    }

    // Return 0 indicates all symbols have been used
    return 0;
}

void process_binding(ASTNode *expression, SymbolTable *global_table, ScopeStack *local_stack, Frame *current_frame, const char *bind_name, DataType condition_type, bool has_literal) {
    OperatorType operator = expression->BinaryOperator.operator;

    // No operator present in the expression should result in error
    if (!operator) {
        exit(SEMANTIC_ERROR_TYPE_COMPAT);
    }

    // Perform semantic analysis on the identifier / function call
    semantic_analysis(expression, global_table, local_stack);

    if (expression->type == AST_IDENTIFIER) {

        // Look up the symbol in the scope
        const char *name = expression->Identifier.identifier;
        Symbol *identifier = lookup_symbol_in_scope(local_stack, name, current_frame);

        // Identifiers which are not nullable can't be used in binding
        if (!identifier->var.is_nullable) {
            exit(OTHER_SEMANTIC_ERROR);
        }

        // Identifier is added to the appropriate symbol table with temporary variable which behaves as
        // constant declaration and inherits has_literal, value, nullability and type attributes from the binding variable
        add_variable_symbol(current_frame->symbol_table, bind_name, condition_type, true, false, has_literal, identifier->var.value);
        return;

    } else if (expression->type == AST_FN_CALL) {

        // Look up the function in the global table
        const char *fn_name = expression->FnCall.fn_name;
        Symbol *fn_symbol = lookup_symbol(global_table, fn_name);

        // Functions which are not nullable can't be used in binding
        if (fn_symbol && !fn_symbol->func.is_nullable) {
            exit(OTHER_SEMANTIC_ERROR);
        }

        // Function call is added to the appropriate symbol table with temporary variable which behaves as constant declaration 
        // constant declaration and doesnt inherit value, because there is none, inherits just function type and and constant declaration behaviour
        add_variable_symbol(current_frame->symbol_table, bind_name, fn_symbol->func.type, true, false, false, 0);
        return;

    } else if (expression->type == AST_NULL) {

        // Even null can be used as binding operator so we add it too according to the function parameters
        add_variable_symbol(current_frame->symbol_table, bind_name, condition_type, true, false, has_literal, 0);
        return;
        
    }

    
    exit(SEMANTIC_ERROR_TYPE_COMPAT);
}

DataType evaluate_condition(ASTNode *expression, SymbolTable *global_table, ScopeStack *local_stack, Frame *current_frame) {
    DataType condition_type = evaluate_expression_type(expression, global_table, local_stack, current_frame);

    // Ensures the condition is a boolean-compatible type
    if (condition_type != AST_I32) { 
        exit(SEMANTIC_ERROR_TYPE_COMPAT);
    }

    return condition_type;
}


void check_main_function(SymbolTable *global_table) {

    // Lookup the main() function in the global symbol table
    Symbol *main_symbol = lookup_symbol(global_table, "main");

    // Main function must be present in the code so we check if the main function has been declared
    if (main_symbol == NULL || main_symbol->type != SYMBOL_FUNC) {
        exit(SEMANTIC_ERROR_UNDEFINED);
    }

    // Main function has to be of type void, nothing else
    if (main_symbol->func.type != AST_VOID) {
        exit(SEMANTIC_ERROR_PARAMS);
    }

    ScopeStack *main_scope_stack = main_symbol->func.scope_stack;

    // Check if the main function has zero parameters, as it should according to the assignment
    if (main_scope_stack == NULL || main_scope_stack->frames[0]->symbol_table->count > 0) {
        
        exit(SEMANTIC_ERROR_PARAMS);
    }
}

DataType deduce_builtin_function_type(const char *fn_name) {

    size_t built_in_count = sizeof(built_in_functions) / sizeof(built_in_functions[0]);
    for (size_t i = 0; i < built_in_count; i++) {
        if (strcmp(fn_name, built_in_functions[i].name) == 0) {
            
            return built_in_functions[i].return_type;
        }
    }

    exit(SEMANTIC_ERROR_UNDEFINED);
}

bool evaluate_nullable_operand(SymbolTable *global_table, ASTNode *node, ScopeStack *local_stack, Frame *local_frame) {
    
    // Setting flag for nullability which is used in evaluate_operator_type function to set flags when the operand is nullable
    Symbol *symbol = lookup_symbol_in_scope(local_stack, node->Identifier.identifier, local_frame);
    
    // No other node than identifier or function or identifier can get here so no check required
    bool is_nullable = false;
    if (node->type == AST_IDENTIFIER) {
        Symbol *symbol = lookup_symbol_in_scope(local_stack, node->Identifier.identifier, local_frame);
        is_nullable = symbol->var.is_nullable;
    } else {
        Symbol *symbol = lookup_symbol(global_table, node->FnCall.fn_name);
        const char *fn_name = node->FnCall.fn_name;
        if (!symbol) {
            for (size_t j = 0; j < sizeof(built_in_functions) / sizeof(built_in_functions[0]); j++) {
                if (strcmp(fn_name, built_in_functions[j].name) == 0 && built_in_functions[j].is_nullable) {
                    is_nullable = true;
                }
            }
        } else {
            is_nullable = symbol->func.is_nullable;
        }
    }

    return is_nullable;
}

bool is_literal(ASTNode *operation_element) {
    return operation_element->type == AST_INT || operation_element->type == AST_FLOAT;
}

DataType evaluate_operator_type(ASTNode *node, SymbolTable *global_table, ScopeStack *local_stack, Frame *local_frame) {

    bool left_is_nullable = false;
    bool right_is_nullable = false;

    // Evaluate the left and right operand types
    DataType left_type = evaluate_expression_type(node->BinaryOperator.left, global_table, local_stack, local_frame);
    if (node->BinaryOperator.left->type == AST_IDENTIFIER || node->BinaryOperator.left->type == AST_FN_CALL) {
        left_is_nullable = evaluate_nullable_operand(global_table, node->BinaryOperator.left, local_stack, local_frame);
    }

    DataType right_type = evaluate_expression_type(node->BinaryOperator.right, global_table, local_stack, local_frame);
    if (node->BinaryOperator.right->type == AST_IDENTIFIER || node->BinaryOperator.right->type == AST_FN_CALL) {
        right_is_nullable = evaluate_nullable_operand(global_table, node->BinaryOperator.right, local_stack, local_frame);
    }

    

    // Determine the operator type
    OperatorType operator = node->BinaryOperator.operator;
    

    // Check type compatibility based on the operator
    switch (operator) {
        case AST_PLUS:
        case AST_MINUS:
        case AST_MUL:
        case AST_DIV: {

            // Arithmetic operators
            if ((left_type == AST_I32 && right_type == AST_I32) || 
                (left_type == AST_F64 && right_type == AST_F64)) {
                return left_type;

            } else if ((left_type == AST_I32 && right_type == AST_F64) || 
                    (left_type == AST_F64 && right_type == AST_I32)) {
                // Check if the integer operand is a literal
                if ((left_type == AST_I32 && right_type == AST_F64 && node->BinaryOperator.left->type == AST_INT) ||
                    (left_type == AST_F64 && right_type == AST_I32 && node->BinaryOperator.right->type == AST_INT)) {
                    // Implicit conversion of integer literal to F64
                    return AST_F64; 

                } else if (node->BinaryOperator.left->type == AST_BIN_OP || 
                           node->BinaryOperator.right->type == AST_BIN_OP) {
                    // Evaluate nested binary operations recursively

                    ASTNode *bin_operator_with_binary_operation = node->BinaryOperator.left->type == AST_BIN_OP ?  
                                                                  node->BinaryOperator.left : node->BinaryOperator.right;
                    DataType bin_operation_type = evaluate_operator_type(bin_operator_with_binary_operation, 
                                                                        global_table, local_stack, local_frame);
                    return bin_operation_type;

                } else {
                    exit(SEMANTIC_ERROR_TYPE_COMPAT);
                }

            } else {
                // If no cases matched, it's a semantic error
                exit(SEMANTIC_ERROR_TYPE_COMPAT);
            }
        }


        case AST_GREATER:
        case AST_GREATER_EQU:
        case AST_LESS:
        case AST_LESS_EQU: {

            // Check for nullability (assignment requirement)
            if (left_is_nullable || right_is_nullable) {
                
                exit(SEMANTIC_ERROR_TYPE_COMPAT);
            }

            // Prevent relational operators on slices or strings (assignment requirement)
            if (left_type == AST_SLICE || right_type == AST_SLICE ||
                left_type == AST_STRING || right_type == AST_STRING) {
                
                exit(SEMANTIC_ERROR_TYPE_COMPAT);
            }

            if ((left_type == AST_I32 && right_type == AST_I32) ||
                (left_type == AST_F64 && right_type == AST_F64)) {
                // Both operands are of the same numeric type
                return AST_I32; // Representing boolean as an integer type

            } else if ((left_type == AST_F64 && right_type == AST_I32 && is_literal(node->BinaryOperator.right))) {
                // Implicitly convert integer literal to f64
                return AST_I32;

            } else if ((left_type == AST_I32 && right_type == AST_F64 && is_literal(node->BinaryOperator.left))) {
                // Implicitly convert integer literal to f64
                return AST_I32;

            } else if (left_type == AST_BIN_OP) {
                // Evaluate left operand recursively
                return evaluate_operator_type(node->BinaryOperator.left, global_table, local_stack, local_frame);

            } else if (right_type == AST_BIN_OP) {
                // Evaluate right operand recursively
                return evaluate_operator_type(node->BinaryOperator.right, global_table, local_stack, local_frame);

            } else {
                
                exit(SEMANTIC_ERROR_TYPE_COMPAT);
            }  
        } 

        case AST_EQU:
        case AST_NOT_EQU: {

            if ((left_is_nullable || right_is_nullable) &&
                (left_type == right_type || left_type == AST_UNSPECIFIED || right_type == AST_UNSPECIFIED)) {
                return AST_I32;

            } else if ((left_type == AST_I32 && right_type == AST_I32) || (left_type == AST_F64 && right_type == AST_F64)) {
                return AST_I32;

            } else if ((left_type == AST_I32 && right_type == AST_F64 && node->BinaryOperator.left->type == AST_INT) ||
                    (left_type == AST_F64 && right_type == AST_I32 && node->BinaryOperator.right->type == AST_INT)) {
                return AST_I32; // Implicit conversion for literals

            } else {
                
                exit(SEMANTIC_ERROR_TYPE_COMPAT);
            }

        }

        default: {
            
            exit(SEMANTIC_ERROR_TYPE_COMPAT);
        }
    }
}

DataType evaluate_expression_type(ASTNode *node, SymbolTable *global_table, ScopeStack *local_stack, Frame *local_frame) {

    
    

    switch (node->type) {
        case AST_INT: {
            return AST_I32;

        } case AST_FLOAT: {
            return AST_F64;

        } case AST_STRING: {
            return AST_SLICE;

        } case AST_IDENTIFIER: { 
            Symbol *symbol = lookup_symbol_in_scope(local_stack, node->Identifier.identifier, local_frame);
            
            if (!symbol) {
                
                exit(SEMANTIC_ERROR_UNDEFINED);
            }
            
            // when var used in fun. mark it as used 
            if (symbol->type == SYMBOL_VAR) {
                symbol->var.used = true;
            }
            
            
            return symbol->var.type;
        } case AST_BIN_OP: {
            
            return evaluate_operator_type(node, global_table, local_stack, local_frame);

        } case AST_ARG: {
            if (!node->Argument.expression) {
                
                exit(SEMANTIC_ERROR_UNDEFINED);
            }
            // Recurs. eval. expression
            return evaluate_expression_type(node->Argument.expression, global_table, local_stack, local_frame);

        } case AST_FN_CALL: {
            const char *fn_name = node->FnCall.fn_name;
    
            // Lookup the function symbol
            Symbol *fn_symbol = lookup_symbol(global_table, fn_name);
            
            semantic_analysis(node, global_table, local_stack);

            if (!fn_symbol) {
                
                DataType built_in_fn_data_type = deduce_builtin_function_type(fn_name);
                return built_in_fn_data_type;

            } else {
                // Return the function's return type
                
                return fn_symbol->func.type;
            }

        } case AST_NULL: {
            
            return AST_UNSPECIFIED;
        }

        default: 
            
            exit(SEMANTIC_ERROR_RETURN);
    }
}


void check_initialization(ASTNode *expression, bool is_nullable, const char *name, bool is_constant) {
    if (expression->type == AST_NULL && !is_nullable) {
        exit(SEMANTIC_ERROR_TYPE_DERIVATION);
    }
}

void check_type_compatibility(DataType data_type_declared, DataType data_type_stored, bool is_nullable, double value, bool expression_is_literal) {
    if (data_type_declared != data_type_stored) {

        

        // Implicit conversion is allowed only if the expression is a literal
        if (expression_is_literal) {
            if (data_type_declared == AST_F64 && data_type_stored == AST_I32) {
                // Assigning integer literal to f64 variable
                // Implicit conversion allowed
                return;
            } else if (data_type_declared == AST_I32 && data_type_stored == AST_F64) {
                // Assigning f64 literal to i32 variable
                // Check if value is an exact integer within i32 range

                if (fabs(value - round(value)) < 1e-9 && value >= INT_MIN && value <= INT_MAX) {
                    // Value is effectively an integer within i32 range
                    // Implicit conversion allowed
                    return;
                }
            }
        }

        // Handle null assignments
        if (is_nullable && data_type_stored == AST_UNSPECIFIED) {
            // Assigning null to a nullable variable
            return;
        }

        // If none of the above conditions are met, it's a semantic error
        exit(SEMANTIC_ERROR_TYPE_COMPAT);
    } else {
        // Data types match, assignment is allowed
        return;
    }
}

DataType evaluate_fn_call_type(ASTNode *expression, SymbolTable *global_table, ScopeStack *local_stack) {

    semantic_analysis(expression, global_table, local_stack);
    Symbol *fn_symbol = lookup_symbol(global_table, expression->FnCall.fn_name);
    const char *fn_name = expression->FnCall.fn_name;;

    if (fn_symbol) {
        
        return fn_symbol->func.type;
    } else {
        

        return deduce_builtin_function_type(fn_name);   
    }

}

void populate_global_table_with_functions(ASTNode *root, SymbolTable *global_table) {
    for (int i = 0; i < root->Program.decl_count; i++) {
        ASTNode *decl = root->Program.declarations[i];
        if (decl->type == AST_FN_DECL) {
            const char *name = decl->FnDecl.fn_name;
            DataType function_type = decl->FnDecl.return_type;
            bool is_nullable = decl->FnDecl.nullable;

            
            // Check for redefinition
            Symbol *existing_symbol = lookup_symbol(global_table, name);
            if (existing_symbol && existing_symbol->type == SYMBOL_FUNC) {
                
                exit(SEMANTIC_ERROR_REDEF);
            }
            // Verify the addition

            // Add the function to the global table
            add_function_symbol(global_table, name, function_type, false, decl, is_nullable);
        }
    }
}

void semantic_analysis(ASTNode *node, SymbolTable *global_table, ScopeStack *local_stack) {
    

    if (!node) return;
    switch (node->type) {
        case AST_PROGRAM: {

            populate_global_table_with_functions(node, global_table);

            for (int i = 0; i < node->Program.decl_count; i++) {
                // "node->Program.declarations[i]" can take a shape of fn, var or const
                ASTNode *decl = node->Program.declarations[i];
                Symbol *symbol = lookup_symbol(global_table, decl->FnDecl.fn_name);

                if (!symbol->func.is_initialized) {
                    semantic_analysis(node->Program.declarations[i], global_table, local_stack);
                }
            }

            // Check for the presence of a valid 'main' function after processing all declarations
            check_main_function(global_table);

            SymbolTable *current_table = global_table;

            // Check for unused variables in the current frame
            unsigned int is_unused = unused_vars_funcs_frame(current_table);

            if (is_unused) {
                exit(is_unused);
            }

            break;
        }

        case AST_FN_DECL: {
            
            const char *fn_name = node->FnDecl.fn_name;
            bool func_is_nullable = node->FnDecl.nullable;
            DataType return_type = node->FnDecl.return_type;
            

            // Check if the function is already declared
            // Symbol *existing_symbol = lookup_symbol(global_table, fn_name);

            // if (existing_symbol != NULL) {
            //     
            //     exit(SEMANTIC_ERROR_REDEF);
            // }

            // Add the function symbol to the global symbol table
            // add_function_symbol(global_table, fn_name, return_type, is_nullable);

            // Retrieve the function symbol to access its scope stack
            Symbol *fn_symbol = lookup_symbol(global_table, fn_name);
            // if (fn_symbol == NULL || fn_symbol->type != SYMBOL_FUNC) {
            //     
            //     exit(SEMANTIC_ERROR_UNDEFINED);
            // }

            // // Initialize the scope stack for the function
            // fn_symbol->func.scope_stack = init_scope_stack();
            
            ScopeStack *function_stack = fn_symbol->func.scope_stack;

            // Push a frame for the function parameters
            push_frame(function_stack);

            Frame *base_frame = top_frame(function_stack);

            // Process function parameters
            for (int i = 0; i < node->FnDecl.param_count; i++) {
                ASTNode *param_node = node->FnDecl.params[i];
                const char *param_name = param_node->Param.identifier;
                DataType param_type = param_node->Param.data_type;

                // Ensure the parameter node is valid
                if (param_node->type != AST_PARAM) {
                    
                    exit(SEMANTIC_ERROR_PARAMS);
                }

                

                Symbol *existing_symbol = lookup_symbol_in_scope(function_stack, param_name, base_frame);

                if (existing_symbol) {
                    
                    exit(SEMANTIC_ERROR_REDEF);

                } else {
                    add_variable_symbol(function_stack->frames[function_stack->top]->symbol_table, param_name, param_type, true, param_node->Param.nullable, false, 0);
                }
            }

            

            if (func_is_nullable) {
                
                fn_symbol->func.is_nullable = true;
            }

            fn_symbol->func.is_initialized = true;

            // Push a new frame for the function body
            push_frame(function_stack);

            // Recursively analyze the function block
            if (node->FnDecl.block) {
                semantic_analysis(node->FnDecl.block, global_table, function_stack);
            }

            // Ensure non-void functions have a return statement
            if (return_type != AST_VOID && fn_symbol && fn_symbol->type == SYMBOL_FUNC && !fn_symbol->func.has_return) {
                if (!(strcmp(fn_symbol->func.name, "main") == 0)) {
                    
                    exit(SEMANTIC_ERROR_RETURN);
                }
            }

            // Cleanup the function's scope stack (pop all frames)
            pop_frame(function_stack);
            break;
        }
    

        case AST_PARAM: {
            // Retrieve parameter details
            const char *param_name = node->Param.identifier;
            DataType param_type = node->Param.data_type;
            // bool nullable = node->Param.nullable;

            Symbol *symbol = lookup_symbol(local_stack->frames[local_stack->top]->symbol_table, param_name);
            
            // Check if the parameter already exists in the current scope
            if (!symbol) {
                
                exit(SEMANTIC_ERROR_PARAMS);
            }

            // Add the parameter to the local symbol table
            add_variable_symbol(local_stack->frames[local_stack->top]->symbol_table, param_name, param_type, true, symbol->var.is_nullable, symbol->var.has_literal, symbol->var.value);            
            break;
        }


        case AST_BLOCK: {
            // Push a new frame to the local stack for the block
            push_frame(local_stack);

            
            Symbol *enclosing_function;

            // Iterate through the global symbol table to find the matching function
            for (int i = 0; i < global_table->capacity; i++) {
                Symbol *symbol = global_table->symbols[i];
                if (symbol && symbol->type == SYMBOL_FUNC && symbol->func.scope_stack == local_stack) {
                    enclosing_function = symbol;
                }
            }

            // Traverse all nodes within the block
            for (int i = 0; i < node->Block.node_count; i++) {
                ASTNode *child_node = node->Block.nodes[i];

                // Perform semantic analysis on each child node
                semantic_analysis(child_node, global_table, local_stack);

                

                // Check if the child node is a function call and its return value is discarded

                if (enclosing_function) {
                    
                    if (child_node->type == AST_FN_CALL) {
                        const char *fn_name = child_node->FnCall.fn_name;
                        bool is_builtin = false;
                        DataType return_type = AST_UNSPECIFIED;

                        // Check if it's a user-defined function
                        Symbol *fn_symbol = lookup_symbol(global_table, fn_name);

                        if (fn_symbol && fn_symbol->type == SYMBOL_FUNC) {
                            // User-defined function
                            return_type = fn_symbol->func.type;
                        } else {
                            // Check for built-in function
                            for (size_t j = 0; j < sizeof(built_in_functions) / sizeof(built_in_functions[0]); j++) {
                                if (strcmp(fn_name, built_in_functions[j].name) == 0) {
                                    is_builtin = true;
                                    return_type = built_in_functions[j].return_type;
                                    break;
                                }
                            }
                        }

                        // Error handling for undefined functions
                        if (!fn_symbol && !is_builtin) {
                            
                            exit(SEMANTIC_ERROR_UNDEFINED);
                        }

                        // Check for discarded non-void return type
                        if (return_type != AST_VOID) {
                            exit(SEMANTIC_ERROR_PARAMS);
                        }
                    } else if (child_node->type == AST_RETURN) {
                        
                        enclosing_function->func.has_return = true;
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
                        
                        exit(SEMANTIC_ERROR_UNUSED_VAR);
                    } else if (symbol && symbol->type == SYMBOL_VAR && !symbol->var.is_constant  && !symbol->var.redefined) {
                        
                        exit(SEMANTIC_ERROR_UNUSED_VAR);
                    }
                }
            }

            // Pop the frame after exiting the block
            
            pop_frame(local_stack);
            break;
        }


        case AST_WHILE:
        case AST_IF_ELSE: {
            

            // Push a frame for the block
            push_frame(local_stack);
            Frame *current_frame = top_frame(local_stack);

            // Handle condition expression differently for `while` and `if-else`
            ASTNode *condition_expression = (node->type == AST_WHILE) ? node->WhileCycle.expression : node->IfElse.expression;
            const char *bind_name = (node->type == AST_WHILE) ? node->WhileCycle.element_bind : node->IfElse.element_bind;

            // Evaluate the condition expression
            if (condition_expression) {

                DataType condition_type = evaluate_expression_type(condition_expression, global_table, local_stack, current_frame);

                if (bind_name) {
                    Symbol *symbol = NULL;
                    bool has_literal = false;

                    if (condition_expression->type == AST_IDENTIFIER) {
                        symbol = lookup_symbol_in_scope(local_stack, condition_expression->Identifier.identifier, current_frame);

                        has_literal = symbol->var.has_literal ? true : false;

                        

                        if (condition_type != AST_I32 && !symbol->var.is_nullable) { // Ensures the condition is a boolean-compatible type
                            
                            exit(SEMANTIC_ERROR_TYPE_COMPAT);
                        }
                    }

                    
                    process_binding(condition_expression, global_table, local_stack, current_frame, bind_name, condition_type, has_literal);

                } else {
                    // Perform semantic analysis on the condition if no binding
                    semantic_analysis(condition_expression, global_table, local_stack);
                }
            }

            // Analyze blocks
            if (node->type == AST_WHILE) {
                semantic_analysis(node->WhileCycle.block, global_table, local_stack);
                pop_frame(local_stack);

            } else {
                // Analyze "if" block
                semantic_analysis(node->IfElse.if_block, global_table, local_stack);
                pop_frame(local_stack);

                // Process the "else" block if it exists
                if (node->IfElse.else_block) {
                    push_frame(local_stack);
                    semantic_analysis(node->IfElse.else_block, global_table, local_stack);
                    pop_frame(local_stack);
                }
            }


            break;
        }


        case AST_BIN_OP: {
            // Determine the operator type
            OperatorType operator = node->BinaryOperator.operator;

            

            // Check if the operator is valid isRelationalOperator
            if (!isRelationalOperator(operator)) {
                
                exit(SEMANTIC_ERROR_TYPE_COMPAT);
            }

            
            break;
        }


        case AST_FN_CALL: {
            const char *fn_name = node->FnCall.fn_name;
            Frame *current_frame = top_frame(local_stack);

            

            // Check if the function is built-in
            bool is_builtin = false;
            BuiltInFunction *builtin_func = NULL;  
            for (size_t i = 0; i < sizeof(built_in_functions) / sizeof(built_in_functions[0]); i++) {
                if (strcmp(fn_name, built_in_functions[i].name) == 0) {
                    
                    is_builtin = true;
                    builtin_func = &built_in_functions[i];
                    break;
                }
            }

            if (is_builtin) {
                

                // Validate argument count for built-in functions
                if (builtin_func->param_count != -1 && node->FnCall.arg_count != builtin_func->param_count) {
                    exit(SEMANTIC_ERROR_PARAMS);
                }

                // Validate if arg in builtin function provided exists
                // Validate argument types for built-in functions
                for (int i = 0; i < node->FnCall.arg_count; i++) {

                    DataType arg_type = evaluate_expression_type(node->FnCall.args[i], global_table, local_stack, current_frame);

                    if (builtin_func->param_count != -1 && arg_type != builtin_func->expected_arg_types[i]) {
                        exit(SEMANTIC_ERROR_TYPE_COMPAT);
                    }
                }
            } else {
                // If not a built-in function, check user-defined functions
                Symbol *fn_symbol = lookup_symbol(global_table, fn_name);
                
                if (!fn_symbol || fn_symbol->type != SYMBOL_FUNC) {
                    
                    exit(SEMANTIC_ERROR_UNDEFINED);
                } 

                

                // Mark the function as used
                fn_symbol->func.used = true;

                if (!fn_symbol->func.fn_node) {
                    
                    exit(INTERNAL_ERROR);
                }

                

                if (!fn_symbol->func.is_initialized) {
                    
                    semantic_analysis(fn_symbol->func.fn_node, global_table, local_stack);
                    
                    fn_symbol->func.is_initialized = true;
                }

                // Check argument count for user-defined functions
                ScopeStack *fn_scope_stack = fn_symbol->func.scope_stack;

                if (!fn_scope_stack || fn_scope_stack->top < 0) {
                    
                    exit(INTERNAL_ERROR);
                }

                Frame *fn_frame_args = fn_scope_stack->frames[0]; // Function parameters are in the first frame
                
                if (!fn_frame_args || !fn_frame_args->symbol_table) {
                    
                    exit(INTERNAL_ERROR);
                }

                SymbolTable *param_table = fn_frame_args->symbol_table;

                int expected_arg_count = param_table->count;
                
                if (node->FnCall.arg_count != expected_arg_count) {
                    exit(SEMANTIC_ERROR_PARAMS);
                }

                for (int i = 0; i < node->FnCall.arg_count; i++) {
                    
                    ASTNode *arg = node->FnCall.args[i];
                    DataType arg_type = evaluate_expression_type(arg, global_table, local_stack, current_frame);
                    

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
                        
                        exit(INTERNAL_ERROR);
                    }

                    // Check type compatibility
                    if (param_symbol->var.type != arg_type) {
                        exit(SEMANTIC_ERROR_PARAMS);
                    }
                }


            }
            
            break;
        }

        case AST_ARG: {
            Frame *current_frame = top_frame(local_stack);

            if (!node->Argument.expression) {
                
                exit(SEMANTIC_ERROR_UNDEFINED);
            }

            // Evaluate the type of the expression
            DataType arg_type = evaluate_expression_type(node->Argument.expression, global_table, local_stack, current_frame);
            

            // Ensure the expression is valid (further checks can be added here if needed)
            if (arg_type == AST_UNSPECIFIED) {
                
                exit(SEMANTIC_ERROR_TYPE_COMPAT);
            }

            break;
        }

        case AST_RETURN: {
            // Ensure the function context is valid (local stack must not be NULL)
            if (local_stack == NULL || local_stack->top == -1) {
                
                exit(SEMANTIC_ERROR_RETURN);
            }

            // Retrieve the first frame in the local stack (function scope)
            Frame *current_frame = local_stack->frames[0];
            if (!current_frame || !current_frame->symbol_table) {
                
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

            if (!function_symbol) {
                
                exit(INTERNAL_ERROR);
            }

            DataType expected_return_type = function_symbol->func.type;
            


            if (expected_return_type != AST_VOID) {
                
                // We check if expression is null because if its not it does mean that there is error in return statement

                if (node->Return.expression) {
                    // Evaluate the type of the return expression

                    DataType return_type = evaluate_expression_type(node->Return.expression, global_table, local_stack, current_frame);
                    

                    // Check if the return type matches the expected return type and that main is of type void
                    if (return_type && return_type == AST_UNSPECIFIED && !(function_symbol->func.is_nullable)) {
                        exit(SEMANTIC_ERROR_RETURN);

                    } else if (return_type && return_type != expected_return_type && return_type != AST_UNSPECIFIED && expected_return_type != AST_UNSPECIFIED) {
                        exit(SEMANTIC_ERROR_PARAMS);
                    }

                } else if (!(function_symbol->func.is_nullable)) {
                    // When the statement is null, return type isn't void nor nullable
                    exit(SEMANTIC_ERROR_RETURN);
                }
                
            } else if (node->Return.expression) {
                // If return expression is not null and it is supposed to return ast void
                // it should be flagged as mismatch between return type and declaration type
                exit(SEMANTIC_ERROR_RETURN);

            }

            break;
        }

        case AST_VAR_DECL: {
            process_declaration(
                global_table,
                local_stack,
                node->VarDecl.var_name,
                node->VarDecl.data_type,
                node->VarDecl.expression,
                false, // is_constant
                node->VarDecl.nullable,
                node->VarDecl.expression->Float.number
            );
            break;
        }

        case AST_CONST_DECL: {
            process_declaration(
                global_table,
                local_stack,
                node->ConstDecl.const_name,
                node->ConstDecl.data_type,
                node->ConstDecl.expression,
                true, // is_constant
                node->ConstDecl.nullable,
                node->ConstDecl.expression->Float.number
            );
            break;
        }

        case AST_INT: {
            break;
        }

        case AST_FLOAT: {
            break;
        }

        case AST_STRING: {
            break;
        }

        case AST_NULL: {
            break;
        }

        case AST_IDENTIFIER: {
            Frame *current_frame = top_frame(local_stack);
            // Look up the identifier in the current scope stack or global table.
            Symbol *symbol = lookup_symbol_in_scope(local_stack, node->Identifier.identifier, current_frame);

            if (!symbol) {
                // If the identifier is not found, it's an undeclared variable.
                
                exit(SEMANTIC_ERROR_UNDEFINED);
            }

            
            
            // Mark the symbol as used.
            if (symbol->type == SYMBOL_VAR) {
                symbol->var.used = true;
            } else if (symbol->type == SYMBOL_FUNC) {
                // Ensure that identifiers aren't functions in invalid contexts.
                
                exit(SEMANTIC_ERROR_TYPE_COMPAT);
            }

            break;
        
        }

        case AST_ASSIGNMENT: {
            Frame *current_frame = top_frame(local_stack);
            const char *identifier = node->Assignment.identifier;
            DataType expression_type = 0;

            if (strcmp(identifier, "_") == 0) {
                expression_type = evaluate_expression_type(node->Assignment.expression, global_table, local_stack, current_frame);
                break;
            }

            // Lookup the identifier in the symbol table (local or global)
            Symbol *symbol = lookup_symbol_in_scope(local_stack, identifier, current_frame);
            
            if (!symbol) {
                // If the variable/constant is not declared, raise an error
                
                exit(SEMANTIC_ERROR_UNDEFINED);
            }

            // Check if the symbol is a constant
            if (symbol->type == SYMBOL_VAR) {
                // Constants cannot be modified
                if (symbol->var.is_constant) {
                    
                    exit(SEMANTIC_ERROR_REDEF);
                } else {
                    // If it's a variable, mark it as redefined
                    
                    symbol->var.redefined = true;
                    symbol->var.used = true;
                }
            }

            // Evaluate the expression type for the right side of the assignment
            expression_type = evaluate_expression_type(node->Assignment.expression, global_table, local_stack, current_frame);

            
            

            bool expression_is_literal = false;

            if (node->Assignment.expression->type == AST_INT || node->Assignment.expression->type == AST_FLOAT) {
                expression_is_literal = true;
            }

            check_type_compatibility(symbol->var.type, expression_type, symbol->var.is_nullable, symbol->var.value, expression_is_literal);



            break;
        }

        default: {
            
            exit(OTHER_SEMANTIC_ERROR);
        }
    }
}

bool process_expression_content(ASTNode *expression, DataType data_type_declared) {

    

    switch (expression->type) {
        case AST_FN_CALL: {
            const char *fn_name = expression->FnCall.fn_name;
            

            size_t built_in_count = sizeof(built_in_functions) / sizeof(built_in_functions[0]);
            for (size_t i = 0; i < built_in_count; i++) {
                if (strcmp(fn_name, built_in_functions[i].name) == 0) {
                    
                    return built_in_functions[i].is_nullable;
                }
            }
        }

        case AST_BIN_OP: {
            if (data_type_declared == AST_UNSPECIFIED) {
                return true;
            } else {
                return false;
            }
        }

        default: {
            return false;
        }

    }
}

void process_declaration(
    SymbolTable *global_table,
    ScopeStack *local_stack,
    const char *name,
    DataType data_type_declared,
    ASTNode *expression,
    bool is_constant,
    bool is_nullable,
    double value
) {
    
    
    

    if (strcmp(name, "_") == 0) {
        return;
    }

    // Determine the current frame
    Frame *current_frame = NULL;
    if (local_stack && local_stack->top >= 0) {
        current_frame = top_frame(local_stack);
    }

    // Check if already declared
    Symbol *existing_symbol = lookup_symbol_in_scope(local_stack, name, current_frame);
    if (existing_symbol) {
        exit(SEMANTIC_ERROR_REDEF);
    }

    // When the stored value is null and the variable isnt declared as nullable (able to store null) it throws SEMANTIC_ERROR_TYPE_DERIVATION error
    if (expression->type == AST_NULL && !is_nullable) {
        exit(SEMANTIC_ERROR_TYPE_DERIVATION);
    }

    if (!is_nullable) {
        
        
        is_nullable = process_expression_content(expression, data_type_declared);
    }

    bool has_literal = false;

    // || expression->type == AST_SLICE || expression->type NLLL ???

    if (expression->type == AST_INT || expression->type == AST_FLOAT) {
        has_literal = true;
    }

    // Deduce data type
    DataType data_type_stored = evaluate_expression_type(expression, global_table, local_stack, current_frame);
    

    // Data type wasnt specified in decleration
    if (data_type_declared == AST_UNSPECIFIED) {

        // When data type wasnt specified and variable isnt assigned null value, we can assign variable a data type of expression
        if (data_type_stored != AST_UNSPECIFIED) {
            data_type_declared = data_type_stored;
        }

    }

    // Perform type compatibility check
    check_type_compatibility(data_type_declared, data_type_stored, is_nullable, value, has_literal);

    // Add the variable/constant to the appropriate symbol table
    if (local_stack && local_stack->top >= 0) {
        if (current_frame && current_frame->symbol_table) {
            
            
            
            if (!(strcmp(name, "_") == 0)) {
                add_variable_symbol(current_frame->symbol_table, name, data_type_declared, is_constant, is_nullable, has_literal, value);
            }
        } else {
            exit(INTERNAL_ERROR);
        }
    }
}


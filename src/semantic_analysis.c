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

const char* get_node_type_name(ASTNodeType type) {
    switch (type) {
        case AST_PROGRAM: return "AST_PROGRAM";
        case AST_FN_DECL: return "AST_FN_DECL";
        case AST_PARAM: return "AST_PARAM";
        case AST_RETURN: return "AST_RETURN";
        case AST_BLOCK: return "AST_BLOCK";
        case AST_VAR_DECL: return "AST_VAR_DECL";
        case AST_CONST_DECL: return "AST_CONST_DECL";
        case AST_FN_CALL: return "AST_FN_CALL";
        case AST_ARG: return "AST_ARG";
        case AST_INT: return "AST_INT";
        case AST_FLOAT: return "AST_FLOAT";
        case AST_STRING: return "AST_STRING";
        case AST_IDENTIFIER: return "AST_IDENTIFIER";
        case AST_BIN_OP: return "AST_BIN_OP";
        case AST_ASSIGNMENT: return "AST_ASSIGNMENT";
        case AST_IF_ELSE: return "AST_IF_ELSE";
        case AST_WHILE: return "AST_WHILE";
        case AST_NULL: return "AST_NULL";
        default: return "UNKNOWN_TYPE";
    }
}

// Function to check if the operator is a valid relational operator
bool isRelationalOperator(OperatorType op) {
    printf("isRelationalOperator function...\n");
    printf("(isRelationalOperator) operator %u\n", op);

    static const OperatorType validOperators[] = {
        AST_GREATER, AST_GREATER_EQU, AST_LESS, AST_LESS_EQU, AST_EQU, AST_NOT_EQU
    };
    for (size_t i = 0; i < sizeof(validOperators) / sizeof(validOperators[0]); ++i) {
        if (op == validOperators[i]) {
            printf("(isRelationalOperator) found it\n");
            return true;
        }
    }
    return false;
}

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

unsigned int unused_vars_funcs_frame(SymbolTable *current_table) {

    for (int j = 0; j < current_table->capacity; j++) {
        Symbol *symbol = current_table->symbols[j];

        // We skip symbol which has not been assigned a value (null)
        if (!symbol) {
            continue;
        }

        // As stated in documentation, variable shall not stay unused, thereby we oversee its usage
        if (symbol && symbol->type == SYMBOL_VAR && !symbol->var.used) {
            fprintf(stderr, "Semantic Error: Variable '%s' declared in the global table was never used.\n", symbol->var.name);
            return SEMANTIC_ERROR_UNUSED_VAR;
        
        // Checking whether the function has been used within the flow of the program unless the function is main
        } else if (symbol && symbol->type == SYMBOL_FUNC && !symbol->func.used && !(strcmp(symbol->func.name, "main") == 0)) {
            fprintf(stderr, "Semantic Error: Function '%s' declared in the global table was never used.\n", symbol->func.name);
            return OTHER_SEMANTIC_ERROR;
        }
    }

    return 0;

}

void process_binding(ASTNode *expression, SymbolTable *global_table, ScopeStack *local_stack, Frame *current_frame, const char *bind_name, DataType condition_type) {
    OperatorType operator = expression->BinaryOperator.operator;

    if (!operator) {
        fprintf(stderr, "Semantic Error: Expression in binding cannot have a relational operator.\n");
        exit(SEMANTIC_ERROR_TYPE_COMPAT);
    }

    if (expression->type != AST_IDENTIFIER && expression->type != AST_FN_CALL) {
        fprintf(stderr, "Semantic Error: Identifier in binding must be AST_IDENTIFIER or AST_FN_CALL.\n");
        exit(SEMANTIC_ERROR_TYPE_COMPAT);
    }

    // Perform semantic analysis on the identifier
    semantic_analysis(expression, global_table, local_stack);

    // Look up the symbol in the scopes
    Symbol *identifier = lookup_symbol_in_scopes(global_table, local_stack, expression->Identifier.identifier, current_frame);

    if (!identifier->var.is_nullable) {
        fprintf(stderr, "Semantic Error: The variable in binding is not nullable.\n");
        exit(OTHER_SEMANTIC_ERROR);
    }

    add_variable_symbol(current_frame->symbol_table, bind_name, condition_type, true, 0);
}

DataType evaluate_condition(ASTNode *expression, SymbolTable *global_table, ScopeStack *local_stack, Frame *current_frame) {
    DataType condition_type = evaluate_expression_type(expression, global_table, local_stack, current_frame);

    if (condition_type != AST_I32) { // Ensures the condition is a boolean-compatible type
        fprintf(stderr, "Semantic Error: Condition must evaluate to int.\n");
        exit(SEMANTIC_ERROR_TYPE_COMPAT);
    }

    return condition_type;
}


void check_main_function(SymbolTable *global_table) {
    // Lookup the main() function in the global symbol table
    Symbol *main_symbol = lookup_symbol(global_table, "main");

    // Check if the main function exists
    if (main_symbol == NULL || main_symbol->type != SYMBOL_FUNC) {
        fprintf(stderr, "Semantic Error: Missing 'main' function declaration.\n");
        exit(SEMANTIC_ERROR_UNDEFINED);
    }

    if (main_symbol->func.type != AST_VOID) {
        fprintf(stderr, "Semantic Error: Main is of non-void type.\n");
        exit(OTHER_SEMANTIC_ERROR);
    }

    // Check if the main function has zero parameters
    ScopeStack *main_scope_stack = main_symbol->func.scope_stack;

    if (main_scope_stack == NULL || main_scope_stack->frames[0]->symbol_table->count > 0) {
        fprintf(stderr, "Semantic Error: 'main' function must have zero parameters.\n");
        exit(SEMANTIC_ERROR_PARAMS);
    }
}

bool evaluate_fractionless_float (SymbolTable *global_table, ScopeStack *local_stack, const char *name, Frame *local_frame) {
    Symbol *symbol = lookup_symbol_in_scopes(global_table, local_stack, name, local_frame);

    printf("symbol->var.value = AST_I32: %f\n", symbol->var.value);

    if (fabs(symbol->var.value - (int)symbol->var.value) < 1e-9) {
        printf("symbol->var.value = AST_I32\n");
        return true; // Flag to adjust the variable type to AST_I32
    }    

    printf("returned false :(\n");
    return false;
}

DataType evaluate_operator_type(ASTNode *node, SymbolTable *global_table, ScopeStack *local_stack, Frame *local_frame) {

    // Ensure the node is of type AST_BIN_OP
    if (node->type != AST_BIN_OP) {
        fprintf(stderr, "Internal Error: Node is not a binary operator.\n");
        exit(INTERNAL_ERROR);
    }

    // Evaluate the left and right operand types
    DataType left_type = evaluate_expression_type(node->BinaryOperator.left, global_table, local_stack, local_frame);
    DataType right_type = evaluate_expression_type(node->BinaryOperator.right, global_table, local_stack, local_frame);

    printf("left_type: %u, right_type: %u\n", left_type, right_type);

    // Determine the operator type
    OperatorType operator = node->BinaryOperator.operator;

    // Check type compatibility based on the operator
    switch (operator) {
        case AST_PLUS:
        case AST_MINUS:
        case AST_MUL:
        case AST_DIV:
            // Arithmetic operators

            if ((left_type == AST_I32 && right_type == AST_I32) || 
                (left_type == AST_F64 && right_type == AST_F64)) {
                return left_type;
            } 

            if (left_type == AST_I32 && right_type == AST_F64) {
                // If left arithmetic operator is literal, implicit conversion is allowed
                if (node->BinaryOperator.left->type == AST_INT) {
                    printf("implicit conversion of left op.\n");
                    return AST_F64;  // Explicit return here
                }
                fprintf(stderr, "Semantic Error: Incompatible types for arithmetic operation. Left: %d, Right: %d\n", left_type, right_type);
                exit(SEMANTIC_ERROR_TYPE_COMPAT);  // Handle incompatible cases
            }

            if (left_type == AST_F64 && right_type == AST_I32) {
                // If right arithmetic operator is literal, implicit conversion is allowed
                if (node->BinaryOperator.right->type == AST_INT) {
                    printf("implicit conversion of right op.\n");
                    return AST_F64;  // Explicit return here
                }
                fprintf(stderr, "Semantic Error: Incompatible types for arithmetic operation. Left: %d, Right: %d\n", left_type, right_type);
                exit(SEMANTIC_ERROR_TYPE_COMPAT);  // Handle incompatible cases
            }

            // If no cases matched, it's a semantic error
            fprintf(stderr, "Semantic Error: Incompatible types for arithmetic operation. Left: %d, Right: %d\n", left_type, right_type);
            exit(SEMANTIC_ERROR_TYPE_COMPAT);


        case AST_GREATER:
        case AST_GREATER_EQU:
        case AST_LESS:
        case AST_LESS_EQU:
        case AST_EQU:
        case AST_NOT_EQU:
            // Relational operators
            if ((left_type == AST_I32 && right_type == AST_I32) || (left_type == AST_F64 && right_type == AST_F64)) {
                // Relational operator returns a boolean result
                return AST_I32; // Representing boolean as an integer type
            } else if (!(left_type == AST_I32 && right_type == AST_F64) || (left_type == AST_F64 && right_type == AST_I32)) {
                printf("ererkbdbf\n");
                if (left_type == AST_F64) {

                    // left type is fractionless
                    bool fractionless_left = fabs(node->BinaryOperator.left->Float.number - (int)node->BinaryOperator.left->Float.number) < 1e-9;

                    printf("fractionless_left %d\n", fractionless_left);
                    if (fractionless_left)  {
                        return AST_I32;
                    }
                    
                } else if (right_type == AST_F64) {
                    // right type is fractionless

                    bool fractionless_right = fabs(node->BinaryOperator.right->Float.number - (int)node->BinaryOperator.right->Float.number) < 1e-9;
                    if (fractionless_right)  {
                        return AST_I32;
                    }
                } else if ((left_type == AST_UNSPECIFIED && right_type != AST_UNSPECIFIED) || (left_type != AST_UNSPECIFIED && right_type == AST_UNSPECIFIED)) {
                    
                    fprintf(stderr, "Semantic Error: Incompatible types for relational operation. Left: %d, Right: %d\n", left_type, right_type);
                    exit(SEMANTIC_ERROR_TYPE_COMPAT);
                }
                
            } else {
                fprintf(stderr, "Semantic Error: Incompatible types for relational operation. Left: %d, Right: %d\n", left_type, right_type);
                exit(SEMANTIC_ERROR_TYPE_COMPAT);
            }

        default:
            fprintf(stderr, "Semantic Error: Unsupported operator type '%d'.\n", operator);
            exit(SEMANTIC_ERROR_TYPE_COMPAT);
    }
}

DataType deduce_builtin_function_type(const char *fn_name) {
    size_t built_in_count = sizeof(built_in_functions) / sizeof(built_in_functions[0]);
    for (size_t i = 0; i < built_in_count; i++) {
        if (strcmp(fn_name, built_in_functions[i].name) == 0) {
            printf("is_builtin found\n");
            return built_in_functions[i].return_type;
        }
    }

    fprintf(stderr, "Semantic Error: Function '%s' is undefined.\n", fn_name);
    exit(SEMANTIC_ERROR_UNDEFINED);
}

DataType evaluate_expression_type(ASTNode *node, SymbolTable *global_table, ScopeStack *local_stack, Frame *local_frame) {

    printf("eval\n");
    printf("node->type: %u\n", node->type);

    switch (node->type) {
        case AST_INT: {
            return AST_I32;
        }
        case AST_FLOAT: {
            return AST_F64;
        }
        case AST_STRING: {
            return AST_SLICE;
        } case AST_IDENTIFIER: { 
            Symbol *symbol = lookup_symbol_in_scopes(global_table, local_stack, node->Identifier.identifier, local_frame);
            if (!symbol) {
                fprintf(stderr, "Semantic Error: Undefined symbol '%s'.\n", node->Identifier.identifier);
                exit(SEMANTIC_ERROR_UNDEFINED);
            }

            // when var used in fun. mark it as used 
            if (symbol->type == SYMBOL_VAR) {
                symbol->var.used = true;
            }

            return symbol->var.type;
        }
        case AST_BIN_OP: {
            printf("AST_BIN_OP\n");
            return evaluate_operator_type(node, global_table, local_stack, local_frame);

        } case AST_ARG: {
            if (!node->Argument.expression) {
                fprintf(stderr, "Semantic Error: Argument expression is missing.\n");
                exit(SEMANTIC_ERROR_UNDEFINED);
            }
            // Recurs. eval. expression
            return evaluate_expression_type(node->Argument.expression, global_table, local_stack, local_frame);

        } case AST_FN_CALL: {
            const char *fn_name = node->FnCall.fn_name;

            // Lookup the function symbol
            Symbol *fn_symbol = lookup_symbol_in_scopes(global_table, local_stack, fn_name, local_frame);

            if (!fn_symbol) {

                DataType built_in_fn_data_type = deduce_builtin_function_type(fn_name);
                return built_in_fn_data_type;

            } else {
                fn_symbol->func.used = true;

                // Return the function's return type
                printf("fn_return_type: %u\n", fn_symbol->func.type);
                return fn_symbol->func.type;
            }

        }
        case AST_NULL: {
            printf("AST_NULL_here\n");
            return AST_UNSPECIFIED;
        }
        default:
            fprintf(stderr, "Semantic Error: Unsupported expression type.\n");
            exit(SEMANTIC_ERROR_RETURN);
    }
}


void check_initialization(ASTNode *expression, bool is_nullable, const char *name, bool is_constant) {
    if (expression->type == AST_NULL && !is_nullable) {
        fprintf(stderr, "Semantic Error: %s '%s' must have an initializing expression.\n", 
                is_constant ? "Constant" : "Variable", name);
        exit(SEMANTIC_ERROR_TYPE_DERIVATION);
    }
}

void check_type_compatibility(DataType data_type_declared, DataType data_type_stored, bool is_nullable, double value) {
    if (data_type_declared != data_type_stored) {

        // Check whether it is able to execute implicit conversion
        bool is_fractionless = (data_type_declared == AST_I32 && data_type_stored == AST_F64) && (fabs(value - (int)value) < 1e-9);

        // Data types dont match, isnt fractionless for implicit conversion, isnt nullable and of data type: AST_UNSPECIFIED (null)
        if (!(is_nullable && data_type_stored == AST_UNSPECIFIED) && !is_fractionless) {
            fprintf(stderr, "Semantic Error: Cannot assign data of type: %u to the var with defined type: %u\n", 
                    data_type_stored, data_type_declared);
            exit(SEMANTIC_ERROR_TYPE_COMPAT);
        }
    }
}

DataType evaluate_fn_call_type(ASTNode *expression, SymbolTable *global_table, ScopeStack *local_stack) {

    semantic_analysis(expression, global_table, local_stack);
    Symbol *fn_symbol = lookup_symbol(global_table, expression->FnCall.fn_name);
    const char *fn_name = expression->FnCall.fn_name;;

    if (fn_symbol) {
        printf("fn_return_type: %u\n", fn_symbol->func.type);
        return fn_symbol->func.type;
    } else {
        printf("deduce_builtin_function_type\n");
        return deduce_builtin_function_type(fn_name);   
    }

}



DataType deduce_data_type(ASTNode *expression, SymbolTable *global_table, ScopeStack *local_stack, Frame *current_frame, DataType data_type_declared) {
    
    // We created parent function for this because we need to evaluate expression which are FN_CALLS further, any other way we navigate to standard expression evaluation
    if (data_type_declared == AST_UNSPECIFIED) {
        if (expression && expression->type == AST_FN_CALL) {
            return evaluate_fn_call_type(expression, global_table, local_stack);

        } else {

            // When undeclared data type we evaluate what has been stored in there and that will define its type
            return evaluate_expression_type(expression, global_table, local_stack, current_frame);
        }

    // Data type is specified
    } else {

        if (expression && expression->type == AST_FN_CALL) {
            return evaluate_fn_call_type(expression, global_table, local_stack);

        } else {
            if (expression->type != AST_BIN_OP && expression->type != AST_NULL) {
                semantic_analysis(expression, global_table, local_stack);
            }
            

            return evaluate_expression_type(expression, global_table, local_stack, current_frame);
        }
    }

    return data_type_declared;
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

            SymbolTable *current_table = global_table;

            // Check for unused variables in the current frame
            unsigned int is_unused = unused_vars_funcs_frame(current_table);

            if (is_unused) {
                exit(is_unused);
            }

            break;
        }

        case AST_FN_DECL: {
            printf("NEW FUNCTION -------------------------------\n");
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
                ASTNode *param_node = node->FnDecl.params[i];

                // Ensure the parameter node is valid
                if (param_node->type != AST_PARAM) {
                    fprintf(stderr, "Semantic Error: Invalid parameter in function '%s'.\n", fn_name);
                    exit(SEMANTIC_ERROR_PARAMS);
                }

                const char *param_name = param_node->Param.identifier;
                DataType param_type = param_node->Param.data_type;

                // Check for duplicate parameters
                Symbol *existing_param = lookup_symbol(function_stack->frames[function_stack->top]->symbol_table, param_name);

                if (existing_param != NULL) {
                    fprintf(stderr, "Semantic Error: Duplicate parameter name '%s' in function '%s'.\n", param_name, fn_name);
                    exit(SEMANTIC_ERROR_PARAMS);
                }

                // Add the parameter to the local scope
                add_variable_symbol(function_stack->frames[function_stack->top]->symbol_table, param_name, param_type, false, 0);
            }

            printf("done with params\n");

            if (node->FnDecl.nullable) {
                printf("IS NULLABLE!\n");
                fn_symbol->func.is_nullable = true;
            }

            // Push a new frame for the function body
            push_frame(function_stack);

            // Recursively analyze the function block
            if (node->FnDecl.block) {
                semantic_analysis(node->FnDecl.block, global_table, function_stack);
            }

            // Ensure non-void functions have a return statement
            if (return_type != AST_VOID && fn_symbol && fn_symbol->type == SYMBOL_FUNC && !fn_symbol->func.has_return) {
                if (!(strcmp(fn_symbol->func.name, "main") == 0)) {
                    fprintf(stderr, "Semantic Error: Function '%s' must have a return statement.\n", fn_name);
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
            
            // Check if the parameter already exists in the current scope
            if (lookup_symbol(local_stack->frames[local_stack->top]->symbol_table, param_name)) {
                fprintf(stderr, "Semantic Error: Duplicate parameter '%s' in the function scope.\n", param_name);
                exit(SEMANTIC_ERROR_PARAMS);
            }

            // Add the parameter to the local symbol table
            add_variable_symbol(local_stack->frames[local_stack->top]->symbol_table, param_name, param_type, false, 0);

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

            printf("AST_BLOCK!!!\n");
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

                printf("Processing node of type: %s\n", get_node_type_name(child_node->type));

                // Check if the child node is a function call and its return value is discarded

                if (enclosing_function) {
                    printf("passed ?\n");
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
                            fprintf(stderr, "Semantic Error: Function '%s' is undefined.\n", fn_name);
                            exit(SEMANTIC_ERROR_UNDEFINED);
                        }

                        // Check for discarded non-void return type
                        if (return_type != AST_VOID) {
                            fprintf(stderr, "Semantic Error: Function '%s' has a non-void return type, but its value is discarded.\n",
                                    fn_name);
                            exit(SEMANTIC_ERROR_PARAMS);
                        }
                    } else if (child_node->type == AST_RETURN) {
                        printf("flagged AST_RETURN as true !!\n");
                        enclosing_function->func.has_return = true;
                    }
                }
            }
            
            
            // Check for unused variables in the current frame
            Frame *current_frame = top_frame(local_stack);
            printf("in block\n");
            if (current_frame && current_frame->symbol_table) {
                SymbolTable *current_table = current_frame->symbol_table;

                for (int j = 0; j < current_table->capacity; j++) {
                    Symbol *symbol = current_table->symbols[j];
                    if (symbol && symbol->type == SYMBOL_VAR && !symbol->var.used) {
                        fprintf(stderr, "Semantic Error: Variable '%s' declared in the block was never used.\n", symbol->var.name);
                        exit(SEMANTIC_ERROR_UNUSED_VAR);
                    } else if (symbol && symbol->type == SYMBOL_VAR && !symbol->var.is_constant  && !symbol->var.redefined) {
                        fprintf(stderr, "Semantic Error: Variable '%s' declared in the block never reinitialized.\n", symbol->var.name);
                        exit(SEMANTIC_ERROR_UNUSED_VAR);
                    }
                }
            }

            // Pop the frame after exiting the block
            printf("popped frame\n");
            pop_frame(local_stack);
            break;
        }


        case AST_WHILE:
        case AST_IF_ELSE: {
            printf("Analyzing %s...\n", node->type == AST_WHILE ? "AST_WHILE" : "AST_IF_ELSE");

            // Push a frame for the block
            push_frame(local_stack);
            Frame *current_frame = top_frame(local_stack);

            // Handle condition expression differently for `while` and `if-else`
            ASTNode *condition_expression = (node->type == AST_WHILE) ? node->WhileCycle.expression : node->IfElse.expression;
            const char *bind_name = (node->type == AST_WHILE) ? node->WhileCycle.element_bind : node->IfElse.element_bind;

            // Evaluate the condition expression
            if (condition_expression) {
                DataType condition_type = evaluate_condition(condition_expression, global_table, local_stack, current_frame);

                if (bind_name) {
                    process_binding(condition_expression, global_table, local_stack, current_frame, bind_name, condition_type);
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

            printf("got to the AST_BIN_OP\n");

            // Check if the operator is valid isRelationalOperator
            if (!isRelationalOperator(operator)) {
                fprintf(stderr, "Semantic Error: Unsupported operator type '%d'.\n", operator);
                exit(SEMANTIC_ERROR_TYPE_COMPAT);
            }

            printf("tu som AST_BIN_OP\n");
            break;
        }


        case AST_FN_CALL: {
            const char *fn_name = node->FnCall.fn_name;
            Frame *current_frame = top_frame(local_stack);

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

                // Validate if arg in builtin function provided exists
                // Validate argument types for built-in functions
                for (int i = 0; i < node->FnCall.arg_count; i++) {
                    printf("tu pojebe tym padom\n");
                    // Frame *current_frame = local_stack->frames[3];
                    print_symbol_table(current_frame->symbol_table);
                    DataType arg_type = evaluate_expression_type(node->FnCall.args[i], global_table, local_stack, current_frame);

                    if (builtin_func->param_count != -1 && arg_type != builtin_func->expected_arg_types[i]) {
                        fprintf(stderr, "Semantic Error: Argument %d of built-in function '%s' has mismatched type. "
                                        "Expected '%d', but got '%d'.\n",
                                i + 1, fn_name, builtin_func->expected_arg_types[i], arg_type);
                        exit(SEMANTIC_ERROR_TYPE_COMPAT);
                    }
                }
            } else {
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
                    DataType arg_type = evaluate_expression_type(arg, global_table, local_stack, current_frame);
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
                        exit(SEMANTIC_ERROR_PARAMS);
                    }
                }


            }

            break;
        }

        case AST_ARG: {
            Frame *current_frame = top_frame(local_stack);

            if (!node->Argument.expression) {
                fprintf(stderr, "Semantic Error: Argument expression is missing.\n");
                exit(SEMANTIC_ERROR_UNDEFINED);
            }

            // Evaluate the type of the expression
            DataType arg_type = evaluate_expression_type(node->Argument.expression, global_table, local_stack, current_frame);
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

            if (!function_symbol) {
                fprintf(stderr, "Internal Error: Function context symbol not found for return statement.\n");
                exit(INTERNAL_ERROR);
            }

            DataType expected_return_type = function_symbol->func.type;
            printf("expected_return_type: %u\n", expected_return_type);


            if (expected_return_type != AST_VOID) {
                printf("ast_return!\n");
                // We check if expression is null because if its not it does mean that there is error in return statement

                if (node->Return.expression) {
                    // Evaluate the type of the return expression

                    DataType return_type = evaluate_expression_type(node->Return.expression, global_table, local_stack, current_frame);
                    printf("return_type_var %u\n", return_type);

                    // Check if the return type matches the expected return type and that main is of type void

                    if (return_type && return_type == AST_UNSPECIFIED && !(function_symbol->func.is_nullable)) {
                        fprintf(stderr, "Semantic Error: Mismatched return type in function '%s'. Expected '%d', got '%d'.\n",
                                function_symbol->func.name, expected_return_type, return_type);
                        exit(SEMANTIC_ERROR_RETURN);
                    } else if (return_type && return_type != expected_return_type && return_type != AST_UNSPECIFIED && expected_return_type != AST_UNSPECIFIED) {
                        printf("herewego\n");
                        fprintf(stderr, "Semantic Error: Mismatched return type in function '%s'. Expected '%d', got '%d'.\n",
                                function_symbol->func.name, expected_return_type, return_type);
                        exit(SEMANTIC_ERROR_TYPE_COMPAT);
                    }
                } else if (!(function_symbol->func.is_nullable)) {
                    // When the statement is null, return type isn't void nor nullable
                    printf("return is null !\n");
                    fprintf(stderr, "Semantic Error: Mismatched return type in function '%s'. Expected '%d'.\n",
                    function_symbol->func.name, expected_return_type);
                    exit(SEMANTIC_ERROR_RETURN);
                }
                
            } else if (node->Return.expression) {
                // If return expression is not null and it is supposed to return ast void
                // it should be flagged as mismatch between return type and declaration type

                fprintf(stderr, "Semantic Error: Mismatched return type in function '%s'. Expected '%d'.\n",
                function_symbol->func.name, expected_return_type);
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
                node->ConstDecl.expression->Float.number
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

        case AST_IDENTIFIER: {
            Frame *current_frame = top_frame(local_stack);
            // Look up the identifier in the current scope stack or global table.
            Symbol *symbol = lookup_symbol_in_scopes(global_table, local_stack, node->Identifier.identifier, current_frame);

            if (!symbol) {
                // If the identifier is not found, it's an undeclared variable.
                fprintf(stderr, "Semantic Error: Undeclared identifier '%s'.\n", node->Identifier.identifier);
                exit(SEMANTIC_ERROR_UNDEFINED);
            }

            printf("got here, not undeclared\n");
            
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
            Frame *current_frame = top_frame(local_stack);
            const char *identifier = node->Assignment.identifier;

            // Lookup the identifier in the symbol table (local or global)
            Symbol *symbol = lookup_symbol_in_scopes(global_table, local_stack, identifier, current_frame);

            if (!symbol) {
                // If the variable/constant is not declared, raise an error
                fprintf(stderr, "Semantic Error: Undeclared variable or constant '%s'.\n", identifier);
                exit(SEMANTIC_ERROR_UNDEFINED);
            }

            // Check if the symbol is a constant
            if (symbol->type == SYMBOL_VAR) {
                // Constants cannot be modified
                if (symbol->var.is_constant) {
                    fprintf(stderr, "Semantic Error: Cannot assign to constant '%s'.\n", identifier);
                    exit(SEMANTIC_ERROR_REDEF);
                } else {
                    // If it's a variable, mark it as redefined
                    printf("flagged used !\n");
                    symbol->var.redefined = true;
                    symbol->var.used = true;
                }
            }

            // Evaluate the expression type for the right-hand side of the assignment
            DataType expression_type = evaluate_expression_type(node->Assignment.expression, global_table, local_stack, current_frame);

            printf("expression type: %u, symbol->var.type: %u\n", expression_type, symbol->var.type);

            check_type_compatibility(symbol->var.type, expression_type, symbol->var.is_nullable, symbol->var.value);

            break;
        }

        default: {
            fprintf(stderr, "Error: Unsupported AST node type.\n");
            exit(OTHER_SEMANTIC_ERROR);
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
    printf("value_var: %f\n", value);
    printf("VAR DECL !!!\n");

    // Determine the current frame
    Frame *current_frame = NULL;
    if (local_stack && local_stack->top >= 0) {
        current_frame = top_frame(local_stack);
    }

    // Check if already declared
    Symbol *existing_symbol = lookup_symbol_in_scopes(global_table, local_stack, name, current_frame);
    if (existing_symbol) {
        fprintf(stderr, "Semantic Error: %s '%s' is already declared in the current scope.\n", 
                is_constant ? "Constant" : "Variable", name);
        exit(SEMANTIC_ERROR_REDEF);
    }

    // When the stored value is null and the variable isnt declared as nullable (able to store null) it throws SEMANTIC_ERROR_TYPE_DERIVATION error
    if (expression->type == AST_NULL && !is_nullable) {
        fprintf(stderr, "Semantic Error: %s '%s' must have an initializing expression.\n", 
                is_constant ? "Constant" : "Variable", name);
        exit(SEMANTIC_ERROR_TYPE_DERIVATION);
    }

    // Deduce data type
    DataType data_type_stored = deduce_data_type(expression, global_table, local_stack, current_frame, data_type_declared);
    printf("data_type_stored_var: %u\n", data_type_stored);

    // Data type wasnt specified in decleration
    if (data_type_declared == AST_UNSPECIFIED) {

        // When data type wasnt specified and variable isnt assigned null value, we can assign variable a data type of expression
        if (data_type_stored != AST_UNSPECIFIED) {
            data_type_declared = data_type_stored;
        }

    }

    // Perform type compatibility check
    check_type_compatibility(data_type_declared, data_type_stored, is_nullable, value);

    // Add the variable/constant to the appropriate symbol table
    if (local_stack && local_stack->top >= 0) {
        if (current_frame && current_frame->symbol_table) {
            printf("data_type_declared_here: %u\n", data_type_declared);
            add_variable_symbol(current_frame->symbol_table, name, data_type_declared, is_constant, value);
        } else {
            fprintf(stderr, "Internal Error: No valid scope to declare %s '%s'.\n",
                    is_constant ? "constant" : "variable", name);
            exit(INTERNAL_ERROR);
        }
    } else {
        // TODO: erase global scope
        // Global scope
        add_variable_symbol(global_table, name, data_type_declared, is_constant, value);
    }

    // Set the nullable flag
    Symbol *new_symbol = lookup_symbol_in_scopes(global_table, local_stack, name, current_frame);
    if (new_symbol) {
        new_symbol->var.is_nullable = is_nullable;
    }
}


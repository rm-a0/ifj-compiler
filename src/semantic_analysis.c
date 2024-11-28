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
    const char *name;               
    int param_count;                
    DataType expected_arg_types[3]; 
    DataType return_type;           
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

        
        if (!symbol) {
            continue;
        }

        
        if (symbol && symbol->type == SYMBOL_VAR && !symbol->var.used) {
            
            return SEMANTIC_ERROR_UNUSED_VAR;
        
        
        } else if (symbol && symbol->type == SYMBOL_FUNC && !symbol->func.used && !(strcmp(symbol->func.name, "main") == 0)) {
            
            return OTHER_SEMANTIC_ERROR;
        }
    }

    return 0;

}

void process_binding(ASTNode *expression, SymbolTable *global_table, ScopeStack *local_stack, Frame *current_frame, const char *bind_name, DataType condition_type) {
    OperatorType operator = expression->BinaryOperator.operator;

    if (!operator) {
        
        exit(SEMANTIC_ERROR_TYPE_COMPAT);
    }

    if (expression->type != AST_IDENTIFIER && expression->type != AST_FN_CALL) {
        
        exit(SEMANTIC_ERROR_TYPE_COMPAT);
    }

    
    semantic_analysis(expression, global_table, local_stack);

    
    Symbol *identifier = lookup_symbol_in_scopes(global_table, local_stack, expression->Identifier.identifier, current_frame);

    if (!identifier->var.is_nullable) {
        
        exit(OTHER_SEMANTIC_ERROR);
    }

    add_variable_symbol(current_frame->symbol_table, bind_name, condition_type, true, 0);
}

DataType evaluate_condition(ASTNode *expression, SymbolTable *global_table, ScopeStack *local_stack, Frame *current_frame) {
    DataType condition_type = evaluate_expression_type(expression, global_table, local_stack, current_frame);

    if (condition_type != AST_I32) { 
        
        exit(SEMANTIC_ERROR_TYPE_COMPAT);
    }

    return condition_type;
}


void check_main_function(SymbolTable *global_table) {
    
    Symbol *main_symbol = lookup_symbol(global_table, "main");

    
    if (main_symbol == NULL || main_symbol->type != SYMBOL_FUNC) {
        
        exit(SEMANTIC_ERROR_UNDEFINED);
    }

    if (main_symbol->func.type != AST_VOID) {
        
        exit(OTHER_SEMANTIC_ERROR);
    }

    
    ScopeStack *main_scope_stack = main_symbol->func.scope_stack;

    if (main_scope_stack == NULL || main_scope_stack->frames[0]->symbol_table->count > 0) {
        
        exit(SEMANTIC_ERROR_PARAMS);
    }
}

bool evaluate_fractionless_float (SymbolTable *global_table, ScopeStack *local_stack, const char *name, Frame *local_frame) {
    Symbol *symbol = lookup_symbol_in_scopes(global_table, local_stack, name, local_frame);

    

    if (fabs(symbol->var.value - (int)symbol->var.value) < 1e-9) {
        
        return true; 
    }    

    
    return false;
}

DataType evaluate_operator_type(ASTNode *node, SymbolTable *global_table, ScopeStack *local_stack, Frame *local_frame) {

    
    if (node->type != AST_BIN_OP) {
        
        exit(INTERNAL_ERROR);
    }

    
    DataType left_type = evaluate_expression_type(node->BinaryOperator.left, global_table, local_stack, local_frame);
    DataType right_type = evaluate_expression_type(node->BinaryOperator.right, global_table, local_stack, local_frame);

    

    
    OperatorType operator = node->BinaryOperator.operator;

    
    switch (operator) {
        case AST_PLUS:
        case AST_MINUS:
        case AST_MUL:
        case AST_DIV:
            

            if ((left_type == AST_I32 && right_type == AST_I32) || 
                (left_type == AST_F64 && right_type == AST_F64)) {
                return left_type;
            } 

            if (left_type == AST_I32 && right_type == AST_F64) {
                
                if (node->BinaryOperator.left->type == AST_INT) {
                    
                    return AST_F64;  
                }
                
                exit(SEMANTIC_ERROR_TYPE_COMPAT);  
            }

            if (left_type == AST_F64 && right_type == AST_I32) {
                
                if (node->BinaryOperator.right->type == AST_INT) {
                    
                    return AST_F64;  
                }
                
                exit(SEMANTIC_ERROR_TYPE_COMPAT);  
            }

            
            
            exit(SEMANTIC_ERROR_TYPE_COMPAT);


        case AST_GREATER:
        case AST_GREATER_EQU:
        case AST_LESS:
        case AST_LESS_EQU:
        case AST_EQU:
        case AST_NOT_EQU:
            
            if ((left_type == AST_I32 && right_type == AST_I32) || (left_type == AST_F64 && right_type == AST_F64)) {
                
                return AST_I32; 
            } else if (!(left_type == AST_I32 && right_type == AST_F64) || (left_type == AST_F64 && right_type == AST_I32)) {
                
                if (left_type == AST_F64) {

                    
                    bool fractionless_left = fabs(node->BinaryOperator.left->Float.number - (int)node->BinaryOperator.left->Float.number) < 1e-9;

                    
                    if (fractionless_left)  {
                        return AST_I32;
                    }
                    
                } else if (right_type == AST_F64) {
                    

                    bool fractionless_right = fabs(node->BinaryOperator.right->Float.number - (int)node->BinaryOperator.right->Float.number) < 1e-9;
                    if (fractionless_right)  {
                        return AST_I32;
                    }
                } else if ((left_type == AST_UNSPECIFIED && right_type != AST_UNSPECIFIED) || (left_type != AST_UNSPECIFIED && right_type == AST_UNSPECIFIED)) {
                    
                    
                    exit(SEMANTIC_ERROR_TYPE_COMPAT);
                }
                
            } else {
                
                exit(SEMANTIC_ERROR_TYPE_COMPAT);
            }

        default:
            
            exit(SEMANTIC_ERROR_TYPE_COMPAT);
    }
}

DataType evaluate_expression_type(ASTNode *node, SymbolTable *global_table, ScopeStack *local_stack, Frame *local_frame) {

    
    

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
                
                exit(SEMANTIC_ERROR_UNDEFINED);
            }

            
            if (symbol->type == SYMBOL_VAR) {
                symbol->var.used = true;
            }

            return symbol->var.type;
        }
        case AST_BIN_OP: {
            
            return evaluate_operator_type(node, global_table, local_stack, local_frame);

        } case AST_ARG: {
            if (!node->Argument.expression) {
                
                exit(SEMANTIC_ERROR_UNDEFINED);
            }
            
            return evaluate_expression_type(node->Argument.expression, global_table, local_stack, local_frame);

        } case AST_FN_CALL: {
            const char *fn_name = node->FnCall.fn_name;

            
            Symbol *fn_symbol = lookup_symbol_in_scopes(global_table, local_stack, fn_name, local_frame);

            if (!fn_symbol || fn_symbol->type != SYMBOL_FUNC) {
                
                exit(SEMANTIC_ERROR_UNDEFINED);
            }

            fn_symbol->func.used = true;

            
            return fn_symbol->func.type;
        }
        case AST_NULL: {
            return AST_UNSPECIFIED;
        }
        default:
            
            exit(SEMANTIC_ERROR_RETURN);
    }
}



void semantic_analysis(ASTNode *node, SymbolTable *global_table, ScopeStack *local_stack) {
    

    if (!node) return;
    switch (node->type) {
        case AST_PROGRAM: {

            for (int i = 0; i < node->Program.decl_count; i++) {
                
                semantic_analysis(node->Program.declarations[i], global_table, local_stack);
            }

            
            check_main_function(global_table);

            SymbolTable *current_table = global_table;

            
            unsigned int is_unused = unused_vars_funcs_frame(current_table);

            if (is_unused) {
                exit(is_unused);
            }

            break;
        }

        case AST_FN_DECL: {
            
            const char *fn_name = node->FnDecl.fn_name;
            DataType return_type = node->FnDecl.return_type;

            
            Symbol *existing_symbol = lookup_symbol(global_table, fn_name);

            if (existing_symbol != NULL) {
                
                exit(SEMANTIC_ERROR_REDEF);
            }

            
            add_function_symbol(global_table, fn_name, return_type);

            
            Symbol *fn_symbol = lookup_symbol(global_table, fn_name);
            if (fn_symbol == NULL || fn_symbol->type != SYMBOL_FUNC) {
                
                exit(SEMANTIC_ERROR_UNDEFINED);
            }

            
            if (!fn_symbol->func.scope_stack) {
                fn_symbol->func.scope_stack = init_scope_stack();
            }
            
            ScopeStack *function_stack = fn_symbol->func.scope_stack;

            
            push_frame(function_stack);

            
            for (int i = 0; i < node->FnDecl.param_count; i++) {
                ASTNode *param_node = node->FnDecl.params[i];
                
                if (param_node->type != AST_PARAM) {
                    
                    exit(SEMANTIC_ERROR_PARAMS);
                }

                const char *param_name = param_node->Param.identifier;
                DataType param_type = param_node->Param.data_type;

                
                Symbol *existing_param = lookup_symbol(function_stack->frames[function_stack->top]->symbol_table, param_name);

                if (existing_param != NULL) {
                    
                    exit(SEMANTIC_ERROR_PARAMS);
                }
                
                add_variable_symbol(function_stack->frames[function_stack->top]->symbol_table, param_name, param_type, false, 0);
            }

            

            if (node->FnDecl.nullable) {
                
                fn_symbol->func.is_nullable = true;
            }

            
            push_frame(function_stack);

            
            if (node->FnDecl.block) {
                semantic_analysis(node->FnDecl.block, global_table, function_stack);
            }

            
            if (return_type != AST_VOID && fn_symbol && fn_symbol->type == SYMBOL_FUNC && !fn_symbol->func.has_return) {
                if (!(strcmp(fn_symbol->func.name, "main") == 0)) {
                    
                    exit(SEMANTIC_ERROR_RETURN);
                }
            }

            

            
            pop_frame(function_stack);
            break;
        }
    

        case AST_PARAM: {
            
            const char *param_name = node->Param.identifier;
            DataType param_type = node->Param.data_type;
            
            
            
            if (lookup_symbol(local_stack->frames[local_stack->top]->symbol_table, param_name)) {
                
                exit(SEMANTIC_ERROR_PARAMS);
            }

            
            add_variable_symbol(local_stack->frames[local_stack->top]->symbol_table, param_name, param_type, false, 0);

            
            
            
            
            
            
            
            
            
            break;
        }


        case AST_BLOCK: {
            
            push_frame(local_stack);

            
            Symbol *enclosing_function = NULL;

            
            for (int i = 0; i < global_table->capacity; i++) {
                Symbol *symbol = global_table->symbols[i];
                if (symbol && symbol->type == SYMBOL_FUNC && symbol->func.scope_stack == local_stack) {
                    enclosing_function = symbol;
                    break;
                }
            }

            
            for (int i = 0; i < node->Block.node_count; i++) {
                ASTNode *child_node = node->Block.nodes[i];

                
                semantic_analysis(child_node, global_table, local_stack);

                // Check if an enclosing function was found
                if (enclosing_function) {
                    if (child_node->type == AST_FN_CALL) {
                        const char *fn_name = child_node->FnCall.fn_name;
                        bool is_builtin = false;
                        DataType return_type = AST_UNSPECIFIED;

                        // Check if it's a user-defined function
                        Symbol *fn_symbol = lookup_symbol(global_table, fn_name);

                        if (fn_symbol && fn_symbol->type == SYMBOL_FUNC) {
                            return_type = fn_symbol->func.type;
                        } else {
                            for (size_t j = 0; j < sizeof(built_in_functions) / sizeof(built_in_functions[0]); j++) {
                                if (strcmp(fn_name, built_in_functions[j].name) == 0) {
                                    is_builtin = true;
                                    return_type = built_in_functions[j].return_type;
                                    break;
                                }
                            }
                        }

                        if (!fn_symbol && !is_builtin) {
                            exit(SEMANTIC_ERROR_UNDEFINED);
                        }

                        if (return_type != AST_VOID) {
                            exit(SEMANTIC_ERROR_PARAMS);
                        }
                    } else if (child_node->type == AST_RETURN) {
                        enclosing_function->func.has_return = true;
                    }
                } else {
                    exit(INTERNAL_ERROR);
                }
            }
            
            
            
            Frame *current_frame = top_frame(local_stack);
            
            if (current_frame && current_frame->symbol_table) {
                SymbolTable *current_table = current_frame->symbol_table;

                for (int j = 0; j < current_table->capacity; j++) {
                    Symbol *symbol = current_table->symbols[j];
                    if (symbol && symbol->type == SYMBOL_VAR && !symbol->var.used) {
                        
                        exit(SEMANTIC_ERROR_UNUSED_VAR);
                    } else if (symbol && symbol->type == SYMBOL_VAR && !symbol->var.is_constant  && !symbol->var.redefined) {
                        
                        exit(SEMANTIC_ERROR_UNUSED_VAR);
                    } else if (symbol && symbol->type == SYMBOL_VAR && !symbol->var.is_constant  && !symbol->var.redefined) {
                        fprintf(stderr, "Semantic Error: Variable '%s' declared in the block never reinitialized.\n", symbol->var.name);
                        exit(SEMANTIC_ERROR_UNUSED_VAR);
                    }
                }
            }

            
            
            pop_frame(local_stack);
            break;
        }


        case AST_WHILE:
        case AST_IF_ELSE: {
            

            
            push_frame(local_stack);
            Frame *current_frame = top_frame(local_stack);

            
            ASTNode *condition_expression = (node->type == AST_WHILE) ? node->WhileCycle.expression : node->IfElse.expression;
            const char *bind_name = (node->type == AST_WHILE) ? node->WhileCycle.element_bind : node->IfElse.element_bind;

            
            if (condition_expression) {
                DataType condition_type = evaluate_condition(condition_expression, global_table, local_stack, current_frame);

                if (bind_name) {
                    process_binding(condition_expression, global_table, local_stack, current_frame, bind_name, condition_type);
                } else {
                    
                    semantic_analysis(condition_expression, global_table, local_stack);
                }
            }

            
            if (node->type == AST_WHILE) {
                semantic_analysis(node->WhileCycle.block, global_table, local_stack);
                pop_frame(local_stack);

            } else {
                
                semantic_analysis(node->IfElse.if_block, global_table, local_stack);
                pop_frame(local_stack);

                
                if (node->IfElse.else_block) {
                    push_frame(local_stack);
                    semantic_analysis(node->IfElse.else_block, global_table, local_stack);
                    pop_frame(local_stack);
                }
            }


            break;
        }


        case AST_BIN_OP: {
            
            OperatorType operator = node->BinaryOperator.operator;

            

            
            if (!isRelationalOperator(operator)) {
                
                exit(SEMANTIC_ERROR_TYPE_COMPAT);
            }

            
            break;
        }


        case AST_FN_CALL: {
            const char *fn_name = node->FnCall.fn_name;
            Frame *current_frame = top_frame(local_stack);

            

            
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
                

                
                if (builtin_func->param_count != -1 && node->FnCall.arg_count != builtin_func->param_count) {
                    
                    exit(SEMANTIC_ERROR_PARAMS);
                }

                
                
                for (int i = 0; i < node->FnCall.arg_count; i++) {
                    
                    
                    DataType arg_type = evaluate_expression_type(node->FnCall.args[i], global_table, local_stack, current_frame);

                    if (builtin_func->param_count != -1 && arg_type != builtin_func->expected_arg_types[i]) {
                        
                        exit(SEMANTIC_ERROR_TYPE_COMPAT);
                    }
                }
            } else {
                
                Symbol *fn_symbol = lookup_symbol(global_table, fn_name);
                if (!fn_symbol || fn_symbol->type != SYMBOL_FUNC) {
                    
                    exit(SEMANTIC_ERROR_UNDEFINED);
                }

                
                fn_symbol->func.used = true;

                
                ScopeStack *fn_scope_stack = fn_symbol->func.scope_stack;

                if (!fn_scope_stack || fn_scope_stack->top < 0) {
                    
                    exit(INTERNAL_ERROR);
                }

                Frame *fn_frame_args = fn_scope_stack->frames[0]; 
                
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
                    

                    
                    Symbol *param_symbol = NULL;
                    int param_index = 0; 

                    for (int j = param_index; j < param_table->capacity; j++) {
                        Symbol *current_symbol = param_table->symbols[j];

                        
                        if (!current_symbol || current_symbol->type != SYMBOL_VAR) {
                            continue;
                        }

                        
                        if (param_index == i) {
                            param_symbol = current_symbol;
                            break;
                        }

                        
                        param_index++;
                    }

                    
                    if (!param_symbol) {
                        
                        exit(INTERNAL_ERROR);
                    }

                    
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

            
            DataType arg_type = evaluate_expression_type(node->Argument.expression, global_table, local_stack, current_frame);
            

            
            if (arg_type == AST_UNSPECIFIED) {
                
                exit(SEMANTIC_ERROR_TYPE_COMPAT);
            }

            break;
        }

        case AST_RETURN: {
            
            if (local_stack == NULL || local_stack->top == -1) {
                
                exit(SEMANTIC_ERROR_RETURN);
            }

            
            Frame *current_frame = local_stack->frames[0];
            if (!current_frame || !current_frame->symbol_table) {
                
                exit(INTERNAL_ERROR);
            }

            
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
                
                

                if (node->Return.expression) {
                    

                    DataType return_type = evaluate_expression_type(node->Return.expression, global_table, local_stack, current_frame);
                    

                    

                    if (return_type && return_type == AST_UNSPECIFIED && !(function_symbol->func.is_nullable)) {
                        
                        exit(SEMANTIC_ERROR_RETURN);
                    } else if (return_type && return_type != expected_return_type && return_type != AST_UNSPECIFIED && expected_return_type != AST_UNSPECIFIED) {
                        
                        exit(SEMANTIC_ERROR_TYPE_COMPAT);
                    }
                } else if (!(function_symbol->func.is_nullable)) {
                    
                    
                    exit(SEMANTIC_ERROR_RETURN);
                }
                
            } else if (node->Return.expression) {
                
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
                false, 
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
                true, 
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
            
            Symbol *symbol = lookup_symbol_in_scopes(global_table, local_stack, node->Identifier.identifier, current_frame);

            if (!symbol) {
                
                
                exit(SEMANTIC_ERROR_UNDEFINED);
            }

            
            
            
            if (symbol->type == SYMBOL_VAR) {
                symbol->var.used = true;
            } else if (symbol->type == SYMBOL_FUNC) {
                
                
                exit(SEMANTIC_ERROR_TYPE_COMPAT);
            }

            break;
        
        }

        case AST_ASSIGNMENT: {
            Frame *current_frame = top_frame(local_stack);
            const char *identifier = node->Assignment.identifier;

            
            Symbol *symbol = lookup_symbol_in_scopes(global_table, local_stack, identifier, current_frame);

            if (!symbol) {
                
                
                exit(SEMANTIC_ERROR_UNDEFINED);
            }

            
            if (symbol->type == SYMBOL_VAR) {
                
                if (symbol->var.is_constant) {
                    
                    exit(SEMANTIC_ERROR_REDEF);
                } else {
                    
                    
                    symbol->var.redefined = true;
                    symbol->var.used = true;
                }
            }

            
            DataType expression_type = evaluate_expression_type(node->Assignment.expression, global_table, local_stack, current_frame);

            

            bool is_fractionless = false;
            
            if (symbol->var.type == AST_F64 && expression_type == AST_I32) {
                
                
                is_fractionless = evaluate_fractionless_float(global_table, local_stack, identifier, current_frame); 
            }

            
            if (symbol->var.type != expression_type) {
                
                if (!(is_fractionless)) {
                    
                    exit(SEMANTIC_ERROR_TYPE_COMPAT);
                }
            }

            break;
        }

        default: {
            
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
)  {

    

    

    
    Frame *current_frame = NULL;
    if (local_stack && local_stack->top >= 0) {
        current_frame = top_frame(local_stack);
    }

    
    Symbol *existing_symbol = lookup_symbol_in_scopes(NULL, local_stack, name, current_frame);
    if (existing_symbol) {
        
        exit(SEMANTIC_ERROR_REDEF);
    }

    
    if (expression->type == AST_NULL && !is_nullable) {
        
        exit(SEMANTIC_ERROR_TYPE_DERIVATION);
    }
    
    

    
    if (data_type_declared == AST_UNSPECIFIED) {
        if (expression && expression->type == AST_FN_CALL) {
            
            
            semantic_analysis(expression, global_table, local_stack);

            
            Symbol *fn_symbol = lookup_symbol(global_table, expression->FnCall.fn_name);
            if (fn_symbol && fn_symbol->type == SYMBOL_FUNC) {
                data_type_declared = fn_symbol->func.type;
            } else {
                
                bool is_builtin = false;
                size_t built_in_count = sizeof(built_in_functions) / sizeof(built_in_functions[0]);
                for (size_t i = 0; i < built_in_count; i++) {
                    if (strcmp(expression->FnCall.fn_name, built_in_functions[i].name) == 0) {
                        is_builtin = true;
                        
                        data_type_declared = built_in_functions[i].return_type;
                        break;
                    }
                }

                if (!is_builtin) {
                    
                    exit(SEMANTIC_ERROR_UNDEFINED);
                }
            }
        
        } else {
            data_type_declared = evaluate_expression_type(expression, global_table, local_stack, current_frame);
        }

        if (data_type_declared == AST_UNSPECIFIED) {
            
            exit(SEMANTIC_ERROR_TYPE_DERIVATION);
        }
    } else {
        
        
        if (expression && expression->type == AST_FN_CALL) {
            
            
            semantic_analysis(expression, global_table, local_stack);

            
            Symbol *fn_symbol = lookup_symbol(global_table, expression->FnCall.fn_name);
            if (fn_symbol && fn_symbol->type == SYMBOL_FUNC) {
                data_type_declared = fn_symbol->func.type;
            } else {
                
                bool is_builtin = false;
                size_t built_in_count = sizeof(built_in_functions) / sizeof(built_in_functions[0]);
                for (size_t i = 0; i < built_in_count; i++) {
                    if (strcmp(expression->FnCall.fn_name, built_in_functions[i].name) == 0) {
                        is_builtin = true;
                        
                        data_type_declared = built_in_functions[i].return_type;
                        break;
                    }
                }

                if (!is_builtin) {
                    
                    exit(SEMANTIC_ERROR_UNDEFINED);
                }
            }
        
        } else {
            DataType data_type_stored = evaluate_expression_type(expression, global_table, local_stack, current_frame);

            

            
            if (expression->type != AST_BIN_OP && expression->type != AST_NULL) {
                semantic_analysis(expression, global_table, local_stack);
            }
            
            
            
            if (data_type_declared != data_type_stored) {
                bool is_fractionless2 = data_type_declared == AST_I32 && data_type_stored == AST_F64;

                bool is_fractionless = (is_fractionless2) && (fabs(value - (int)value) < 1e-9);
                    
                

                
                if (!(is_nullable && data_type_stored == AST_UNSPECIFIED) && !is_fractionless) {
                    
                    exit(SEMANTIC_ERROR_TYPE_COMPAT);
                }
            }
        }
    }

    
    if (local_stack && local_stack->top >= 0) {
        Frame *current_frame = top_frame(local_stack);
        if (current_frame && current_frame->symbol_table) {
            add_variable_symbol(current_frame->symbol_table, name, data_type_declared, is_constant, value);
        } else {
            exit(INTERNAL_ERROR);
        }
    } else {
        
        add_variable_symbol(global_table, name, data_type_declared, is_constant, value);
    }

    Symbol *new_symbol = lookup_symbol_in_scopes(global_table, local_stack, name, current_frame);
    if (new_symbol) {
        new_symbol->var.is_nullable = is_nullable;
    }
}

/**
 * @file ast.h
 * @brief Implementation of function that handle nodes of AST
 * @authors Michal Repcik (xrepcim00)
*/
#define _POSIX_C_SOURCE 200809L // Used for strdup(), optimize later
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "error.h"
#include "token.h"

#define DEFAULT_PROGRAM_DECL_CNT    10  ///< Used for pre-allocating memory for declaration array inside program node
#define DEFAULT_FN_ARG_CNT          3   ///< Used for pre-allocating memory for argument array inside function declaration node
#define DEFAULT_FN_PARAM_CNT        3   ///< Used for pre-allocating memory for parameter array inside function call node
#define DEFAULT_BLOCK_NODE_CNT      5   ///< Used for pre-allocating memory for node array inside block

// For testing purposes

const char* token_type_to_string(int token_type) {
    switch (token_type) {
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_MULT: return "MULTIPLY";
        case TOKEN_DIV: return "DIVIDE";
        case TOKEN_EQU: return "EQUAL";
        case TOKEN_NOT_EQU: return "NOT_EQUAL";
        case TOKEN_LESS: return "LESS_THAN";
        case TOKEN_LESS_EQU: return "LESS_EQUAL";
        case TOKEN_GREATER: return "GREATER_THAN";
        case TOKEN_GREATER_EQU: return "GREATER_EQUAL";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_INTEGER: return "INTEGER";
        case TOKEN_FLOAT: return "FLOAT";
        case TOKEN_STRING: return "STRING";
        case TOKEN_L_PAREN: return "LEFT_PAREN";
        case TOKEN_R_PAREN: return "RIGHT_PAREN";
        case TOKEN_EOF: return "EOF";
        // Add other tokens as needed
        default: return "UNKNOWN_TOKEN";
    }
}


/**
 * @brief Converts ASTNodeType enum to its string representation.
 * @param type The ASTNodeType enum value.
 * @return String representation of the ASTNodeType.
 */
const char* ast_node_type_to_string(ASTNodeType type) {
    switch (type) {
        case AST_PROGRAM: return "Program";
        case AST_FN_DECL: return "Function Declaration";
        case AST_PARAM: return "Parameter";
        case AST_VAR_DECL: return "Variable Declaration";
        case AST_CONST_DECL: return "Constant Declaration";
        case AST_BLOCK: return "Block";
        case AST_FN_CALL: return "Function Call";
        case AST_ARG: return "Argument";
        case AST_WHILE: return "While Loop";
        case AST_IF_ELSE: return "If-Else Statement";
        case AST_BIN_OP: return "Binary Operator";
        case AST_INT: return "Integer Literal";
        case AST_FLOAT: return "Float Literal";
        case AST_STRING: return "String Literal";
        case AST_IDENTIFIER: return "Identifier";
        case AST_RETURN: return "Return Statement";
        case AST_EXPRESSION: return "Expression";
        default: return "Unknown AST Node Type";
    }
}

/**
 * @brief Converts OperatorType enum to its string representation.
 * @param op_type The OperatorType enum value.
 * @return String representation of the OperatorType.
 */
const char* operator_type_to_string(OperatorType op_type) {
    switch (op_type) {
        case AST_PLUS: return "PLUS (+)";
        case AST_MINUS: return "MINUS (-)";
        case AST_MUL: return "MULTIPLY (*)";
        case AST_DIV: return "DIVIDE (/)";
        case AST_GREATER: return "GREATER THAN (>)";
        case AST_GREATER_EQU: return "GREATER THAN OR EQUAL (>=)";
        case AST_LESS: return "LESS THAN (<)";
        case AST_LESS_EQU: return "LESS THAN OR EQUAL (<=)";
        case AST_EQU: return "EQUAL (=)";
        case AST_NOT_EQU: return "NOT EQUAL (!=)";
        default: return "Unknown Operator Type";
    }
}

void print_ast_node(ASTNode* node, int indent_level) {
    if (node == NULL) {
        for (int i = 0; i < indent_level; i++) printf("  ");
        printf("NULL ASTNode\n");
        return;
    }

    for (int i = 0; i < indent_level; i++) printf("  ");

    printf("ASTNode Type: %s\n", ast_node_type_to_string(node->type));

    switch (node->type) {
        case AST_IDENTIFIER:
            for (int i = 0; i < indent_level + 1; i++) printf("  ");
            printf("Identifier: %s\n", node->Identifier.identifier);
            break;

        case AST_INT:
            for (int i = 0; i < indent_level + 1; i++) printf("  ");
            printf("Integer Value: %d\n", node->Integer.number);
            break;

        case AST_FLOAT:
            for (int i = 0; i < indent_level + 1; i++) printf("  ");
            printf("Float Value: %f\n", node->Float.number);
            break;

        case AST_STRING:
            for (int i = 0; i < indent_level + 1; i++) printf("  ");
            printf("String Value: \"%s\"\n", node->String.string);
            break;

        case AST_BIN_OP:
            for (int i = 0; i < indent_level + 1; i++) printf("  ");
            printf("Operator: %s\n", operator_type_to_string(node->BinaryOperator.operator));
            for (int i = 0; i < indent_level + 1; i++) printf("  ");
            printf("Left Operand:\n");
            print_ast_node(node->BinaryOperator.left, indent_level + 2);
            for (int i = 0; i < indent_level + 1; i++) printf("  ");
            printf("Right Operand:\n");
            print_ast_node(node->BinaryOperator.right, indent_level + 2);
            break;

        // Implement other ASTNodeTypes as needed
        // Example for AST_RETURN
        case AST_RETURN:
            for (int i = 0; i < indent_level + 1; i++) printf("  ");
            printf("Return Expression:\n");
            print_ast_node(node->Return.expression, indent_level + 2);
            break;

        // Example for AST_IF_ELSE
        case AST_IF_ELSE:
            for (int i = 0; i < indent_level + 1; i++) printf("  ");
            printf("If Expression:\n");
            print_ast_node(node->IfElse.expression, indent_level + 2);
            for (int i = 0; i < indent_level + 1; i++) printf("  ");
            printf("If Block:\n");
            print_ast_node(node->IfElse.if_block, indent_level + 2);
            if (node->IfElse.else_block != NULL) {
                for (int i = 0; i < indent_level + 1; i++) printf("  ");
                printf("Else Block:\n");
                print_ast_node(node->IfElse.else_block, indent_level + 2);
            }
            break;

        // Handle other ASTNodeTypes similarly...

        default:
            for (int i = 0; i < indent_level + 1; i++) printf("  ");
            printf("Details not implemented for this ASTNodeType.\n");
            break;
    }
}


// Expression nodes

ASTNode* create_binary_op_node(int operator, ASTNode* left, ASTNode* right) {
    // Map the operator token to the OperatorType enum
    OperatorType op_type;
    switch (operator) {
        case TOKEN_PLUS:
            op_type = AST_PLUS;
            break;
        case TOKEN_MINUS:
            op_type = AST_MINUS;
            break;
        case TOKEN_MULT:
            op_type = AST_MUL;
            break;
        case TOKEN_DIV:
            op_type = AST_DIV;
            break;
        case TOKEN_LESS:
            op_type = AST_LESS;
            break;
        case TOKEN_LESS_EQU:
            op_type = AST_LESS_EQU;
            break;
        case TOKEN_GREATER:
            op_type = AST_GREATER;
            break;
        case TOKEN_GREATER_EQU:
            op_type = AST_GREATER_EQU;
            break;
        case TOKEN_EQU:
            op_type = AST_EQU;
            break;
        case TOKEN_NOT_EQU:
            op_type = AST_NOT_EQU;
            break;
        // Add other operators as needed
        default:
            // Invalid operator token
            set_error(SYNTAX_ERROR);
            return NULL;
    }

    // Allocate memory for the ASTNode
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) {
        set_error(INTERNAL_ERROR);
        return NULL;
    }

    // Set the node type
    node->type = AST_BIN_OP;

    // Initialize the BinaryOperator struct
    node->BinaryOperator.operator = op_type;
    node->BinaryOperator.left = left;
    node->BinaryOperator.right = right;

    return node;
}

ASTNode* create_identifier_node(char* identifier) {
    // Allocate memory for the ASTNode
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) {
        // Handle memory allocation failure
        set_error(INTERNAL_ERROR);
        return NULL;
    }

    // Set the node type
    node->type = AST_IDENTIFIER;

    // Initialize the Identifier struct
    node->Identifier.identifier = strdup(identifier);
    if (node->Identifier.identifier == NULL) {
        // Handle memory allocation failure
        set_error(INTERNAL_ERROR);
        free(node);
        return NULL;
    }

    return node;
}

ASTNode* create_i32_node(int value) {
    // Allocate memory for the ASTNode
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) {
        // Handle memory allocation failure
        set_error(INTERNAL_ERROR);
        return NULL;
    }

    // Set the node type
    node->type = AST_INT;

    // Set the integer value
    node->Integer.number = value;

    return node;
}

ASTNode* create_f64_node(double value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) {
        set_error(INTERNAL_ERROR);
        return NULL;
    }

    node->type = AST_FLOAT;
    node->Float.number = value;

    return node;
}

ASTNode* create_string_node(char* value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) {
        set_error(INTERNAL_ERROR);
        return NULL;
    }

    node->type = AST_STRING;
    node->String.string = strdup(value);
    if (node->String.string == NULL) {
        set_error(INTERNAL_ERROR);
        free(node);
        return NULL;
    }

    return node;
}


//

ASTNode* create_program_node() {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        set_error(INTERNAL_ERROR);
        fprintf(stderr, "Memory allocation for program node failed\n");
        return NULL;
    }

    node->type = AST_PROGRAM;
    node->Program.has_prolog = false;
    node->Program.decl_count = 0;

    // Allocate meory for default size (can be reallocated later)
    node->Program.decl_capacity = DEFAULT_PROGRAM_DECL_CNT;
    node->Program.declarations = malloc(DEFAULT_PROGRAM_DECL_CNT * sizeof(ASTNode*));
    if (node->Program.declarations == NULL) {
        free(node);
        set_error(INTERNAL_ERROR);
        fprintf(stderr, "Memory allocation for declaration array in program node failed\n");
        return NULL;
    }

    // Set all pointers to NULL
    for (int i = 0; i < DEFAULT_PROGRAM_DECL_CNT; i++) {
        node->Program.declarations[i] = NULL;
    }

    return node;
}

ASTNode* create_fn_decl_node(char* fn_name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        set_error(INTERNAL_ERROR);
        fprintf(stderr, "Memory allocation for function declaration node failed\n");
        return NULL;
    }

    node->type = AST_FN_DECL;
    node->FnDecl.fn_name = strdup(fn_name);
    if (node->FnDecl.fn_name == NULL) {
        set_error(INTERNAL_ERROR);
        free(node);
        fprintf(stderr, "Memory allocation for function name in function declaration node failed\n");
    }

    node->FnDecl.param_count = 0;

    // Allocate memory for default parameter count (can be re-allocated later)
    node->FnDecl.param_capacity = DEFAULT_FN_PARAM_CNT;
    node->FnDecl.params = malloc(DEFAULT_FN_PARAM_CNT * sizeof(ASTNode*));
    if (node->FnDecl.params == NULL) {
        set_error(INTERNAL_ERROR);
        free(node->FnDecl.fn_name);
        free(node);
        fprintf(stderr, "Memroy allocation for paramaeter array in function declaration node failed\n");
        return NULL;
    }

    // Set all pointers to NULL
    for (int i = 0; i < DEFAULT_FN_PARAM_CNT; i++) {
        node->FnDecl.params[i] = NULL;
    }

    node->FnDecl.block = NULL;
    node->FnDecl.return_type = AST_UNSPECIFIED; // Not specified when creating node

    return node;
}

ASTNode* create_param_node(DataType data_type, char* identifier) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        set_error(INTERNAL_ERROR);
        fprintf(stderr, "Memory allocation for parameter node failed\n");
        return NULL;
    }

    node->type = AST_PARAM;
    node->Param.data_type = data_type;
    node->Param.identifier = strdup(identifier);
    if (node->Param.identifier == NULL) {
        set_error(INTERNAL_ERROR);
        free(node);
        fprintf(stderr, "Memory allocation for identifier in parameter node failed\n");
        return NULL;
    }

    return node;
}

ASTNode* create_return_node() {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        set_error(INTERNAL_ERROR);
        fprintf(stderr, "Memory allocation for return node failed\n");
        return NULL;
    }
    
    node->type = AST_RETURN;
    node->Return.expression = NULL;
    return node;
}

ASTNode* create_var_decl_node(DataType data_type, char* var_name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        set_error(INTERNAL_ERROR);
        fprintf(stderr, "Memory allocation for variable declaration node failed\n");
        return NULL;
    }

    node->type = AST_VAR_DECL;
    node->VarDecl.data_type = data_type;
    node->VarDecl.expression = NULL;
    node->VarDecl.var_name = strdup(var_name);
    if (node->VarDecl.var_name == NULL) {
        set_error(INTERNAL_ERROR);
        free(node);
        fprintf(stderr, "Memory allocation for variable name in variable declaration node failed\n");
        return NULL;
    }

    return node;
}

ASTNode* create_const_decl_node(DataType data_type, char* const_name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        set_error(INTERNAL_ERROR);
        fprintf(stderr, "Memory allocation for const declaration node failed\n");
        return NULL;
    }

    node->type = AST_CONST_DECL;
    node->ConstDecl.data_type = data_type;
    node->ConstDecl.expression = NULL;
    node->ConstDecl.const_name = strdup(const_name);
    if (node->VarDecl.var_name == NULL) {
        set_error(INTERNAL_ERROR);
        free(node);
        fprintf(stderr, "Memory allocation for const name in constant declaration node failed\n");
        return NULL;
    }

    return node;
}

ASTNode* create_block_node() {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        set_error(INTERNAL_ERROR);
        fprintf(stderr, "Memory allocation for block node failed\n");
        return NULL;
    }

    node->type = AST_BLOCK;
    node->Block.node_count = 0;
    node->Block.node_capacity = DEFAULT_BLOCK_NODE_CNT;
    node->Block.nodes = malloc(DEFAULT_BLOCK_NODE_CNT * sizeof(ASTNode*));
    if (node->Block.nodes == NULL) {
        set_error(INTERNAL_ERROR);
        free(node);
        fprintf(stderr, "Memory allocation for pointer array in block node failed\n");
        return NULL;
    }

    // Set all pointers to NULL
    for (int i = 0; i < DEFAULT_BLOCK_NODE_CNT; i++) {
        node->Block.nodes[i] = NULL;
    }

    return node;
}

ASTNode* create_while_node() {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        set_error(INTERNAL_ERROR);
        fprintf(stderr, "Memory allocation for while node failed\n");
        return NULL;
    }

    node->type = AST_WHILE;
    node->WhileCycle.element_bind = NULL;     
    node->WhileCycle.expression = NULL;
    node->WhileCycle.block = NULL;

    return node;
}

ASTNode* create_if_node() {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        set_error(INTERNAL_ERROR);
        fprintf(stderr, "Memory allocation for if/else node failed\n");
        return NULL;
    }

    node->type = AST_IF_ELSE;
    node->IfElse.element_bind = NULL;
    node->IfElse.if_block = NULL;
    node->IfElse.else_block = NULL;
    node->IfElse.expression = NULL;

    return node;
}

ASTNode* create_fn_call_node(char* fn_name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        set_error(INTERNAL_ERROR);
        fprintf(stderr, "Memory allocation for function call node failed\n");
        return NULL;
    }

    node->type = AST_FN_CALL;
    node->FnCall.fn_name = strdup(fn_name);
    if (node->FnCall.fn_name == NULL) {
        set_error(INTERNAL_ERROR);
        free(node);
        fprintf(stderr, "Memory allocation for function name in function call node failed\n");
        return NULL;
    }

    node->FnCall.arg_count = 0;

    // Allocate memory for default arguemnt count (can be re-allocated later)
    node->FnCall.arg_capacity = DEFAULT_FN_ARG_CNT;
    node->FnCall.args = malloc(DEFAULT_FN_ARG_CNT * sizeof(ASTNode*));
    if (node->FnCall.args == NULL) {
        set_error(INTERNAL_ERROR);
        free(node);
        fprintf(stderr, "Memory allocation for pointer array in function call node failed\n");
        return NULL;
    }

    // Set all pointes to NULL
    for (int i = 0; i < DEFAULT_FN_ARG_CNT; i++) {
        node->FnCall.args[i] = NULL;
    }

    return node;
}

ASTNode* create_arg_node() {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        set_error(INTERNAL_ERROR);
        fprintf(stderr, "Memory allocation for argument node failed\n");
        return NULL;
    }

    node->type = AST_ARG;
    node->Argument.expression = NULL;

    return node;
}

void free_ast_node(ASTNode* node) {
    if (node == NULL) {
        return;
    }

    switch (node->type) {
        case AST_BIN_OP:
            // Recursively free left and right operands
            free_ast_node(node->BinaryOperator.left);
            free_ast_node(node->BinaryOperator.right);
            // Free the node itself
            break;
        case AST_IDENTIFIER:
            if (node->Identifier.identifier != NULL) {
                free(node->Identifier.identifier);
            }
            break;
        case AST_INT:
            // No additional memory to free for AST_INT nodes
            break;
        case AST_FLOAT:
            // No additional memory to free for AST_INT nodes
            break;
        case AST_STRING:
            if (node->String.string != NULL) {
                free(node->String.string);
            }
            break;
        case AST_PROGRAM:
            // Free all declarations recursively
            for (int i = 0; i < node->Program.decl_count; i++) {
                if (node->Program.declarations[i] != NULL) {
                    free_ast_node(node->Program.declarations[i]);
                }
            }
            free(node->Program.declarations); // Free array itself
            break;

        case AST_FN_DECL:
            free(node->FnDecl.fn_name); // Free name
            // Free all parameters recursively
            for (int i = 0; i < node->FnDecl.param_count; i++) {
                if (node->FnDecl.params[i] != NULL) {
                    free_ast_node(node->FnDecl.params[i]);
                }
            }
            free(node->FnDecl.params); // Free array itself
            // Free block node
            if (node->FnDecl.block != NULL) {
                free_ast_node(node->FnDecl.block);
            }
            break;

        case AST_PARAM:
            free(node->Param.identifier); // Free identifier
            break;

        case AST_RETURN:
            // Free exression node
            if (node->Return.expression != NULL) {
                free_ast_node(node->Return.expression);
            }
            break;

        case AST_VAR_DECL:
            free(node->VarDecl.var_name); // Free name
            // Free expression node
            if (node->VarDecl.expression != NULL) {
                free_ast_node(node->VarDecl.expression);
            }
            break;

        case AST_CONST_DECL:
            free(node->ConstDecl.const_name); // Free name
            // Free expression node
            if (node->ConstDecl.expression != NULL) {
                free_ast_node(node->ConstDecl.expression);
            }
            break;

        case AST_BLOCK:
            // Free all block nodes recursively
            for (int i = 0; i < node->Block.node_count; i++) {
                if (node->Block.nodes[i] != NULL) {
                    free_ast_node(node->Block.nodes[i]);
                }
            }
            free(node->Block.nodes); // Free array itself
            break;

        case AST_WHILE:
            // Free expression
            if (node->WhileCycle.expression != NULL) {
                free_ast_node(node->WhileCycle.expression);
            }
            // Free while block
            if (node->WhileCycle.block != NULL) {
                free_ast_node(node->WhileCycle.block);
            }
            // Free element bind
            if (node->WhileCycle.element_bind != NULL) {
                free(node->WhileCycle.element_bind);
            }
            break;

        case AST_IF_ELSE:
            // Free expression
            if (node->IfElse.expression != NULL) {
                free_ast_node(node->IfElse.expression);
            }
            // Free if block
            if (node->IfElse.if_block != NULL) {
                free_ast_node(node->IfElse.if_block);
            }
            // Free else block
            if (node->IfElse.else_block != NULL) {
                free_ast_node(node->IfElse.else_block);
            }
            // Free element bind
            if (node->IfElse.element_bind != NULL) {
                free(node->IfElse.element_bind);
            }
            break;

        case AST_FN_CALL:
            free(node->FnCall.fn_name); // Free name
            // Free all arguments recursively
            for (int i = 0; i < node->FnCall.arg_count; i++) {
                if (node->FnCall.args[i] != NULL) {
                    free_ast_node(node->FnCall.args[i]);
                }
            }
            free(node->FnCall.args); // Free array itself
            break;

        case AST_ARG:
            // Free expression
            if (node->Argument.expression != NULL) {
                free_ast_node(node->Argument.expression);
            }
            break;

        default:
            set_error(INTERNAL_ERROR);
            fprintf(stderr, "Unknown node type: %d\n", node->type);
            break;
    }

    free(node); // Free node itself
}

int append_decl_to_prog(ASTNode* program_node, ASTNode* decl_node) {
    if (program_node == NULL || decl_node == NULL) {
        set_error(INTERNAL_ERROR);
        return 1;
    }

    // If capacity is reached, double the size of an array
    if (program_node->Program.decl_count >= program_node->Program.decl_capacity) {
        program_node->Program.decl_capacity *= 2;
        ASTNode** new_decl = realloc(program_node->Program.declarations, program_node->Program.decl_capacity);
        if (new_decl == NULL) {
            set_error(INTERNAL_ERROR);
            fprintf(stderr, "Failed to reallocate memory for program node declarations\n");
            return 1;
        }

        program_node->Program.declarations = new_decl;

    }
    // Append node to declaration pointer array
    program_node->Program.declarations[program_node->Program.decl_count] = decl_node;
    program_node->Program.decl_count++;
    return 0;
}

int append_param_to_fn(ASTNode* fn_node, ASTNode* param_node) {
    if (fn_node == NULL || param_node == NULL) {
        set_error(INTERNAL_ERROR);
        return 1;
    }

    // If capacity is reached, double the size of an array
    if (fn_node->FnDecl.param_count >= fn_node->FnDecl.param_capacity) {
        fn_node->FnDecl.param_capacity *= 2;
        ASTNode** new_params = realloc(fn_node->FnDecl.params, fn_node->FnDecl.param_capacity);
        if (new_params == NULL) {
            set_error(INTERNAL_ERROR);
            fprintf(stderr, "Failed to reallocate memory for parameters in function declaration node\n");
            return 1;
        }

        fn_node->FnDecl.params = new_params;

    }
    // Append node to parameter pointer array
    fn_node->FnDecl.params[fn_node->FnDecl.param_count] = param_node;
    fn_node->FnDecl.param_count++;
    return 0;
}

int append_node_to_block(ASTNode* block, ASTNode* node) {
    if (node == NULL || block == NULL) {
        set_error(INTERNAL_ERROR);
        return 1;
    }

    if (block->Block.node_count >= block->Block.node_capacity) {
        block->Block.node_capacity *= 2;
        ASTNode** new_nodes = realloc(block->Block.nodes, block->Block.node_capacity);
        if (new_nodes == NULL) {
            set_error(INTERNAL_ERROR);
            fprintf(stderr, "Failed to reallocate memory for nodes array in block node\n");
            return 1;
        }

        block->Block.nodes = new_nodes;
    }
    // Append node to node pointer array
    block->Block.nodes[block->Block.node_count] = node;
    block->Block.node_count++;
    return 0;
}
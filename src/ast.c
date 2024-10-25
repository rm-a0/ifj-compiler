/**
 * @file ast.h
 * @brief Implementation of function that handle nodes of AST
 * @authors Michal Repcik (xrepcim00)
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"

#define DEFAULT_PROGRAM_DECL_CNT    10  ///< Used for pre-allocating memory for declaration array inside program node
#define DEFAULT_FN_ARG_CNT          3   ///< Used for pre-allocating memory for argument array inside function declaration node
#define DEFAULT_FN_PARAM_CNT        3   ///< Used for pre-allocating memory for parameter array inside function call node
#define DEFAULT_BLOCK_NODE_CNT      5   ///< Used for pre-allocating memory for node array inside block

ASTNode* create_program_node() {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (node == NULL) {
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
        fprintf(stderr, "Memory allocation for function declaration node failed\n");
        return NULL;
    }

    node->type = AST_FN_DECL;
    node->FnDecl.fn_name = strdup(fn_name);
    if (node->FnDecl.fn_name == NULL) {
        free(node);
        fprintf(stderr, "Memory allocation for function name in function declaration node failed\n");
    }

    node->FnDecl.param_count = 0;

    // Allocate memory for default parameter count (can be re-allocated later)
    node->FnDecl.param_capacity = DEFAULT_FN_PARAM_CNT;
    node->FnDecl.params = malloc(DEFAULT_FN_PARAM_CNT * sizeof(ASTNode*));
    if (node->FnDecl.params == NULL) {
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
        fprintf(stderr, "Memory allocation for parameter node failed\n");
        return NULL;
    }

    node->type = AST_PARAM;
    node->Param.data_type = data_type;
    node->Param.identifier = strdup(identifier);
    if (node->Param.identifier == NULL) {
        free(node);
        fprintf(stderr, "Memory allocation for identifier in parameter node failed\n");
        return NULL;
    }

    return node;
}

ASTNode* create_return_node() {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (node == NULL) {
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
        fprintf(stderr, "Memory allocation for variable declaration node failed\n");
        return NULL;
    }

    node->type = AST_VAR_DECL;
    node->VarDecl.data_type = data_type;
    node->VarDecl.expression = NULL;
    node->VarDecl.var_name = strdup(var_name);
    if (node->VarDecl.var_name == NULL) {
        free(node);
        fprintf(stderr, "Memory allocation for variable name in variable declaration node failed\n");
        return NULL;
    }

    return node;
}

ASTNode* create_const_decl_node(DataType data_type, char* const_name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        fprintf(stderr, "Memory allocation for const declaration node failed\n");
        return NULL;
    }

    node->type = AST_CONST_DECL;
    node->ConstDecl.data_type = data_type;
    node->ConstDecl.expression = NULL;
    node->ConstDecl.const_name = strdup(const_name);
    if (node->VarDecl.var_name == NULL) {
        free(node);
        fprintf(stderr, "Memory allocation for const name in constant declaration node failed\n");
        return NULL;
    }

    return node;
}

ASTNode* create_block_node() {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        fprintf(stderr, "Memory allocation for block node failed\n");
        return NULL;
    }

    node->type = AST_BLOCK;
    node->Block.node_count = 0;
    node->Block.node_capacity = DEFAULT_BLOCK_NODE_CNT;
    node->Block.nodes = malloc(DEFAULT_BLOCK_NODE_CNT * sizeof(ASTNode*));
    if (node->Block.nodes == NULL) {
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
        fprintf(stderr, "Memory allocation for function call node failed\n");
        return NULL;
    }

    node->type = AST_FN_CALL;
    node->FnCall.fn_name = strdup(fn_name);
    if (node->FnCall.fn_name == NULL) {
        free(node);
        fprintf(stderr, "Memory allocation for function name in function call node failed\n");
        return NULL;
    }

    node->FnCall.arg_count = 0;

    // Allocate memory for default arguemnt count (can be re-allocated later)
    node->FnCall.arg_capacity = DEFAULT_FN_ARG_CNT;
    node->FnCall.args = malloc(DEFAULT_FN_ARG_CNT * sizeof(ASTNode*));
    if (node->FnCall.args == NULL) {
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
                free_ast_node(node->WhileCycle.element_bind);
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
                free_ast_node(node->IfElse.element_bind);
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
            fprintf(stderr, "Unknown node type: %d\n", node->type);
            break;
    }

    free(node); // Free node itself
}

int append_decl_to_prog(ASTNode* program_node, ASTNode* decl_node) {
    if (program_node == NULL || decl_node == NULL) {
        return 1;
    }

    // If capacity is reached, double the size of an array
    if (program_node->Program.decl_count >= program_node->Program.decl_capacity) {
        program_node->Program.decl_capacity *= 2;
        ASTNode** new_decl = realloc(program_node->Program.declarations, program_node->Program.decl_capacity);
        if (new_decl == NULL) {
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

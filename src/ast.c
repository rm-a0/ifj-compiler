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

    node->FnDecl.block = NULL;
    node->FnDecl.return_type = AST_VOID; // Void as deault data type

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
        fprintf(stderr, "Memory allocation for parameter node failed\n");
        return NULL;
    }

    node->Return.expression = NULL;
    return node;
}
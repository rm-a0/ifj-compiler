/**
 * @file parser.h
 * @brief Header file for parser.c
 * @authors Michal Repcik (xrepcim00)
*/

#ifndef AST_H
#define AST_H

#include <stdbool.h>

typedef enum {
    AST_PROGRAM,
    AST_FN_DECL,
    AST_VAR_DECL,
    AST_CONST_DECL
} ASTNodeType;

typedef enum {
    DT_U8,
    DT_I32,
    DT_F64,
    DT_VAR,
    DT_VOID 
} DataType;

/**
 * @struct ASTNode
 * @brief Representation of a node in Abstract Syntax Tree
 * 
 * @param type Type of node
 * @union Encapsulates different types of substructures for nodes (declarations, statements, expression)
*/
typedef struct {
    ASTNodeType type;
    union {
        struct {
            bool has_prolog;        ///< Flag for prolog (@import)
            ASTNode** declarations; ///< Top level declaration (fn, var, const)
        } Program;                  ///< Program node (root of AST)
 
        struct {
            char* fn_name;          ///< Function name
            int param_count;        ///< Number of parameters
            ASTNode** params;       ///< Array of pointers to function parameters
            ASTNode* block;         ///< Pointer to node encapsulating content of the function
            DataType return_type;   ///< Return type
        } FnDecl;

        struct {
            char* var_name;         ///< Variable name
            DataType data_type;     ///< Data type (optional)
            ASTNode* expression;    ///< TODO
        } VarDecl;

        struct {
            char* const_name;       ///< Constant name
            DataType data_type;     ///< Data type (optional)
            ASTNode* expression;    ///< TODO
        } ConstDecl;

        struct {
            ASTNode** nodes;        ///< Array of pointer to nodes (delcaration or statements)
        } Block;

        struct {
            ASTNode* expression;    ///< Expression controlling cycle
            char* element_bind;     ///< Ellement bind (optional)
            ASTNode* block;         ///< Pointer to a node encapsulating while cycle
        } WhileCycle;

        struct {
            ASTNode* expression;    ///< Expression controlling cycle
            char* element_bind;     ///< Ellement bind (optional)
            ASTNode* if_block;      ///< Pointer to a node encapsulating if block
            ASTNode* else_block;    ///< Pointer to a node encapsulating else block
        } IfElse;
    };
} ASTNode;

/*
 * TODO
 * Create function for allocating and freeing memory for ASTNode
 * Define grammar for LL
 * Create parsing function that parses tokens
 * Optional: Create different types of ASTNodes based on LL grammar
 * Finish parser that builds the tree
*/

#endif // AST_H
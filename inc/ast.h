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
    AST_CONST_DECL,
    AST_BLOCK,
    AST_FN_CALL,
    AST_PARAM,
    AST_ARG,
    AST_WHILE,
    AST_IF_ELSE,
    AST_BIN_OP
} ASTNodeType;

typedef enum {
    AST_U8,
    AST_I32,
    AST_F64,
    AST_VAR,
    AST_VOID 
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
            int decl_count;         ///< Number of top level declarations
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
            DataType data_type;     ///< Expected data type
            char* identifier;       ///< Name of varaible used in function
        } Param;

        struct {
            char* var_name;         ///< Variable name
            DataType data_type;     ///< Data type (optional)
            ASTNode* expression;    ///< Expression/identifier/number
        } VarDecl;

        struct {
            char* const_name;       ///< Constant name
            DataType data_type;     ///< Data type (optional)
            ASTNode* expression;    ///< Expression/identifier/number
        } ConstDecl;

        struct {
            int node_count;         ///< Number of nodes;
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

        struct {
            char* fn_name;          ///< Function name
            int arg_count;          ///< Number of arguments
            ASTNode** args;         ///< Array of pointers to function arguments
        } FnCall;

        struct {
            ASTNode* expression;    ///< Expression/identifier/number used as function argument
        } Argument;

        struct {
            ASTNode* expression;    ///< Expression/identifier/number that is returned
        } Return;

        struct {
            ASTNode* left;          ///< Left child node (can be expression/id/num)
            ASTNode* right;         ///< Right child node (can be expression/id/num)
        } BinaryOperator;

        struct {
            float number;           ///< Node for float (leaf)
        } Float;

        struct {
            int number;             ///< Node for integer (leaf)
        } Integer;

        struct {
            char* string;           ///< String node
        } String;

        struct {
            char* identifier;       ///< Identifier node (used in binary expressions)
        } Identifier;
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
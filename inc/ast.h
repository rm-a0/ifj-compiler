/**
 * @file ast.h
 * @brief Header file for ast.c
 * @authors Michal Repcik (xrepcim00)
*/

#ifndef AST_H
#define AST_H

#include <stdbool.h>

/**
 * @enum ASTNodeType
 * @brief Enumeration for different AST node types
*/
typedef enum {
    AST_PROGRAM,        ///< Root program node
    AST_FN_DECL,        ///< Node for function declarations
    AST_PARAM,          ///< Node for function parameter
    AST_VAR_DECL,       ///< Node for variable declaration
    AST_CONST_DECL,     ///< Node for constant declaration
    AST_BLOCK,          ///< Node for block
    AST_FN_CALL,        ///< Node for function call
    AST_ARG,            ///< Node for function arguments
    AST_WHILE,          ///< Node for while cycle
    AST_IF_ELSE,        ///< Node for if and else statements
    AST_BIN_OP,         ///< Node for binary operator
    AST_INT,            ///< Node for integer
    AST_FLOAT,          ///< Node for float
    AST_STRING,         ///< Node for stirg
    AST_IDENTIFIER,     ///< Node for identifier
    AST_RETURN          ///< Node for return statement
} ASTNodeType;

/**
 * @enum DataType
 * @brief Enumeration for different data types
*/
typedef enum {
    AST_U8,             ///< 'u8' (unsigend 8 bit int)
    AST_I32,            ///< 'i32' (32 bit integer)
    AST_F64,            ///< 'f64' (64 bit float)
    AST_VOID            ///< 'void' (void data type)
} DataType;

/**
 * @enum Operator type
 * @brief Enumeration for different operator types
*/
typedef enum {
    AST_PLUS,           ///< '+' operator
    AST_MINUS,          ///< '-' operator
    AST_MUL,            ///< '*' operator
    AST_DIV,            ///< '/' operator
    AST_GREATER,        ///< '>' opeartor
    AST_GREATER_EQU,    ///< '>= opeartor
    AST_LESS,           ///< '<' operator
    AST_LESS_EQU,       ///< '<=' operator
    AST_EQU,            ///< '=' operator
    AST_NOT_EQU         ///< '!=' operator
} OperatorType;

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
            int decl_capacity;      ///< Number of allocated pointers to declaration nodes
            int decl_count;         ///< Number of top level declarations
            ASTNode** declarations; ///< Top level declaration (fn, var, const)
        } Program;                  ///< Program node (root of AST)
 
        struct {
            char* fn_name;          ///< Function name
            int param_count;        ///< Number of parameters
            int param_capacity;     ///< Number of allocated pointers to parameter nodes
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
            int node_capacity;      ///< Number of allocated pointers to nodes
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
            int arg_capacity;       ///< Number of allocated pointers to function arguments
            ASTNode** args;         ///< Array of pointers to function arguments
        } FnCall;

        struct {
            ASTNode* expression;    ///< Expression/identifier/number used as function argument
        } Argument;

        struct {
            ASTNode* expression;    ///< Expression/identifier/number that is returned
        } Return;

        struct {
            OperatorType operator;  ///< Type of operator
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

        struct {
            ASTNode* binary_operator;
            // Technically just a packer node for binary operator
            // Not necessary
        } Expression;
    };
} ASTNode;

ASTNode* create_program_node();

ASTNode* create_fn_decl_node();

ASTNode* create_param_node();

ASTNode* create_return_node();

ASTNode* create_var_decl_node();

ASTNode* create_const_decl_node();

ASTNode* create_block_node();

ASTNode* create_while_node();

ASTNode* create_if_node();

ASTNode* create_fn_call_node();

ASTNode* create_arg_node();

/*
 * TODO
 * Create function for allocating and freeing memory for ASTNode
 * Define grammar for LL
 * Create parsing function that parses tokens
 * Optional: Create different types of ASTNodes based on LL grammar
 * Finish parser that builds the tree
*/

#endif // AST_H
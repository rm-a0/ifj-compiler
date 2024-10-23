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
    AST_UNSPECIFIED,    ///< If data type was not specified  
    AST_VOID,           ///< 'void' (void data type)
    AST_U8,             ///< 'u8' (unsigend 8 bit int)
    AST_SLICE,          ///< 'u8' (unsigend 8 bit int)
    AST_I32,            ///< 'i32' (32 bit integer)
    AST_F64             ///< 'f64' (64 bit float)
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
typedef struct ASTNode ASTNode;     ///< Forward declaration to ensure ASTNode is defined before its usage
struct ASTNode {
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
};

/**
 * @fn ASTNode* create_program_node()
 * @brief Function that creates program (root) node for AST
 *  
 * Node type is set to AST_PROGRAM, Program structs values are set to default values
 * (has prolog = false, declaration count = 0) and array of declaration pointers is 
 * pre-allocated for DEFAULT_PROGRAM_DECL_CNT nodes. Decalaration capacity is set 
 * to the same value and all pointers to declaration nodes are set to NULL.
 *  
 * @return Returns pointer to ASTNode or null if memory allocation failed
*/
ASTNode* create_program_node();

/**
 * @fn ASTNode* create_fn_decl_node(char* fn_name)
 * @brief Function that creates function declaration node
 * 
 * Node type is set to AST_FN_DECL, Function declaration struct values are set to default
 * (return type = unspecified, block = NULL) and array of parameter pointers is pre-allocated for
 * DEFAULT_FN_PARAM_CNT nodes. Parameter capacity is set to the same value and all
 * pointers to parameter nodes are set to NULL.
 * Memory is allocated for fn_name using strdup (must be freed later).
 * 
 * @param[in] fn_name Name of the function (identifier)
 * @return Returns pointer to ASTNode or null if memory allocation failed
*/
ASTNode* create_fn_decl_node(char* fn_name);

/**
 * @fn ASTNode* create_param_node(DataType data_type, char* identifier)
 * @brief Function that creates parameter node
 * 
 * Node type is set to AST_PARAM, Parameter struct values are set 
 * based on the arguments (data_type, identifier). Memory is allocated
 * for idnetifier using strdup (must be freed later).
 * 
 * @param[in] data_type Data type of the identifier
 * @param[in] identifier Name of the variable (identifier)
 * @return Returns pointer to ASTNode or null if memory allocation failed
*/
ASTNode* create_param_node(DataType data_type, char* identifier);

/**
 * @fn ASTNode* create_return_node()
 * @brief Function that creates return node
 * 
 * Node type is set to AST_RETURN, Return expression is set to default (NULL). 
 * 
 * @return Returns pointer to ASTNode or null if memory allocation failed
*/
ASTNode* create_return_node();

/**
 * @fn ASTNode* create_var_decl_node(DataType data_type, char* var_name)
 * @brief Function that creates variable declaration node
 * 
 * Node type is set to AST_VAR_DECL, Variable declaration struct values are
 * set based on arguemnts (data_type, var_name) and expression pointer is set
 * to default (NULL). Memory is allocated for var_name using strdup (must be freed later).
 * 
 * @param[in] data_type Data type of the variable
 * @param[in] var_name Variable identifier
 * @return Returns pointer to ASTNode or null if memory allocation failed
*/
ASTNode* create_var_decl_node(DataType data_type, char* var_name);

/**
 * @fn ASTNode* create_const_decl_node(DataType data_type, char* const_name)
 * @brief Function that creates constant declaration node
 * 
 * Node type is set to AST_CONST_DECL, Constant declaration struct values are
 * set based on arguemnts (data_type, const_name) and expression pointer is set
 * to default (NULL). Memory is allocated for const_name using strdup (must be freed later).
 * 
 * @param[in] data_type Data type of the constant
 * @param[in] var_name Constant identifier
 * @return Returns pointer to ASTNode or null if memory allocation failed
*/
ASTNode* create_const_decl_node(DataType data_type, char* const_name);

/**
 * @fn ASTNode* create_block_node()
 * @brief Function that creates block node
 * 
 * Node type is set to AST_BLOCK, Block struct values are
 * set to default (node_count = 0), memory is preallocated for
 * DEFAULT_BLOCK_NODE_COUNT nodes (can be re-allocated later) and all
 * pointers are set to NULL.
 * 
 * @return Returns pointer to ASTNode or null if memory allocation failed
*/
ASTNode* create_block_node();

/**
 * @fn ASTNode* create_while_node()
 * @brief Function that creates while node
 * 
 * Node type is set to AST_WHILE, While struct values are
 * set to default (elemebnt bind, experssion and block = NULL)
 * 
 * @return Returns pointer to ASTNode or null if memory allocation failed
*/
ASTNode* create_while_node();

/**
 * @fn ASTNode* create_if_node()
 * @brief Function that creates if node
 * 
 * Node type is set to AST_IF_ELSE, If else struct values are
 * set to default (elemebnt bind, experssion, if block and else block = NULL).
 * 
 * @return Returns pointer to ASTNode or null if memory allocation failed
*/
ASTNode* create_if_node();

/**
 * @fn ASTNode* create_fn_call_node(char* fn_name)
 * @brief Function that creates fn call node
 * 
 * Node type is set to AST_FN_CALL, Function call struct values
 * are set to default, memory is pre-allocated for argument array and capacity
 * is set to DEFAULT_FN_ARG_CNT. Memory for function name is allocaetd using strdup
 * (must be freed later). All pointers are set to NULL.
 * 
 * @param[in] fn_name Identifier of the function
 * @return Returns pointer to ASTNode or null if memory allocation failed
*/
ASTNode* create_fn_call_node(char* fn_name);

/**
 * @fn ASTNode* create_arg_node()
 * @brief Function that creates argument node
 * 
 * Node type is set to AST_ARG, Function Argument struct values
 * are set to default (expression = NULL). This node serves as a
 * packer node for expression node.
 * 
 * @return Returns pointer to ASTNode or null if memory allocation failed
*/
ASTNode* create_arg_node();

/**
 * // TODO CREATE ADITIONAL NODES
*/

/**
 * @fn void free_ast_node(ASTNode* node)
 * @brief Function that frees memory for all child node of node passed as an argument
 * 
 * This function deletes all nodes based on their types recursively,
 * function uses switch statement to switch between different node types
 * and frees coresponding structs within nodes based on the node type.
 * 
 * @param[in, out] node pointer to a node
 * @return void
*/
void free_ast_node(ASTNode* node);

#endif // AST_H

/**
 * @file parser.h
 * @brief Header file for parser.c
 * @authors Michal Repcik (xrepcim00)
*/

#ifndef AST_H
#define AST_H

typedef enum {
    AST_PROGRAM,
    AST_DECL,
    AST_ASSIGN,
    AST_FN_DEF,
    AST_FN_CALL,
    AST_IF,
    AST_WHILE,
    AST_RETURN,
    AST_BLOCK, // inside {}
    AST_TYPE_ANOT, // var x: i32 = 0; i32 is typa anot for variable x
    AST_BIN_OP // = - == != <= ...
    // add more
} ASTNodeType;

/**
 * @struct ASTNode
 * @brief TODO
*/
typedef struct {
    ASTNodeType type;
    union {
        struct {
            char* name;
        } identifier;

        struct {
            int value;
        } number_literal;
        // add more        
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
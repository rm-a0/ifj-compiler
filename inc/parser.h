/**
 * @file parser.h
 * @brief Header file for parser.c
 * @authors Michal Repcik (xrepcim00)
*/

#ifndef PARSER_H
#define PARSER_H

#include "token.h"  // Include definiton of Token struct
#include "lexer.h"  // Include definition of lexer struct
#include "ast.h"    // Include definition of ASTNodes

/**
 * @fn ASTNode* parse_const_decl(Lexer* lexer, Token** token)
 * @brief Parses constant declarations and constructs var_decl node of AST
 * 
 * This function parses source code and checks if the constant declaration
 * is valid or not while constructing AST constant declaration node. 
 * 
 * @param[in] lexer Pointer to a lexer struct
 * @param[in, out] token Pointer to a pointer of token
 * @return AST constant declaration node or NULL if syntax is invalid
*/
ASTNode* parse_const_decl(Lexer* lexer, Token** token);


/**
 * @fn ASTNode* parse_var_decl(Lexer* lexer, Token** token)
 * @brief Parses variable declarations and constructs var_decl node of AST
 * 
 * This function parses source code and checks if the variable declaration
 * is valid or not while constructing AST variable declaration node. 
 * 
 * @param[in] lexer Pointer to a lexer struct
 * @param[in, out] token Pointer to a pointer of token
 * @return AST variable declaration node or NULL if syntax is invalid
*/
ASTNode* parse_var_decl(Lexer* lexer, Token** token);

/** 
 * @fn ASTNode* parse_tokens(Lexer* lexer)
 * @brief Parses stream of tokens and creates an Abstract Syntax Tree (AST).
 * 
 * This function parses tokens provided by lexer, while generating AST.
 * Code is parsed using recursice descent method. Each non terminal has its own
 * function and can call coresponding funcions within. This function parses prolog
 * and top level declarations: variable, constant and function declarations. Each
 * of these declarations have their separate functions that continue parsing code 
 * and generating nodes. In this function program (root) node is created and other nodes
 * are connected to it. 
 * 
 * @param[in] lexer Pointer to a lexer struct
 * @return A pointer to the root node of the generated AST or NULL if syntax analysis failed.
 * 
 * @note The caller is responsible for freeing the memory allocated 
 * for the AST, memory of tokens is freed within this function after their use.
*/
ASTNode* parse_tokens(Lexer* lexer);

#endif // PARSER_H

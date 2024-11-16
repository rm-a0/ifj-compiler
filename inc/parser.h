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
 * @fn void advance_token(Token** token, Lexer* lexer);
 * @brief Frees token if its not NULL and gets another one from lexer
 * 
 * Function gets next token from stream input and frees previous token 
 * if it wasnt NULL.
 * 
 * @param[in, out] token Pointer to a pointer of token
 * @param[in] lexer Pointer to a lexer struct
 * @return void
*/
void advance_token(Token** token, Lexer* lexer);

/**
 * @fn int check_token(Token* token, TokenType expected_type, const char* expected_value)
 * @brief Checks if token has expected type and value
 * 
 * If token is null, expected type or value is not matching function returns false,
 * when expected value is set to NULL, value is not checked, only type.
 * 
 * @param[in] token Pointer to a token
 * @param[in] expected_type Expected token type
 * @param[in] expected_value Expected token value
 * @return Returns 1 if the token matches or 0 if it does not
*/
int check_token(Token* token, TokenType expected_type, const char* expected_value);

/**
 * @fn int parse_prolog(Lexer* lexer, Token** token)
 * @brief Parses prolog
 * 
 * This function parses source code and checks if the prolog is
 * is valid or not.
 * 
 * @param[in] lexer Pointer to a lexer struct
 * @param[in, out] token Pointer to a pointer of token
 * @return Returns 1 if the prolog is correrct or 0 if its not.
*/
int parse_prolog(Lexer* lexer, Token** token);

/**
 * @fn int parse_element_bind(Lexer* lexer, Token** token, ASTNode* node)
 * @brief Parses element bind
 * 
 * This function parses source code and checks if the element bind
 * is valid or not. 
 * 
 * @param[in] lexer Pointer to a lexer struct
 * @param[in, out] token Pointer to a pointer of token
 * @param[in, out] node pointer to a node element bind is connected to
 * @return 0 if element bind is not there or valid and 1 if the element bind is invalid
*/
int parse_element_bind(Lexer* lexer, Token** token, ASTNode* node);

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
 * @fn ASTNode* parse_fn_decl(Lexer* lexer, Token** token)
 * @brief Parses function declarations and constructs fn_decl node of AST
 * 
 * This function parses source code and checks if the function declaration
 * is valid or not while constructing AST function declaration node. 
 * 
 * @param[in] lexer Pointer to a lexer struct
 * @param[in, out] token Pointer to a pointer of token
 * @return AST function declaration node or NULL if syntax is invalid
*/
ASTNode* parse_fn_decl(Lexer* lexer, Token** token);

/**
 * @fn ASTNode* parse_fn_params(Lexer* lexer, Token** token)
 * @brief Parses function parameters and constructs fn parameter node of AST
 * 
 * This function parses source code and checks if the function parameters 
 * are valid or not while constructing AST function parameter node. 
 * 
 * @param[in] lexer Pointer to a lexer struct
 * @param[in, out] token Pointer to a pointer of token
 * @return AST function declaration node or NULL if syntax is invalid
*/
ASTNode* parse_fn_params(Lexer* lexer, Token** token);

/**
 * @fn ASTNode* parse_block(Lexer* lexer, Token** token)
 * @brief Parses block and constructs block node of AST
 * 
 * This function parses source code and checks if the block structure
 * is valid or not while constructing AST block node. 
 * 
 * @param[in] lexer Pointer to a lexer struct
 * @param[in, out] token Pointer to a pointer of token
 * @return AST block node or NULL if syntax is invalid
*/
ASTNode* parse_block(Lexer* lexer, Token** token);

/**
 * @fn ASTNode* parse_if_else(Lexer* lexer, Token** token)
 * @brief Parses if/else statements and constructs if_else node of AST
 * 
 * This function parses source code and checks if the if/else structure
 * is valid or not while constructing AST if_else node. 
 * 
 * @param[in] lexer Pointer to a lexer struct
 * @param[in, out] token Pointer to a pointer of token
 * @return AST if/else node or NULL if syntax is invalid
*/
ASTNode* parse_if_else(Lexer* lexer, Token** token);

/** 
 * @fn ASTNode* parse_while(Lexer* lexer, Token** token)
 * @brief Parses while cycle and constructs while node
 * 
 * This function parses source code and checks if the whiel cycle
 * is valid or not while constructing AST while node.
 * 
 * @param[in] lexer Pointer to a lexer struct
 * @param[in, out] token Pointer to a pointer of token
 * @return AST if/else node or NULL if syntax is invalid
*/ 
ASTNode* parse_while(Lexer* lexer, Token** token);

/** 
 * @fn ASTNode* parse_fn_call(Lexer* lexer, Token** token, char* identifier)
 * @brief Parses function call aand construct fn_call node
 * 
 * This function parses source code and checks if the function call
 * is valid or not while constructing AST fn_call node.
 * 
 * @param[in] lexer Pointer to a lexer struct
 * @param[in, out] token Pointer to a pointer of token
 * @param[in] identifier Identifier of a function
 * @return AST fn call node or NULL if syntax is invalid
*/ 
ASTNode* parse_fn_call(Lexer* lexer, Token** token, char* identifier);

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

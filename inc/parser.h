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
 * @fn ASTNode* parse_tokens(Token* tokens, int token_count)
 * @brief Parses an array of tokens and creates an Abstract Syntax Tree (AST).
 *
 * @return A pointer to the root node of the generated AST or NULL if syntax analysis failed.
 * 
 * @note The caller is responsible for freeing the memory allocated 
 * for the tokens, as well as managing the memory for the generated AST.
*/
ASTNode* parse_tokens(Lexer* lexer);

#endif // PARSER_H

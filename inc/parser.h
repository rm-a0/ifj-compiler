/**
 * @file parser.h
 * @brief Header file for parser.c
 * @authors Michal Repcik (xrepcim00)
*/

#ifndef PARSER_H
#define PARSER_H

#include "token.h"  // Include definiton of Token struct
#include "ast.h"    // Include definition of ASTNode

/** 
 * @fn ASTNode* parse_tokens(Token* tokens, int token_count)
 * @brief Parses an array of tokens and creates an Abstract Syntax Tree (AST).
 *
 * @return A pointer to the root node of the generated AST.
 * 
 * @note The caller is responsible for freeing the memory allocated 
 * for the tokens, as well as managing the memory for the generated AST.
 */
ASTNode* parse_tokens();

#endif // PARSER_H
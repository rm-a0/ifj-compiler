/**
 * @file token.h
 * @brief Header file for token.c
 * @authors Michal Repcik (xrepcim00)
*/

#ifndef TOKEN_H
#define TOKEN_H

/**
 * @brief Constants for different token types
 * @param TOKEN_IF
 * @param TOKEN_ELSE
 */
typedef enum {
    TOKEN_IF,
    TOKEN_ELSE
} TokenType;

/**
 * @brief Struct containing Token info
 * @param token_type type of token
 * @param value token value
 */
typedef struct {
    TokenType token_type;
    char* value;
} Token;

/**
 * @brief Returns token
 * @param[in] None
 * @return Token
*/
Token get_token();

#endif // TOKEN_H


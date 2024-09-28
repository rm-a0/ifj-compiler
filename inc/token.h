/**
 * @file token.h
 * @brief Header file for token.c
 * @authors Michal Repcik (xrepcim00)
*/

#ifndef TOKEN_H
#define TOKEN_H

/**
 * @enum TokenType
 * @brief Constants for different token types
 * 
 * @param TOKEN_CONST           Constant for 'const' keyword.
 * @param TOKEN_ELSE            Constant for 'else' keyword.
 * @param TOKEN_FN              Constant for 'fn' keyword.
 * @param TOKEN_IF              Constant for 'if' keyword.
 * @param TOKEN_I32             Constant for 'i32' keyword.
 * @param TOKEN_F64             Constant for 'f64' keyword.
 * @param TOKEN_NULL            Constant for 'null' keyword.
 * @param TOKEN_PUB             Constant for 'pub' keyword.
 * @param TOKEN_RETURN          Constant for 'return' keyword.
 * @param TOKEN_U8              Constant for 'u8' keyword.
 * @param TOKEN_VAR             Constant for 'var' keyword.
 * @param TOKEN_VOID            Constant for 'void' keyword.
 * @param TOKEN_WHILE           Constant for 'while' keyword.
 * @param TOKEN_IDENTIFIER      Constant for identifier.
 * @param TOKEN_L_PAREN         Constant for left parenthesis.
 * @param TOKEN_R_PAREN         Constant for right parenthesis.
 * @param TOKEN_EOF             Constant for end of file.
*/
typedef enum {
    TOKEN_CONST,
    TOKEN_ELSE,
    TOKEN_FN,
    TOKEN_IF,
    TOKEN_I32,
    TOKEN_F64,
    TOKEN_NULL,
    TOKEN_PUB,
    TOKEN_RETURN,
    TOKEN_U8,
    TOKEN_VAR,
    TOKEN_VOID,
    TOKEN_WHILE,
    TOKEN_IDENTIFIER,
    TOKEN_L_PAREN,
    TOKEN_R_PAREN,
    TOKEN_EOF,
} TokenType;

/**
 * @struct Token
 * @brief Struct containing informations about token
 * 
 * @param token_type type of token
 * @param value token value
*/
typedef struct {
    TokenType token_type;
    char* value;
} Token;

/**
 * @fn Token get_token()
 * @brief Returns token
 * 
 * @param[in] None
 * @return Token
*/
Token get_token();

#endif // TOKEN_H

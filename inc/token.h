/**
 * @file token.h
 * @brief Contains constants and utilities for token management
 * @authors Michal Repcik (xrepcim00)
*/

#ifndef TOKEN_H
#define TOKEN_H

/**
 * @enum TokenType
 * @brief Constants for different token types
 * 
 * @param INVALID               Constant for invalid tokens.
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
 * @param TOKEN_STRING          Constant for string.
 * @param TOKEN_L_PAREN         Constant for '(' character.
 * @param TOKEN_R_PAREN         Constant for ')' character.
 * @param TOKEN_L_BRACE         Constant for '{' character.
 * @param TOKEN_R_BRACE         Constant for '}' character.
 * @param TOKEN_DOT             Constant for '.' character.
 * @param TOKEN_COMA            Constant for ',' character.
 * @param TOKEN_COLON           Constant for ':' character.
 * @param TOKEN_SEMICOLON       Constant for ';' character.
 * @param TOKEN_PIPE            Constant for '|' character.
 * @param TOKEN_PLUS            Constant for '+' character.
 * @param TOKEN_MINUS           Constant for '-' character.
 * @param TOKEN_MULT            Constant for '*' character.
 * @param TOKEN_DIV             Constant for '/' character.
 * @param TOKEN_ASSIGN          Constant for '=' character.
*/
typedef enum {
    INVALID,
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
    TOKEN_STRING,
    TOKEN_L_PAREN,
    TOKEN_R_PAREN,
    TOKEN_L_BRACE,
    TOKEN_R_BRACE,
    TOKEN_DOT,
    TOKEN_COMA,
    TOKEN_COLON,
    TOKEN_SEMICOLON,
    TOKEN_PIPE,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULT,
    TOKEN_DIV,
    TOKEN_ASSIGN
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
 * @fn Token* create_token(TokenType token_type, size_t lenght, const char* value)
 * @brief Allocates and initializes Token struct
 * 
 * @param[in] toke_type Type of token
 * @param[in] length Length of string that that is passed as value
 * @param[in] value String that is assigned as token value
 * @return Returns pointer to a token
 * 
 * @note When function is called with length set to 0, memory for value
 * wont be allocated and char* value in Token will be set to NULL pointer
*/
Token* create_token(TokenType token_type, size_t lenght, const char* value);

/**
 * @fn void free_token(Token *token)
 * @brief Frees memory allocated for Token struct
 * 
 * @param[in] token Pointer to a token
*/
void free_token(Token *token);

#endif // TOKEN_H

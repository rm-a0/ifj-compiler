/**
 * @file token.h
 * @brief Contains constants and utilities for token management
 * @authors Michal Repcik (xrepcim00)
*/

#ifndef TOKEN_H
#define TOKEN_H

/**
 * @enum TokenType
 * @brief Constants for different token types.
 */
typedef enum {
    INVALID,              ///< Constant for invalid tokens.
    TOKEN_CONST,          ///< Constant for 'const' keyword.
    TOKEN_ELSE,           ///< Constant for 'else' keyword.
    TOKEN_FN,             ///< Constant for 'fn' keyword.
    TOKEN_IF,             ///< Constant for 'if' keyword.
    TOKEN_I32,            ///< Constant for 'i32' keyword.
    TOKEN_F64,            ///< Constant for 'f64' keyword.
    TOKEN_NULL,           ///< Constant for 'null' keyword.
    TOKEN_PUB,            ///< Constant for 'pub' keyword.
    TOKEN_RETURN,         ///< Constant for 'return' keyword.
    TOKEN_U8,             ///< Constant for 'u8' keyword.
    TOKEN_VAR,            ///< Constant for 'var' keyword.
    TOKEN_VOID,           ///< Constant for 'void' keyword.
    TOKEN_WHILE,          ///< Constant for 'while' keyword.
    TOKEN_IDENTIFIER,     ///< Constant for identifier.
    TOKEN_STRING,         ///< Constant for string.
    TOKEN_INTEGER,        ///< Constant for integer (whole number).
    TOKEN_FLOAT,          ///< Constant for float (decimal number).
    TOKEN_SLICE,          ///< Constant for slice '[]'.
    TOKEN_L_PAREN,        ///< Constant for '(' character.
    TOKEN_R_PAREN,        ///< Constant for ')' character.
    TOKEN_L_BRACE,        ///< Constant for '{' character.
    TOKEN_R_BRACE,        ///< Constant for '}' character.
    TOKEN_DOT,            ///< Constant for '.' character.
    TOKEN_COMMA,          ///< Constant for ',' character.
    TOKEN_COLON,          ///< Constant for ':' character.
    TOKEN_SEMICOLON,      ///< Constant for ';' character.
    TOKEN_PIPE,           ///< Constant for '|' character.
    TOKEN_PLUS,           ///< Constant for '+' character.
    TOKEN_MINUS,          ///< Constant for '-' character.
    TOKEN_MULT,           ///< Constant for '*' character.
    TOKEN_DIV,            ///< Constant for '/' character.
    TOKEN_ASSIGN,         ///< Constant for '=' character.
    TOKEN_Q_MARK,         ///< Constant for '?' character.
    TOKEN_LESS,           ///< Constant for '<' character.
    TOKEN_GREATER,        ///< Constant for '>' character.
    TOKEN_EXCM,           ///< Constant for '!' character.
    TOKEN_LESS_EQU,       ///< Constant for '<=' operator.
    TOKEN_GREATER_EQU,    ///< Constant for '>=' operator.
    TOKEN_NOT_EQU,        ///< Constant for '!=' operator.
    TOKEN_EQU,            ///< Constant for '==' operator.
    TOKEN_IMPORT          ///< Constant for '@import' directive.
} TokenType;

/**
 * @struct Token
 * @brief Struct containing informations about token.
*/
typedef struct {
    TokenType token_type;   ///< Type of token
    char* value;            ///< Token value 
} Token;

/**
 * @fn Token* create_token(TokenType token_type, size_t lenght, const char* value)
 * @brief Allocates and initializes Token struct.
 * 
 * @param[in] toke_type Type of token
 * @param[in] length Length of string that that is passed as value
 * @param[in] value String that is assigned as token value
 * @return Returns pointer to a token
 * 
 * @note When function is called with length set to 0, memory for value
 * wont be allocated and char* value in Token will be set to NULL pointer.
*/
Token* create_token(TokenType token_type, size_t lenght, const char* value);

/**
 * @fn void free_token(Token *token)
 * @brief Frees memory allocated for Token struct.
 * 
 * @param[in] token Pointer to a token
*/
void free_token(Token *token);

#endif // TOKEN_H

/**
 * @file lexer.h
 * @brief Contains declarations and definitions of main lexer components
 * @authors Michal Repcik (xrepcim00)
*/

#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "token.h"
#include "ascii_lookup.h"

typedef enum {
    START,
    ID_OR_KEY,
    FWD_SLASH,
    COMMENT,
    Q_MARK,
    UNDERSCORE,
    KEYWORD,
    L_SQ_BRACKET,
    R_SQ_BRACKET,
    STRING,
    ESC_SEQ,
    HEX_NUM,
    ZERO,
    INTEGER,
    FLOAT,
    EXPONENT,
    EXPONENT_NUM,
    SIGN,
    MULTI_OP
} LexerState;

/**
 * @struct Lexer
 * @brief Struct containing context for lexer
 * 
 * @param src Pointer to source file or stdin
 * @param ascii_l_table Lookup table for validating ascii characters
 * @param state State of the lexer
 * @param buff Buffer that holds value of last created token
 * @param buff_len Length of buffer
*/
typedef struct {
    FILE* src;
    LookupTable ascii_l_table;
    LexerState state;
    char* buff;
    int buff_len;
} Lexer;

/**
 * @fn int init_lexer(Lexer* lexer, const char* file)
 * @brief Initializes src, ascii lookup table and allocates buffer
 * for storing token values inside lexer struct.
 * 
 * @param[out] lexer Pointer to lexer struct
 * @param[in] fp Pointer to a file/stding
 * @return Returns 0 when everything went succesfully, otherwise returns 1-
*/
int init_lexer(Lexer* lexer, FILE* fp);

/**
 * @fn destroy_lexer(Lexer* lexer)
 * @brief frees all allocated memory inside lexer struct
 * 
 * @param[in, out] lexer Pointer to a lexer struct
 * @return void
*/
void destroy_lexer(Lexer* lexer);

/**
 * @fn Token* get_token(Lexer* lexer)
 * @brief Scans source code and extracts tokens using FSM
 *
 * @param[in, out] lexer Pointer to lexer struct
 * @return Pointer to a token struct or NULL if the token is invalid (Lexical error)
*/
Token* get_token(Lexer* lexer);

#endif // LEXER_H
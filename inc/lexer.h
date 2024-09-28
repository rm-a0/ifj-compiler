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
    ZERO,
    INTEGER,
    FLOAT,
    EXPONENT,
    SIGN
} LexerState;

/**
 * @struct Lexer
 * @brief Struct containing context for lexer
 * 
 * @param src Pointer to source file or stdin
 * @param predefined_tokens Array of predefined tokensA
 * @param ascii_l_table Lookup table for validating ascii characters
*/
typedef struct {
    FILE* src;
    Token* predefined_tokens;
    LookupTable ascii_l_table;
    LexerState state;
} Lexer;

/**
 * @fn int init_lexer(Lexer* lexer, const char* file)
 * @brief Initializes src, predefined tokens and ascii lookup
 * table inside Lexer struct.
 * 
 * @param[out] lexer Pointer to lexer struct
 * @param[in] file Name of the file
 * @return Returns 0 when everything went succesfully, otherwise returns 1-
 * 
 * @note fopen was used in this function, dont forget to close the file.
 * Other elements were not allocated using malloc so there is no need to
 * free memory.
*/
int init_lexer(Lexer* lexer, const char* file);

Token* get_token(Lexer* lexer);

#endif // LEXER_H
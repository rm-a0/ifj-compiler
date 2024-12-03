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
#include "keyword_htab.h"

/**
 * @enum LexerState
 * @brief Constants for finite state machine (FSM) states in the lexer.
*/
typedef enum {
    START,              ///< Initial state of the lexer, ready to begin tokenization.
    ID_OR_KEY,          ///< State for identifying either an identifier or a keyword.
    FWD_SLASH,          ///< State after encountering a forward slash '/'.
    BCK_SLASH,          ///< State after encountering a backslash '\'.
    COMMENT,            ///< State for processing comments.
    Q_MARK,             ///< State for processing a question mark '?'.
    UNDERSCORE,         ///< State for processing underscores '_'.
    KEYWORD,            ///< State for processing keywords.
    L_SQ_BRACKET,       ///< State for processing the left square bracket '['.
    R_SQ_BRACKET,       ///< State for processing the right square bracket ']'.
    STRING,             ///< State for processing string literals.
    MULTI_STRING,       ///< State for processing multi line string literals.
    EO_ML_STRING,       ///< State for processing end of multi line string literals.
    ESC_SEQ,            ///< State for processing escape sequences.
    HEX_NUM,            ///< State for processing hexadecimal numbers.
    ZERO,               ///< State for processing the integer zero.
    INTEGER,            ///< State for processing integer literals.
    FLOAT,              ///< State for processing floating-point literals.
    EXPONENT,           ///< State for processing exponent parts of numbers.
    EXPONENT_NUM,       ///< State for processing numbers in the exponent.
    SIGN,               ///< State for processing a sign (+ or -) in numeric literals.
    MULTI_OP,           ///< State for processing multi-character operators.
    IMPORT              ///< State for processing the '@import' directive.
} LexerState;

/**
 * @struct Lexer
 * @brief Struct containing context for the lexer.
 * 
 * This structure maintains actual state of FSM and important
 * context information used for code tokeniziation.
*/
typedef struct {
    FILE* src;                     ///< Pointer to the source file or stdin.
    LookupTable ascii_l_table;     ///< Lookup table for validating ASCII characters.
    KeywordHtab* keyword_htab;     ///< Hash table for fast keyword access.
    LexerState state;              ///< Current state of the lexer.
    char* buff;                    ///< Buffer that holds the value of the last created token.
    int buff_len;                  ///< Length of the buffer.
} Lexer;

/**
 * @fn int init_lexer(Lexer* lexer, const char* file)
 * @brief Initializes src, ascii lookup table and allocates buffer
 * for storing token values inside lexer struct.
 * 
 * @param[out] lexer Pointer to lexer struct
 * @param[in] fp Pointer to a file/stding
 * @return Returns 0 when everything went succesfully, otherwise returns -1
*/
int init_lexer(Lexer* lexer, FILE* fp);

/**
 * @fn destroy_lexer(Lexer* lexer)
 * @brief Closes src, destroys keyword_htab, frees buffer and sets all pointers to NULL.
 * 
 * @param[in, out] lexer Pointer to a lexer struct
 * @return void
*/
void destroy_lexer(Lexer* lexer);

/**
 * @fn Token* get_token(Lexer* lexer)
 * @brief Scans source code and extracts tokens using FSM.
 *
 * @param[in, out] lexer Pointer to lexer struct
 * @return Pointer to a token struct or NULL if the token is invalid (Lexical error)
*/
Token* get_token(Lexer* lexer);

#endif // LEXER_H

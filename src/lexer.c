/**
 * @file lexer.c
 * @brief Contains definitons of fucntions declared in lexer.c
 * @authors Michal Repcik (xrepcim00)
*/
#include <ctype.h>
#include <stdlib.h>

#include "lexer.h"
#include "ascii_lookup.h"
#include "keyword_htab.h"
#include "error.h"

#define BUFFER_LENGTH 128

// Checkes if a character is valid based on the lookup table (implementation in ascii_lookup.c)
static inline int isvalid(int c, LookupTable table) {
    return (c < 128 && table[c] != INVALID);
}

// Checks if character is valid hexadecimal digit (0-9, a-f, A-F)
static inline int ishexnum(int c) {
    return ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9'));
}

// Checks if character is valid escape sequence ('n', 't', 'r', '"', '\')
static inline int isescseq(int c) {
    return (c == 'n' || c == 't' || c == 'r' || c == '"' || c == '\\');
}

// Appneds character to a buffer, increments index and resizes buffer if needed 
static inline void append(Lexer* lexer, int* idx, int c) {
    if (*idx >= lexer->buff_len - 1) { // If buffer is too small double its length
        lexer->buff_len *= 2;
        lexer->buff = realloc(lexer->buff, lexer->buff_len);
        if (lexer->buff == NULL) {
            fprintf(stderr, "Failed to reallocate memory for buffer in lexer");
            exit(INTERNAL_ERROR);
        }
    }

    lexer->buff[*idx] = (char)c; // Append c
    (*idx)++; // Increment index
    lexer->buff[*idx] = '\0'; // Null terminate the string

}

int init_lexer(Lexer* lexer, FILE* fp) {
    if (lexer == NULL) {
        return -1;
    }
    // Allocate memory for buffer used in get_token function
    lexer->buff = malloc(BUFFER_LENGTH * sizeof(char));
    if (lexer->buff == NULL) {
        return -1;
    }

    lexer->buff_len = BUFFER_LENGTH; // Set length of buffer
    lexer->src = fp; // Initialize file/stdin pointer
    init_lookup_table(lexer->ascii_l_table); // Initialize lookup table
    lexer->keyword_htab = create_keyword_htab(OPTIMAL_SIZE); // Allocate memory for hash table
    init_keyword_htab(lexer->keyword_htab); // Fill hash table with keywords
    lexer->state = START; // Set state to start

    return 0;
}

void destroy_lexer(Lexer* lexer) {
    if (lexer == NULL) {
        return;
    }

    if (lexer->buff != NULL) {
        free(lexer->buff);
        lexer->buff = NULL;
    }

    if (lexer->src != NULL) {
        fclose(lexer->src);
        lexer->src = NULL;
    }

    if (lexer->keyword_htab != NULL) {
        destroy_keyword_htab(lexer->keyword_htab);
        lexer->keyword_htab = NULL;
    }
}

Token* get_token(Lexer* lexer) {
    int c;
    int hex_cnt = 0; // Counts how many hexadecimal numbers are in '\xdd' esc sequence
    int exp_flag = 0; // Flag for exponent value to prevent empty exponents
    int idx = 0; // Index for indexing buffer for token values

    while ((c = fgetc(lexer->src)) != EOF) {
        switch (lexer->state) {
            case START:
                switch (c) {
                    case ' ':
                    case '\t':
                    case '\n':
                        break; // Continue in START state
                    case '/':
                        lexer->state = FWD_SLASH;
                        break;
                    case '@':
                        lexer->state = IMPORT;
                        break;
                    case '"':
                        lexer->state = STRING;
                        break;
                    case '0':
                        lexer->state = ZERO;
                        append(lexer, &idx, c);
                        break;
                    case '?':
                        lexer->state = Q_MARK;
                        return create_token(TOKEN_Q_MARK, 0, NULL);
                        break;
                    case '_':
                        lexer->state = UNDERSCORE;
                        append(lexer, &idx, c);
                        break;
                    case '[':
                        lexer->state = L_SQ_BRACKET;
                        break;
                    case '>':
                    case '<':
                    case '=':
                    case '!':
                        lexer->state = MULTI_OP;
                        append(lexer, &idx, c);
                        break;
                    default:
                        if (isalpha(c)) {
                            lexer->state = ID_OR_KEY;
                            append(lexer, &idx, c);
                        }
                        else if (c > '0' && c <= '9') {
                            lexer->state = INTEGER;
                            append(lexer, &idx, c);
                        }
                        else if (isvalid(c, lexer->ascii_l_table)) {
                            // precreate tokens for single characters
                            return create_token(lexer->ascii_l_table[c], 0, NULL);
                        }
                        else {
                            set_error(LEXICAL_ERROR);
                            return NULL; // Invalid character
                        }
                        break;
                }
                break;
            case IMPORT:
                if (idx < 5) {
                    append(lexer, &idx, c);
                    break; // Continue in import state
                }
                else if (idx == 5) { 
                    append(lexer, &idx, c);
                    if (strcmp(lexer->buff, "import") == 0) {
                        lexer->state = START;
                        return create_token(TOKEN_IMPORT, 0, NULL);
                    }
                    else {
                        set_error(LEXICAL_ERROR);
                        return NULL;
                    }
                }
                else {
                    set_error(LEXICAL_ERROR);
                    return NULL;
                }
                break;
            case MULTI_OP:
                if (c == '=') {
                    lexer->state = START;
                    switch ((int)lexer->buff[0]) {
                        case '<' :
                            return create_token(TOKEN_LESS_EQU, 0, NULL);
                        case '>':
                            return create_token(TOKEN_GREATER_EQU, 0, NULL);
                        case '!':
                            return create_token(TOKEN_NOT_EQU, 0, NULL);
                        case '=':
                            return create_token(TOKEN_EQU, 0, NULL);
                        default:
                            break;
                    }
                }
                else if (isvalid(c, lexer->ascii_l_table) || isspace(c)) {
                    lexer->state = START;
                    ungetc(c, lexer->src);
                    return create_token(lexer->ascii_l_table[(int)lexer->buff[0]], 0, NULL);
                }
                else {
                    set_error(LEXICAL_ERROR);
                    return NULL;
                }
                break;
            case ID_OR_KEY:
                if (isalnum(c) || c == '_') {
                    append(lexer, &idx, c);
                    break; // Continue in ID_OR_KEY state
                }
                else if (isspace(c) || isvalid(c, lexer->ascii_l_table)) {
                    ungetc(c, lexer->src); // Put c back to stream
                    lexer->state = START;
                    // find token in hash table
                    TokenType token = find_keyword(lexer->keyword_htab, lexer->buff);
                    if (token == TOKEN_IDENTIFIER) {
                        return create_token(token, idx, lexer->buff); // create token for identifier
                    }
                    else {
                        return create_token(token, 0, NULL); // create token for specific keyword
                    }
                }
                else {
                    set_error(LEXICAL_ERROR);
                    return NULL; // Invalid identifier
                }
                break;
            case STRING:
                if (c == '"') {
                    lexer->state = START;
                    return create_token(TOKEN_STRING, idx, lexer->buff);
                }
                if (c == '\\') {
                    lexer->state = ESC_SEQ;
                    append(lexer, &idx, c);
                }
                else if (c == '\n') {
                    set_error(LEXICAL_ERROR);
                    return NULL; // Invalid string character
                }
                else {
                    append(lexer, &idx, c);
                    break; // Continue in STRING state
                }
                break;
            case ESC_SEQ:
                if (isescseq(c)) {
                    lexer->state = STRING;
                    append(lexer, &idx, c);
                }
                else if (c == 'x') {
                    lexer->state = HEX_NUM;
                    append(lexer, &idx, c);
                }
                else {
                    set_error(LEXICAL_ERROR);
                    return NULL; // Invalid escape sequence
                }
                break;
            case HEX_NUM:
                if (ishexnum(c) && hex_cnt < 2) {
                    hex_cnt++;
                    append(lexer, &idx, c);
                    if (hex_cnt == 2) {
                        lexer->state = STRING;
                    }
                }
                else {
                    set_error(LEXICAL_ERROR);
                    return NULL;
                }
                break;
            case UNDERSCORE:
                if (c == '_') {
                    append(lexer, &idx, c);
                    break; // Countinue in UNDERSCORE state
                }
                else if (isalnum(c)) {
                    lexer->state = ID_OR_KEY;
                    append(lexer, &idx, c);
                }
                else {
                    set_error(LEXICAL_ERROR);
                    return NULL;
                }
                break;
            case ZERO:
                if (c == '.') {
                    lexer->state = FLOAT;
                    append(lexer, &idx, c);
                }
                else if (isspace(c) || isvalid(c, lexer->ascii_l_table)) { // is dot valid?
                    lexer->state = START;
                    ungetc(c, lexer->src); // Put c back to stream
                    return create_token(TOKEN_INTEGER, 1, "0");
                }
                else {
                    set_error(LEXICAL_ERROR);
                    return NULL; // Invalid number
                }
                break;
            case INTEGER:
                if (c == '.') {
                    lexer->state = FLOAT;
                    append(lexer, &idx, c);
                }
                else if (c == 'e' || c == 'E') {
                    lexer->state = EXPONENT;
                    append(lexer, &idx, c);
                }
                else if (c >= '0' && c <= '9') {
                    append(lexer, &idx, c);
                    break; // Continue in INTEGER state 
                }
                else if (isspace(c) || isvalid(c, lexer->ascii_l_table)) { // is dot valid?
                    lexer->state = START;
                    ungetc(c, lexer->src); // Put c back to stream
                    return create_token(TOKEN_INTEGER, idx, lexer->buff);
                }
                else {
                    set_error(LEXICAL_ERROR);
                    return NULL; // Invalid integer
                }
                break;
            case FLOAT:
                if (c >= '0' && c <= '9') {
                    append(lexer, &idx, c);
                    break; // Continue in FLOAT state
                }
                else if (c == 'e' || c == 'E') {
                    lexer->state = EXPONENT;
                    append(lexer, &idx, c);
                }
                else if (isspace(c) || isvalid(c, lexer->ascii_l_table)) { // is dot valid?
                    lexer->state = START;
                    ungetc(c, lexer->src);
                    return create_token(TOKEN_FLOAT, idx, lexer->buff); // TODO
                }
                else {
                    set_error(LEXICAL_ERROR);
                    return NULL; // Invalid float
                }
                break;
            case EXPONENT:
                if (c == '+' || c == '-') {
                    lexer->state = SIGN;
                    append(lexer, &idx, c);
                }
                else if (c >= '0' && c <= '9') {
                    exp_flag = 1;
                    lexer->state = EXPONENT_NUM;
                    append(lexer, &idx, c);
                }
                else {
                    set_error(LEXICAL_ERROR);
                    return NULL;
                }
                break;
            case SIGN:
                if (c >= '0' && c <= '9') {
                    exp_flag = 1; // Set exponent flag
                    lexer->state = EXPONENT_NUM;
                    append(lexer, &idx, c);
                }
                else {
                    set_error(LEXICAL_ERROR);
                    return NULL;
                }
                break;
            case EXPONENT_NUM:
                if (c >= '0' && c <= '9') {
                    exp_flag = 1; // Set exponent flag
                    append(lexer, &idx, c);
                    break; // Continue in EXPONENT_NUM state
                }
                else if ((isspace(c) || isvalid(c, lexer->ascii_l_table)) && exp_flag) {
                    lexer->state = START;
                    ungetc(c, lexer->src);
                    return create_token(TOKEN_FLOAT, idx, lexer->buff);
                }
                else {
                    set_error(LEXICAL_ERROR);
                    return NULL;
                }
                break;
            case FWD_SLASH:
                if (c == '/') {
                    lexer->state = COMMENT;
                }
                else if (isspace(c) || isvalid(c, lexer->ascii_l_table) || isalnum(c)) {
                    lexer->state = START;
                    ungetc(c, lexer->src);
                    return create_token(TOKEN_DIV, 0, NULL);
                }
                else {
                    set_error(LEXICAL_ERROR);
                    return NULL;
                }
                break;
            case COMMENT:
                if (c == '\n') {
                    lexer->state = START;
                }
                break; // Continue in COMMENT state
            case Q_MARK:
                if (c == '[') {
                    lexer->state = L_SQ_BRACKET;
                }
                else if (isalpha(c)) {
                    lexer->state = KEYWORD;
                    append(lexer, &idx, c);
                }
                else {
                    set_error(LEXICAL_ERROR);
                    return NULL;
                }
                break;
            case KEYWORD:
                if (isalnum(c)) {
                    append(lexer, &idx, c);
                    break; // Continue in KEYWORD state
                }
                else if (isspace(c) || isvalid(c, lexer->ascii_l_table)) {
                    lexer->state = START;
                    ungetc(c, lexer->src);
                    // return corresponding token stored in hash table
                    TokenType token = find_keyword(lexer->keyword_htab, lexer->buff);
                    return create_token(token, 0, NULL);
                }
                else {
                    set_error(LEXICAL_ERROR);
                    return NULL;
                }
                break;
            case L_SQ_BRACKET:
                if (c == ']') {
                    lexer->state = R_SQ_BRACKET;
                    return create_token(TOKEN_SLICE, 0, NULL);
                }
                else {
                    set_error(LEXICAL_ERROR);
                    return NULL;
                }
                break;
            case R_SQ_BRACKET:
                if (isalpha(c)) {
                    lexer->state = KEYWORD;
                    append(lexer, &idx, c);
                }
                else {
                    set_error(LEXICAL_ERROR);
                    return NULL;
                }
                break;
        }
    }

    return create_token(TOKEN_EOF, 0, NULL);
}

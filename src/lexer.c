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

int isvalid(int c, LookupTable table) {
    return (c < 128 && table[c] != INVALID);
}

int ishexnum(int c) {
    return ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9'));
}

int isescseq(int c) {
    return (c == 'n' || c == 't' || c == 'r' || c == '"' || c == '\\');
}

int init_lexer(Lexer* lexer, FILE* fp) {
    if (lexer == NULL) {
        return -1;
    }

    lexer->src = fp; // Initialize file/stdin pointer

    init_lookup_table(lexer->ascii_l_table); // Initialize lookup table

    lexer->predefined_tokens = NULL; // Initialize predefined tokens

    lexer->state = START; // Set state to start

    return 0;
}

Token* get_token(Lexer* lexer) {
    int c;
    int hex_cnt = 0;
    char buff [128];

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
                    case '"':
                        lexer->state = STRING;
                        break;
                    case '0':
                        lexer->state = ZERO;
                        break;
                    case '?':
                        lexer->state = Q_MARK;
                        return create_token(TOKEN_Q_MARK, 0, NULL);
                        break;
                    case '_':
                        lexer->state = UNDERSCORE;
                        break;
                    case '[':
                        lexer->state = L_SQ_BRACKET;
                        break;  
                    default:
                        if (isalpha(c)) {
                            lexer->state = ID_OR_KEY;
                        }
                        else if (c > '0' && c <= '9') {
                            lexer->state = INTEGER;
                        }
                        else if (isvalid(c, lexer->ascii_l_table)) {
                            // precreate tokens for single characters
                            return create_token(lexer->ascii_l_table[c], 1, (char)c);
                        }
                        else {
                            return NULL; // Invalid character
                        }
                        break;
                }
                break;
            case ID_OR_KEY:
                if (isalnum(c) || c == '_') {
                    break; // Continue in ID_OR_KEY state
                }
                else if (isspace(c) || isvalid(c, lexer->ascii_l_table)) {
                    ungetc(c, lexer->src); // Put c back to stream
                    lexer->state = START;
                    return create_token(TOKEN_IDENTIFIER, 0, NULL);
                    // return either id or keyword
                    // implement hashtable for keywords
                }
                else {
                    return NULL; // Invalid identifier
                }
                break;
            case STRING:
                if (c == '"') {
                    lexer->state = START;
                    // return string token
                }
                if (c == '\\') {
                    lexer->state = ESC_SEQ;
                }
                else if (c == '\n') {
                    return NULL; // Invalid string character
                }
                else {
                    break; // Continue in STRING state
                }
                break;
            case ESC_SEQ:
                if (isescseq(c)) {
                    lexer->state = STRING;
                }
                else if (c == 'x') {
                    lexer->state = HEX_NUM;
                }
                else {
                    return NULL; // Invalid escape sequence
                }
                break;
            case HEX_NUM:
                if (ishexnum(c) && hex_cnt < 2) {
                    hex_cnt++;
                }
                else if (hex_cnt == 2) {
                    lexer->state = STRING;
                }
                else {
                    return NULL;
                }
                break;
            case UNDERSCORE:
                if (c == '_') {
                    break; // Countinue in UNDERSCORE state
                }
                else if (isalnum(c)) {
                    lexer->state = ID_OR_KEY;
                }
                else {
                    return NULL; // Probably cant have multiple '_' (fix later?)
                }
                break;
            case ZERO:
                if (c == '.') {
                    lexer->state = FLOAT;
                }
                else if (isspace(c) || isvalid(c, lexer->ascii_l_table)) { // is dot valid?
                    lexer->state = START;
                    ungetc(c, lexer->src); // Put c back to stream
                    return create_token(TOKEN_INTEGER, 1, "0");
                }
                else {
                    return NULL; // Invalid number
                }
                break;
            case INTEGER:
                if (c == '.') {
                    lexer->state = FLOAT;
                }
                else if (c == 'e' || c == 'E') {
                    lexer->state = EXPONENT;
                }
                else if (c >= '0' && c <= '9') {
                    break; // Continue in INTEGER state 
                }
                else if (isspace(c) || isvalid(c, lexer->ascii_l_table)) { // is dot valid?
                    lexer->state = START;
                    ungetc(c, lexer->src); // Put c back to stream
                    return create_token(TOKEN_INTEGER, 0, NULL);
                }
                else {
                    return NULL; // Invalid integer
                }
                break;
            case FLOAT:
                if (c >= '0' && c <= '9') {
                    break; // Continue in FLOAT state
                }
                else if (c == 'e' || c == 'E') {
                    lexer->state = EXPONENT;
                }
                else if (isspace(c) || isvalid(c, lexer->ascii_l_table)) { // is dot valid?
                    lexer->state = START;
                    ungetc(c, lexer->src);
                    return create_token(TOKEN_FLOAT, 0, NULL);
                }
                else {
                    return NULL; // Invalid float
                }
                break;
            case EXPONENT:
                if (c == '+' || c == '-') {
                    lexer->state = SIGN;
                }
                else if (c >= '0' && c <= '9') { // what about chained exponents ?
                    lexer->state = EXPONENT_NUM;
                }
                else {
                    return NULL;
                }
                break;
            case SIGN:
                if (c >= '0' || c <= '9') {
                    lexer->state = EXPONENT_NUM;
                }
                else {
                    return NULL;
                }
                break;
            case EXPONENT_NUM:
                if (c >= '0' && c <= '9') {
                    break; // Continue in EXPONENT_NUM state
                }
                else if (isspace(c) || isvalid(c, lexer->ascii_l_table)) {
                    lexer->state = START;
                    ungetc(c, lexer->src);
                    return create_token(TOKEN_FLOAT, 0, NULL);
                }
                else {
                    return NULL;
                }
                break;
            case FWD_SLASH:
                if (c == '/') {
                    lexer->state = COMMENT;
                }
                else {
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
                }
                else {
                    return NULL;
                }
                break;
            case KEYWORD:
                if (isalnum(c)) {
                    break; // Continue in KEYWORD state
                }
                else if (isspace(c) || isvalid(c, lexer->ascii_l_table)) {
                    lexer->state = START;
                    ungetc(c, lexer->src);
                    return create_token(INVALID, 0, NULL);
                }
                else {
                    return NULL;
                }
                break;
            case L_SQ_BRACKET:
                if (c == ']') {
                    lexer->state = R_SQ_BRACKET;
                    return create_token(TOKENS_SLICE, 0, NULL);
                }
                else {
                    return NULL;
                }
                break;
            case R_SQ_BRACKET:
                if (isalpha(c)) {
                    lexer->state = KEYWORD;
                }
                else {
                    return NULL;
                }
                break;
        }
    }
    return NULL;
}

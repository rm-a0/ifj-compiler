/**
 * @file lexer.c
 * @brief Contains definitons of fucntions declared in lexer.c
 * @authors Michal Repcik (xrepcim00)
*/
#include <ctype.h>

#include "lexer.h"
#include "ascii_lookup.h"
#include "keyword_htab.h"
#include "error.h"


int init_lexer(Lexer* lexer, const char* filename) {
    if (lexer == NULL) {
        return -1;
    }

    // initialize src
    if (filename == NULL) {
        lexer->src = stdin;
    }
    else {
        lexer->src = fopen(filename, "r");
        if (lexer->src == NULL) {
            fprintf(stderr, "Error opening file\n");
            return -1;
        }
    }

    // initialize ascii lookup table
    init_lookup_table(lexer->ascii_l_table);

    // initialize predefined tokens
    lexer->predefined_tokens = NULL;

    lexer->state = START;

    return 0;
}

Token* get_token(Lexer* lexer) {
    int c;

    while ((c = fgetc(lexer->src)) != EOF) {
        switch (lexer->state) {
            case START:
                switch (c) {
                    case ' ':
                    case '\t':
                    case '\n':
                        break;
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
                        // return ? token
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
                        else if (lexer->ascii_l_table[c] != INVALID) {
                            // precreate tokens for single characters
                            return create_token(lexer->ascii_l_table[c], 1, (char)c);
                        }
                        else {
                            // If we encounter unexpected character
                            exit(LEXICAL_ERROR);
                        }
                        break;
                }
                break;
            case ID_OR_KEY:
                if (isalnum(c) || isalpha(c) || c == '_') {
                    break;
                }
                else if (isspace(c)) {
                    // return either id or keyword
                    // implement hashtable for keywords
                    lexer->state = START;
                }
                else {
                    // invalid identifier
                    exit(LEXICAL_ERROR);
                }
                break;
            case STRING:
                break;
        }

    }
    return NULL;
}

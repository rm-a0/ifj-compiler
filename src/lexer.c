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
                if (isspace(c)) {
                    break;
                }
                else if (lexer->ascii_l_table[c] != INVALID) {
                    // precreate tokens for single characters instead of 
                    // allocating them using malloc
                    return create_token(lexer->ascii_l_table[c], 1, (char)c);
                }
                else if (c == '/') {
                    lexer->state = FWD_SLASH;
                }
                else if (c == '"') {
                    lexer->state = STRING;
                }
                else if (c == '0') {
                    lexer->state = ZERO;
                }
                else if (c == '?') {
                    lexer->state = Q_MARK;
                }
                else if (c == '_') {
                    lexer->state = UNDERSCORE;
                }
                else if (c == '[') {
                    lexer->state = L_SQ_BRACKET;
                }
                break;
        }

    }
    return NULL;
}

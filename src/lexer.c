/**
 * @file lexer.c
 * @brief Contains definitons of fucntions declared in lexer.c
 * @authors Michal Repcik (xrepcim00)
*/

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

    return 0;
}

Token* get_token(Lexer* lexer) {
    return NULL;
}

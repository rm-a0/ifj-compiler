#include "lexer.h"
#include "error.h"

int parse_tokens(Lexer* lexer) {
    Token* cur_tok = get_token(lexer);
    Token* next_tok = NULL;

    if (cur_tok == NULL) {
        return NO_ERROR; // idk (empty code aallowed ?)
    }

    while ((next_tok = get_token(lexer)) != NULL) {
        // do something (implement LL grammar and table)
    }

    return NO_ERROR;
}
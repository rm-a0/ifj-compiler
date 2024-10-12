/**
 * @file ascii_lookup.c
 * @brief File containing implementation of functions
 * for creation and manipulation of ascii lookup tables
 * @author Michal Repcik (xrepcim00)
*/

#include <stdio.h>
#include <string.h>

#include "ascii_lookup.h"
#include "token.h"

void init_lookup_table(LookupTable table) {
    memset(table, INVALID, sizeof(LookupTable));

    table['('] = TOKEN_L_PAREN;
    table[')'] = TOKEN_R_PAREN;
    table['{'] = TOKEN_L_BRACE;
    table['}'] = TOKEN_R_BRACE;
    table['.'] = TOKEN_DOT;
    table[','] = TOKEN_COMA;
    table[':'] = TOKEN_COLON;
    table[';'] = TOKEN_SEMICOLON;
    table['|'] = TOKEN_PIPE;
    table['!'] = TOKEN_EXCM;
    table['='] = TOKEN_ASSIGN;
    table['/'] = TOKEN_DIV;
    table['*'] = TOKEN_MULT;
    table['-'] = TOKEN_MINUS;
    table['+'] = TOKEN_PLUS;
    table['<'] = TOKEN_LESS;
    table['>'] = TOKEN_GREATER;
}

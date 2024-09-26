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
    memset(table, INVALID, sizeof(table));

    table['('] = TOKEN_L_PAREN;
    table[')'] = TOKEN_R_PAREN;
    table['{'] = TOKEN_L_BRACE;
    table['}'] = TOKEN_R_BRACE;
    table['.'] = TOKEN_DOT;
    table[','] = TOKEN_COMA;
    table[':'] = TOKEN_COLON;
    table[';'] = TOKEN_SEMICOLON;
    table['|'] = TOKEN_PIPE;
}

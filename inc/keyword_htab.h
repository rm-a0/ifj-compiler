/**
 * @file keyword_htab.h
 * @brief Header file containing implementation of hash table for keyword tokens
 * @authors Michal Repcik (xrepcim00)
*/

#ifndef KEYWORD_HTAB_H
#define KEYWORD_HTAB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"

#define OPTIMAL_SIZE 31

typedef struct {
    const char* keyword;
    TokenType keyword_token;
} KeywordHtabItem;

typedef struct {
    int size;
    KeywordHtabItem* items;
} KeywordHtab;

KeywordHtab* create_keyword_htab(int size);

void destroy_keyword_htab(KeywordHtab* htab);

void init_keyword_htab(KeywordHtab* htab);

int insert_keyword_htab(KeywordHtab* htab, const char* keyword, TokenType token);

inline unsigned long hash_keyword(const char* str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

inline TokenType find_keyword(KeywordHtab* htab, const char* keyword) {
    unsigned long hash = hash_keyword(keyword);
    int idx = hash % htab->size;

    while (htab->items[idx].keyword != NULL) {
        if (strcmp(htab->items[idx].keyword, keyword) == 0) {
            return htab->items[idx].keyword_token; // Return the token type
        }
        idx = (idx + 1) % htab->size; // Linear probing
    }

    return TOKEN_IDENTIFIER; // Return IDENTIFIER token if not found
}

#endif // KEYWORD_HTAB_H
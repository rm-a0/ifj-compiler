/**
 * @file keyword_htab.h
 * @brief Header file containing implementation of hash table for keyword tokens
 * @authors Michal Repcik (xrepcim00)
*/

#ifndef KEYWORD_HTAB_H
#define KEYWORD_HTAB_H

#include "token.h"

typedef struct{
    int size;
    KeywordHtabItem* items;
} KeywordHtab;

typedef struct {
    char* keyword;
    TokenType keyword_token;
} KeywordHtabItem;

void create_keyword_htab();

void init_keyword_htab();

void hash_keyword();

#endif // KEYWORD_HTAB_H
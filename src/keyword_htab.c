/**
 * @file keyword_htab.c
 * @brief File contatining definitions of functions for hash table manipulation
 * @authors Michal Repcik (xrepcim00)
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "keyword_htab.h"

KeywordHtab* create_keyword_htab(int size) {
    KeywordHtab* htab = malloc(sizeof(KeywordHtab));
    if (htab == NULL) {
        fprintf(stderr, "Memory allocation for keyword hash table failed\n");
        return NULL;
    }

    htab->size = size;
    htab->items = calloc(size, sizeof(KeywordHtabItem));
    if (htab->items == NULL) {
        free(htab);
        fprintf(stderr, "Memory allocation for keyword hash table items failed\n");
        return NULL;
    }
    return htab;
}

void destroy_keyword_htab(KeywordHtab* htab) {
    if (htab == NULL) {
        return;
    }
    for (int i = 0; i < htab->size; i++) {
        free((char*)htab->items[i].keyword);
    }

    free(htab->items);
    free(htab);
}

int insert_keyword_htab(KeywordHtab* htab, const char* keyword, TokenType token) {
    unsigned long hash = hash_keyword(keyword);
    int idx = hash % htab->size;

    htab->items[idx].keyword = strdup(keyword);
    if (htab->items[idx].keyword == NULL) {
        fprintf(stderr, "Memory allocation for keyword failed\n");
        return -1;
    }

    htab->items[idx].keyword_token = token;
    return 0;
}

void init_keyword_htab(KeywordHtab* htab) {
    if (htab == NULL || htab->items == NULL) {
        return;
    }

    insert_keyword_htab(htab, "const", TOKEN_CONST);
    insert_keyword_htab(htab, "else", TOKEN_ELSE);
    insert_keyword_htab(htab, "fn", TOKEN_FN);
    insert_keyword_htab(htab, "if", TOKEN_IF);
    insert_keyword_htab(htab, "i32", TOKEN_I32);
    insert_keyword_htab(htab, "f64", TOKEN_F64);
    insert_keyword_htab(htab, "u8", TOKEN_U8);
    insert_keyword_htab(htab, "null", TOKEN_NULL);
    insert_keyword_htab(htab, "pub", TOKEN_PUB);
    insert_keyword_htab(htab, "return", TOKEN_RETURN);
    insert_keyword_htab(htab, "var", TOKEN_VAR);
    insert_keyword_htab(htab, "void", TOKEN_VOID);
    insert_keyword_htab(htab, "while", TOKEN_WHILE);
}

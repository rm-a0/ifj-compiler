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

#define OPTIMAL_SIZE 113 ///< Optimal size to ensure perfect hashing and O(1) complexity for keywords.

/**
 * @struct KeywordHtabItem
 * @brief Struct containing a keyword and its corresponding token.
 * 
 * This structure represents a single entry in the keyword hash table,
 * mapping a valid keyword to its associated token type.
*/
typedef struct {
    const char* keyword;         ///< Valid keyword string.
    TokenType keyword_token;     ///< Token for the given keyword.
} KeywordHtabItem;

/**
 * @struct KeywordHtab
 * @brief Struct containing items and size of the hash table.
 * 
 * This structure represents the keyword hash table, which stores 
 * keywords and their corresponding token types for efficient lookup.
*/
typedef struct {
    int size;                    ///< Number of buckets (total number of items).
    KeywordHtabItem* items;      ///< Pointer to the array of items in the hash table.
} KeywordHtab;


/**
 * @fn KeywordHtab* create_keyword_htab(int size)
 * @brief Allocates memory for hash table, items and initializes size.
 * 
 * @param[in] size Number of items
 * @return Returns pointer to KeywordHtab
*/
KeywordHtab* create_keyword_htab(int size);

/**
 * @fn void destroy_keyword_htab(KeywordHtab* htab)
 * @brief Frees all memory previoulsy allocated by create_keyword_htab function.
 * 
 * @param[in, out] htab pointer to a hash table
 * @return void
*/
void destroy_keyword_htab(KeywordHtab* htab);

/**
 * @fn void init_keyword_htab(KeywordHtab* htab)
 * @brief Initializes hash table with all valid keywords.
 * 
 * @param[in, out] htab pointer to a hash table
 * @return void
*/
void init_keyword_htab(KeywordHtab* htab);

/**
 * @fn int insert_keyword_htab(KeywordHtab* htab, const char* keyword, TokenType token)
 * @brief Inserts keyword with coresponding token into hash table. 
 * 
 * @param[in, out] htab pointer to a hash table
 * @param keyword Keyword to be inserted into hash table (used as key)
 * @param token Tpye of token that matches the keyword
 * @return Returns 0 if everything went succesfully, otherwise returns -1
 * 
 * @note Memory is allocated in this function using strdup and potentially malloc (memory is freed
 * in destroy_keyword_htab function). This funcition does not take into account already existing items,
 * since main goal of the hash table is to ensure 0(1) complexity for keyword lookups. To prevent overwriting, 
 * increase OPTIMAL_SIZE constant or use different algorithm for hash_keyword function.
*/
int insert_keyword_htab(KeywordHtab* htab, const char* keyword, TokenType token);

/**
 * @fn inline unsigned long hash_keyword(const char* str)
 * @brief Implementation of djb2 hashing algorithm.
 * 
 * @param str String used as key for hashing
 * @return Returns hash as unsigned long
 * 
 * @note Function is inlined to prevent function call overhead.
*/
inline unsigned long hash_keyword(const char* str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

/**
 * @fn inline TokenType find_keyword(KeywordHtab* htab, const char* keyword)
 * @brief Tries to find given keyword inside hash table.
 * 
 * @param keyword Keyword to be found
 * @return Returns either corresponding token or token for identifiers
 * 
 * @note Function is inlined to prevent function call overhead.
*/
inline TokenType find_keyword(KeywordHtab* htab, const char* keyword) {
    unsigned long hash = hash_keyword(keyword);
    int idx = hash % htab->size;

    if (htab->items[idx].keyword != NULL) {
        if (strcmp(htab->items[idx].keyword, keyword) == 0) {
            return htab->items[idx].keyword_token;
        }
    }
    return TOKEN_IDENTIFIER;
}

#endif // KEYWORD_HTAB_H

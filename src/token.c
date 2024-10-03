/**
 * @file token.c
 * @brief Contains definitions of functions declared in token.h file
 * @authors Michal Repcik (xrepcim00)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "error.h"

Token* create_token(TokenType type, size_t length, const char* value) {
    Token* token = malloc(sizeof(Token));
    if (!token) {
        fprintf(stderr, "Memory allocation for token failed\n");
        exit(INTERNAL_ERROR);
    }

    token->token_type = type;

    if (value != NULL) {
        token->value = malloc(length + 1);
        if (!token->value) {
            free(token);
            fprintf(stderr, "Memory allocation for token value failed\n");
            exit(INTERNAL_ERROR);
        }
        strncpy(token->value, value, length);
        token->value[length] = '\0';
    }
    else {
        token->value = NULL; 
    }

    return token;
}

void free_token(Token *token) {
    if (token) {
        if (token->value) {
            free(token->value);
        }
        free(token);
    }
}

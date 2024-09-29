/**
 * @file main.c
 * @brief Main program
 * @authors Michal Repcik (xrepcim00)
 */
#include <stdlib.h>

#include "lexer.h"
#include "error.h"

FILE* process_file(int argc, char**  argv) {
    FILE *fp = NULL;
    if (argc > 2) {
        fprintf(stderr, "Only one argument suppported\n");
        exit(INTERNAL_ERROR);
    }
    else if (argc == 2) {
        fp = fopen(argv[1], "r");
        if (fp == NULL) {
            fprintf(stderr, "Failed to read from the file\n");
            exit(INTERNAL_ERROR);
        }
    }
    else {
        fp = stdin;
    }
    return fp;
}

int main(int argc, char** argv) {
    Lexer lexer;

    FILE* fp;
    fp = process_file(argc, argv); 

    if (init_lexer(&lexer, fp) != 0) {
        exit(INTERNAL_ERROR);
    }

    Token* token;
    while((token = get_token(&lexer)) != NULL) {
        fprintf(stderr, "lessgo");
        free_token(token);
    }

    return NO_ERROR; 
}
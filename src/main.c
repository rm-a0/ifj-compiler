/**
 * @file main.c
 * @brief Main program
 * @authors Michal Repcik (xrepcim00)
 */

#include "lexer.h"
#include "error.h"

int main(int argc, char** argv) {
    Lexer lexer;
    
    if (init_lexer(&lexer, argv[1]) != 0) {
        exit(INTERNAL_ERROR);
    }

    return NO_ERROR; 
}
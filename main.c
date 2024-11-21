/**
 * @file main.c
 * @brief Main program
 * @authors Michal Repcik (xrepcim00)
*/
#include <stdlib.h>
#include <time.h>

#include "lexer.h"
#include "parser.h"
#include "error.h"
#include "semantic_analysis.h"
#include "symtable.h"
#include "stack.h"
#include "ast_utils.h"

#define RESET   "\x1b[0m"
#define GREEN   "\x1b[32m"
#define BOLD_GREEN "\x1b[1;32m"

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
    clock_t start;
    start = clock();

    Lexer lexer;
    FILE* fp;
    fp = process_file(argc, argv); 

    if (init_lexer(&lexer, fp) != 0) {
        exit(INTERNAL_ERROR);
    }

    ASTNode* root = parse_tokens(&lexer);
    if (root == NULL) {
        destroy_lexer(&lexer);
        exit(error_tracker);
    }
    else {
        fprintf(stderr, "Syntax is correct\n");
        // free_ast_node(root);
    }
    print_ast_node(root, 0);

    destroy_lexer(&lexer);

    SymbolTable *global_table = init_symbol_table(); // Initialize global table
    ScopeStack *local_stack = NULL;                  // No local scope initially

    semantic_analysis(root, global_table, local_stack);
    free_symbol_table(global_table);

    free_ast_node(root);
    fprintf(stderr, "Time: %.3g\n", (double)(clock()-start)/CLOCKS_PER_SEC);

    return NO_ERROR; 
}

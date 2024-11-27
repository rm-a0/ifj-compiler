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
#include "generator.h"

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

    ASTNode* root = parse_tokens(&lexer);
    if (root == NULL) {
        destroy_lexer(&lexer);
        exit(error_tracker);
    }


    SymbolTable *global_table = init_symbol_table(); // Initialize global table
    ScopeStack *local_stack = NULL;                  // No local scope initially

    semantic_analysis(root, global_table, local_stack);
    free_symbol_table(global_table);

    //print_ast_node(root, 0);

    generate_code(root);

    destroy_lexer(&lexer);
    free_ast_node(root);

    return NO_ERROR; 
}

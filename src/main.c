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

int print_token(Lexer* lexer) {
    const char* tok_name[] = {
    "INVALID",
    "TOKEN_CONST",
    "TOKEN_ELSE",
    "TOKEN_FN",
    "TOKEN_IF",
    "TOKEN_I32",
    "TOKEN_F64",
    "TOKEN_NULL",
    "TOKEN_PUB",
    "TOKEN_RETURN",
    "TOKEN_U8",
    "TOKEN_VAR",
    "TOKEN_VOID",
    "TOKEN_WHILE",
    "TOKEN_IDENTIFIER",
    "TOKEN_STRING",
    "TOKEN_INTEGER",
    "TOKEN_FLOAT",
    "TOKENS_SLICE",
    "TOKEN_L_PAREN",
    "TOKEN_R_PAREN",
    "TOKEN_L_BRACE",
    "TOKEN_R_BRACE",
    "TOKEN_DOT",
    "TOKEN_COMA",
    "TOKEN_COLON",
    "TOKEN_SEMICOLON",
    "TOKEN_PIPE",
    "TOKEN_PLUS",
    "TOKEN_MINUS",
    "TOKEN_MULT",
    "TOKEN_DIV",
    "TOKEN_ASSIGN",
    "TOKEN_Q_MARK",
    "TOKEN_LESS",
    "TOKEN_GREATER",
    "TOKEN_EXCM",
    "TOKEN_LESS_EQU",
    "TOKEN_GREATER_EQU",
    "TOKEN_NOT_EQU",
    "TOKEN_EQU",
    "TOKEN_IMPORT",
    };

    Token* token;
    while((token = get_token(lexer)) != NULL) {
        fprintf(stdout, "Token type: ");
        fprintf(stdout, BOLD_GREEN "%-25s" RESET, tok_name[token->token_type]);

        fprintf(stdout, "Token value: ");
        fprintf(stdout, BOLD_GREEN " %s\n" RESET, token->value);
        free_token(token);
    }

    return NO_ERROR; 
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

    destroy_lexer(&lexer);

    SymbolTable *global_table = init_symbol_table(); // Initialize global table
    ScopeStack *local_stack = NULL;                  // No local scope initially

    semantic_analysis(root, global_table, local_stack);
    free_symbol_table(global_table);

    free_ast_node(root);
    fprintf(stderr, "Time: %.3g\n", (double)(clock()-start)/CLOCKS_PER_SEC);

    return NO_ERROR; 
}

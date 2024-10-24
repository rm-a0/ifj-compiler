/** 
 * @file parser.c
 * @brief Implementation of a parser (syntax analysis)
 * @authors Michal Repcik (xrepcim00)
*/
#include "error.h"
#include "parser.h"

void advance_token(Token** token, Lexer* lexer) {
    if (token && *token) {
        free_token(*token);
    }
    // delete later
    else {
        fprintf(stderr, "Token is null in advance token function");
    }
    *token = get_token(lexer);
}

int check_token(Token* token, TokenType expected_type, const char* expected_value) {
    if (token == NULL) {
        return 0;   // Token is invalid
    }

    if (token->token_type != expected_type) {
        return 0;   // Type is different than expected
    }

    if (expected_value != NULL) {
        if (token->value == NULL || strcmp(token->value, expected_value) != 0) {
            return 0;   // Token value is different from expected value
        }
    }

    return 1;   // Return true
}

int parse_prolog(Lexer* lexer, Token** token) {
    advance_token(token, lexer);
    if (!check_token(*token, TOKEN_ASSIGN, NULL)) {
        return 0;
    }
    advance_token(token, lexer);
    if (!check_token(*token, TOKEN_IMPORT, NULL)) {
        return 0;
    }
    advance_token(token, lexer);
    if (!check_token(*token, TOKEN_L_PAREN, NULL)) {
        return 0;
    }
    advance_token(token, lexer);
    if (!check_token(*token, TOKEN_STRING, "ifj24.zig")) {
        return 0;
    }
    advance_token(token, lexer);
    if (!check_token(*token, TOKEN_R_PAREN, NULL)) {
        return 0;
    }
    advance_token(token, lexer);
    if (!check_token(*token, TOKEN_SEMICOLON, NULL)) {
        return 0;
    }
    return 1; // Return true
}

ASTNode* parse_const_decl(Lexer* lexer, Token* token) {
    return NULL;
}

ASTNode* parse_tokens(Lexer* lexer) {
    Token* token = get_token(lexer);
    if (token == NULL) {
        return NULL; // idk (empty code allowed ?)
    }

    ASTNode* program_node = create_program_node();  // Create root (program node)

    // Loop until the token is EOF or NULL
    while (!check_token(token, TOKEN_EOF, NULL)) {
        // CONST
        if (check_token(token, TOKEN_CONST, NULL)) {
            advance_token(&token, lexer);
            // PROLOG
            if (check_token(token, TOKEN_IDENTIFIER, "ifj")) {
                if (parse_prolog(lexer, &token)) {
                    program_node->Program.has_prolog = true;
                }
                else {
                    goto error;
                }
            }
            // CONST_DECL
            else if (check_token(token, TOKEN_IDENTIFIER, NULL)) {
                ASTNode* const_decl = parse_const_decl(lexer, token);
                if (const_decl != NULL) {
                    //append_declaration();
                }
                else {
                    goto error;
                }
            }
            // SYNTAX ERROR
            else {
                goto error;
            }
        }
        // VAR
        else if (check_token(token, TOKEN_VAR, NULL)) {
            // VAR_DECL
            // parse_var_decl();
        }
        // PUB
        else if (check_token(token, TOKEN_PUB, NULL)) {
            // FN_DECL
            // parse_fn_decl();

        }
        // SYNTAX ERROR
        else {
            goto error;
        }

        advance_token(&token, lexer);
    }

    free_token(token);
    return program_node;

    // Error handle for go to
    error:
        free_ast_node(program_node);
        free_token(token);
        return NULL;
}

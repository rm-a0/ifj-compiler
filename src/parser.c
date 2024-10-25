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

ASTNode* parse_const_decl(Lexer* lexer, Token** token) {
    // No need to get identifier token since it was checked before
    ASTNode* const_decl_node = create_const_decl_node(AST_UNSPECIFIED, (*token)->value);
    advance_token(token, lexer);
    // Optional data type
    if (check_token(*token, TOKEN_COLON, NULL)) {
        advance_token(token, lexer);
        if (*token == NULL) {
            return NULL;
        }
        // Based on token type assign data type of const decl node
        switch ((*token)->token_type) {
            case TOKEN_I32:
                const_decl_node->ConstDecl.data_type = AST_I32;
                break;
            case TOKEN_F64:
                const_decl_node->ConstDecl.data_type = AST_F64;
                break;
            case TOKEN_U8:
                const_decl_node->ConstDecl.data_type = AST_U8;
                break;
            default:
                return NULL; // Unexpected token type
                break;
        }
        advance_token(token, lexer); // Advance for assign check
    }
    if (!check_token(*token, TOKEN_ASSIGN, NULL)) {
        return NULL;
    }
    // Delete later, for now it accepts anything as expression untill it encounters semicolon
    while (!check_token(*token, TOKEN_SEMICOLON, NULL)) {
        advance_token(token, lexer);
    }
    //ASTNode* expression_node = parser_expression(lexer, token);

    return const_decl_node;
}

ASTNode* parse_var_decl(Lexer* lexer, Token** token) {
    advance_token(token, lexer); // get identifier
    if (!check_token(*token, TOKEN_IDENTIFIER, NULL)) {
        return NULL;
    }
    ASTNode* var_decl_node = create_var_decl_node(AST_UNSPECIFIED, (*token)->value);
    advance_token(token, lexer);
    // Optional data type
    if (check_token(*token, TOKEN_COLON, NULL)) {
        advance_token(token, lexer);
        if (*token == NULL) {
            return NULL;
        }
        // Based on token type assign data type of const decl node
        switch ((*token)->token_type) {
            case TOKEN_I32:
                var_decl_node->ConstDecl.data_type = AST_I32;
                break;
            case TOKEN_F64:
                var_decl_node->ConstDecl.data_type = AST_F64;
                break;
            case TOKEN_U8:
                var_decl_node->ConstDecl.data_type = AST_U8;
                break;
            default:
                return NULL; // Unexpected token type
                break;
        }
        advance_token(token, lexer); // Advance for assign check
    }
    if (!check_token(*token, TOKEN_ASSIGN, NULL)) {
        return NULL;
    }
    // Delete later, for now it accepts anything as expression untill it encounters semicolon
    while (!check_token(*token, TOKEN_SEMICOLON, NULL)) {
        advance_token(token, lexer);
    }
    //ASTNode* expression_node = parser_expression(lexer, token);

    return var_decl_node;
}

ASTNode* parse_tokens(Lexer* lexer) {
    Token* token = get_token(lexer);
    if (token == NULL) {
        return NULL; // idk (empty code allowed ?)
    }

    ASTNode* program_node = create_program_node();  // Create root (program node)

    // Loop until the token is EOF
    while (!check_token(token, TOKEN_EOF, NULL)) {
        // CONST
        if (check_token(token, TOKEN_CONST, NULL)) {
            advance_token(&token, lexer);
            // PROLOG
            if (check_token(token, TOKEN_IDENTIFIER, "ifj")) {
                // Check if prolog is valid
                if (parse_prolog(lexer, &token)) {
                    program_node->Program.has_prolog = true;
                }
                else {
                    goto error;
                }
            }
            // CONST_DECL
            else if (check_token(token, TOKEN_IDENTIFIER, NULL)) {
                // Check if constant declaration is valid
                ASTNode* const_decl = parse_const_decl(lexer, &token);
                if (const_decl != NULL) {
                    // Append variable declaration into program node array
                    if (append_decl_to_prog(program_node, const_decl) != 0) {
                        goto error;
                    }
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
        // VAR_DECL
        else if (check_token(token, TOKEN_VAR, NULL)) {
            // Check if variable declaration is valid
            ASTNode* var_decl = parse_var_decl(lexer, &token);
            if (var_decl != NULL) {
                // Append variable declaration into program node array
                if (append_decl_to_prog(program_node, var_decl) != 0) {
                    goto error;
                }
            }
            else {
                goto error;
            }
        }
        // PUB
        else if (check_token(token, TOKEN_PUB, NULL)) {
            // FN_DECL
            //ASTNode* fn_decl = parse_fn_decl(lexer, &token);

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

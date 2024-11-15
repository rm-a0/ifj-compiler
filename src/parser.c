/** 
 * @file parser.c
 * @brief Implementation of a parser (syntax analysis)
 * @authors Michal Repcik (xrepcim00)
*/
#include "error.h"
#include "parser.h"

const char* tok_name[] = {
    "INVALID",              ///< Constant for invalid tokens.
    "TOKEN_EOF",            ///< Constant for end of file tokens.
    "TOKEN_CONST",          ///< Constant for 'const' keyword.
    "TOKEN_ELSE",           ///< Constant for 'else' keyword.
    "TOKEN_FN",             ///< Constant for 'fn' keyword.
    "TOKEN_IF",             ///< Constant for 'if' keyword.
    "TOKEN_I32",            ///< Constant for 'i32' keyword.
    "TOKEN_F64",            ///< Constant for 'f64' keyword.
    "TOKEN_NULL",           ///< Constant for 'null' keyword.
    "TOKEN_PUB",            ///< Constant for 'pub' keyword.
    "TOKEN_RETURN",         ///< Constant for 'return' keyword.
    "TOKEN_U8",             ///< Constant for 'u8' keyword.
    "TOKEN_VAR",            ///< Constant for 'var' keyword.
    "TOKEN_VOID",           ///< Constant for 'void' keyword.
    "TOKEN_WHILE",          ///< Constant for 'while' keyword.
    "TOKEN_IDENTIFIER",     ///< Constant for identifier.
    "TOKEN_STRING",         ///< Constant for string.
    "TOKEN_INTEGER",        ///< Constant for integer (whole number).
    "TOKEN_FLOAT",          ///< Constant for float (decimal number).
    "TOKEN_SLICE",          ///< Constant for slice '[]'.
    "TOKEN_L_PAREN",        ///< Constant for '(' character.
    "TOKEN_R_PAREN",        ///< Constant for ')' character.
    "TOKEN_L_BRACE",        ///< Constant for '{' character.
    "TOKEN_R_BRACE",        ///< Constant for '}' character.
    "TOKEN_DOT",            ///< Constant for '.' character.
    "TOKEN_COMMA",          ///< Constant for ',' character.
    "TOKEN_COLON",          ///< Constant for ':' character.
    "TOKEN_SEMICOLON",      ///< Constant for ';' character.
    "TOKEN_PIPE",           ///< Constant for '|' character.
    "TOKEN_PLUS",           ///< Constant for '+' character.
    "TOKEN_MINUS",          ///< Constant for '-' character.
    "TOKEN_MULT",           ///< Constant for '*' character.
    "TOKEN_DIV",            ///< Constant for '/' character.
    "TOKEN_ASSIGN",         ///< Constant for '=' character.
    "TOKEN_Q_MARK",         ///< Constant for '?' character.
    "TOKEN_LESS",           ///< Constant for '<' character.
    "TOKEN_GREATER",        ///< Constant for '>' character.
    "TOKEN_EXCM",           ///< Constant for '!' character.
    "TOKEN_LESS_EQU",       ///< Constant for '<=' operator.
    "TOKEN_GREATER_EQU",    ///< Constant for '>=' operator.
    "TOKEN_NOT_EQU",        ///< Constant for '!=' operator.
    "TOKEN_EQU",            ///< Constant for '==' operator.
    "TOKEN_IMPORT"          ///< Constant for '@import' directive.
};

void advance_token(Token** token, Lexer* lexer) {
    if (token && *token) {
        free_token(*token);
    }
    *token = get_token(lexer);
    if (*token == NULL) {
        set_error(LEXICAL_ERROR);
    }
    else {
        // delete later
        printf("TokenType: %s\n", tok_name[(*token)->token_type]);
    }
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
    if (!check_token(*token, TOKEN_CONST, NULL)) {
        return 0;
    }
    advance_token(token, lexer);
    if (!check_token(*token, TOKEN_IDENTIFIER, "ifj")) {
        return 0;
    }
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

int parse_element_bind(Lexer* lexer, Token** token, ASTNode* node) {
    advance_token(token, lexer);
    // Element bind is not there return success
    if (!check_token(*token, TOKEN_PIPE, NULL)) {
        return 0;
    }
    advance_token(token, lexer);
    // Incorrect element bind sequence
    if (!check_token(*token, TOKEN_IDENTIFIER, NULL)) {
        return 1;
    }
    // Based on node type assign element bind
    switch (node->type) {
        case AST_WHILE: {
            node->WhileCycle.element_bind = strdup((*token)->value);
            if (node->WhileCycle.element_bind == NULL) {
                set_error(INTERNAL_ERROR);
                return 1;
            }
        }
            break;
        case AST_IF_ELSE: {
            node->IfElse.element_bind = strdup((*token)->value);
            if (node->IfElse.element_bind == NULL) {
                set_error(INTERNAL_ERROR);
                return 1;
            }
        }
            break;    
        default:
            set_error(INTERNAL_ERROR);
            return 1; 
            break;
    }

    advance_token(token, lexer);
    if (!check_token(*token, TOKEN_PIPE, NULL)) {
        return 1;
    }
    // Advance token to get next token before checking for block
    advance_token(token, lexer);
    // Parse succeeded
    return 0;
}

ASTNode* parse_const_decl(Lexer* lexer, Token** token) {
    advance_token(token, lexer);
    if (!check_token(*token, TOKEN_IDENTIFIER, NULL)) {
        return NULL;
    }
    ASTNode* const_decl_node = create_const_decl_node(AST_UNSPECIFIED, (*token)->value);
    if (const_decl_node == NULL) {
        return NULL;
    }
    advance_token(token, lexer);
    // Optional data type
    if (check_token(*token, TOKEN_COLON, NULL)) {
        advance_token(token, lexer);

        // Check for question mark (nullable data type)
        if (check_token(*token, TOKEN_Q_MARK, NULL)) {
            const_decl_node->ConstDecl.nullable = true;
            advance_token(token, lexer);
        }

        if (token == NULL || *token == NULL) {
            free_ast_node(const_decl_node);
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
            case TOKEN_SLICE:
                advance_token(token, lexer);
                if (!check_token(*token, TOKEN_U8, NULL)) {
                    free_ast_node(const_decl_node);
                    return NULL;
                }
                const_decl_node->ConstDecl.data_type = AST_SLICE;
                break;
            default:
                free_ast_node(const_decl_node);
                return NULL; // Unexpected token type
                break;
        }
        advance_token(token, lexer); // Advance for assign check
    }
    if (!check_token(*token, TOKEN_ASSIGN, NULL)) {
        free_ast_node(const_decl_node);
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
    // Create variable declaration node (mmust be freed when returnin NULL)
    ASTNode* var_decl_node = create_var_decl_node(AST_UNSPECIFIED, (*token)->value);
    if (var_decl_node == NULL) {
        return NULL;
    }
    advance_token(token, lexer);
    // Optional data type
    if (check_token(*token, TOKEN_COLON, NULL)) {
        advance_token(token, lexer);

        // Check for nullable data type
        if (check_token(*token, TOKEN_Q_MARK, NULL)) {
            var_decl_node->VarDecl.nullable = true;
            advance_token(token, lexer);
        }

        if (token == NULL || *token == NULL) {
            free_ast_node(var_decl_node);
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
            case TOKEN_SLICE:
                advance_token(token, lexer);
                if (!check_token(*token, TOKEN_U8, NULL)) {
                    free_ast_node(var_decl_node);
                    return NULL;
                }
                var_decl_node->ConstDecl.data_type = AST_SLICE;
                break;
            default:
                free_ast_node(var_decl_node);
                return NULL; // Unexpected token type
                break;
        }
        advance_token(token, lexer); // Advance for assign check
    }
    if (!check_token(*token, TOKEN_ASSIGN, NULL)) {
        free_ast_node(var_decl_node);
        return NULL;
    }
    // Delete later, for now it accepts anything as expression untill it encounters semicolon
    while (!check_token(*token, TOKEN_SEMICOLON, NULL)) {
        advance_token(token, lexer);
    }
    // ASTNode* expression_node = parser_expression(lexer, token);

    return var_decl_node;
}

ASTNode* parse_fn_params(Lexer* lexer, Token** token) {
    if (!check_token(*token, TOKEN_IDENTIFIER, NULL)) {
        return NULL;
    }
    ASTNode* parameter = create_param_node(AST_UNSPECIFIED, (*token)->value);
    if (parameter == NULL) {
        return NULL;
    }
    advance_token(token, lexer);
    if (!check_token(*token, TOKEN_COLON, NULL)) {
        free_ast_node(parameter);
        return NULL;
    }
    advance_token(token, lexer);

    // Check for nullable fn parameter
    if (check_token(*token, TOKEN_Q_MARK, NULL)) {
        parameter->Param.nullable = true;
        advance_token(token, lexer);
    }

    // Check for null pointer before acessing token type
    if (token == NULL || *token == NULL) {
        free_ast_node(parameter);
        return NULL;
    }
    switch ((*token)->token_type) {
        case TOKEN_I32:
            parameter->Param.data_type = AST_I32;
            break;
        case TOKEN_F64:
            parameter->Param.data_type = AST_F64;
            break;
        case TOKEN_U8:
            parameter->Param.data_type = AST_U8;
            break;
        case TOKEN_SLICE:
            advance_token(token, lexer);
            if (!check_token(*token, TOKEN_U8, NULL)) {
                free_ast_node(parameter);
                return NULL;
            }
            parameter->Param.data_type = AST_SLICE;
            break;
        default: // Unexpected token syntax error
            free_ast_node(parameter);
            return NULL;
            break;
    }
    advance_token(token, lexer);
    // Parenthesis after coma is alowed so we move to next
    if (check_token(*token, TOKEN_COMMA, NULL)) {
        advance_token(token, lexer);
    }
    // If its not parenthesis or coma return NULL
    else if (!check_token(*token, TOKEN_R_PAREN, NULL)) {
        free_ast_node(parameter);
        return NULL;
    }

    return parameter;
}

ASTNode* parse_if_else(Lexer* lexer, Token** token) {
    if (!check_token(*token, TOKEN_IF, NULL)) {
        return NULL;
    }
    advance_token(token, lexer);
    if (!check_token(*token, TOKEN_L_PAREN, NULL)) {
        return NULL;
    }
    while (!check_token(*token, TOKEN_R_PAREN, NULL)) {
        // replace with expression
        // ASTNode* expression_node = parse_expression
        advance_token(token, lexer);
    }

    // Create if else node
    ASTNode* if_else_node = create_if_node();
    if (if_else_node == NULL) {
        return NULL;
    }

    // Check element bind 
    if (parse_element_bind(lexer, token, if_else_node) != 0) {
        free_ast_node(if_else_node);
        return NULL;
    }

    // Parse if block
    ASTNode* if_block = parse_block(lexer, token);
    if (if_block == NULL) {
        free_ast_node(if_else_node);
        return NULL; 
    }
    // Add if block to if else node
    if_else_node->IfElse.if_block = if_block;

    // Check for else block
    advance_token(token, lexer);
    if (check_token(*token, TOKEN_ELSE, NULL)) {
        // Parse else block
        advance_token(token, lexer);
        ASTNode* else_block = parse_block(lexer, token);
        if (else_block == NULL) {
            free_ast_node(if_else_node);
            return NULL;
        }
        if_else_node->IfElse.else_block = else_block;
        // This is needed because we advanced to check for else before
        advance_token(token, lexer);
    }

    return if_else_node;
}

ASTNode* parse_while(Lexer* lexer, Token** token) {
    advance_token(token, lexer);
    if (!check_token(*token, TOKEN_L_PAREN, NULL)) {
        return NULL;
    }
    // Parse expression
    advance_token(token, lexer); // delete later?
    if (!check_token(*token, TOKEN_R_PAREN, NULL)) {
        return NULL;
    }
    // Create while node
    ASTNode* while_node = create_while_node();
    if (while_node == NULL) {
        return NULL;
    }
    // Parse element bind
    if (parse_element_bind(lexer, token, while_node) != 0) {
        free_ast_node(while_node);
        return NULL;
    }
    // Parse block node
    ASTNode* block_node = parse_block(lexer, token);
    if (block_node == NULL) {
        free_ast_node(while_node);
        return NULL;
    }
    while_node->WhileCycle.block = block_node;
    // while_node->WhileCycle.expression = expression;
    return while_node;
}

ASTNode* parse_fn_call(Lexer* lexer, Token** token, char* identifier) {
    ASTNode* fn_call = create_fn_call_node(identifier);
    if (fn_call == NULL) {
        return NULL;
    }
    // Parse fn args
    while (!(check_token(*token, TOKEN_R_PAREN, NULL))) {
        advance_token(token, lexer);
    }
    // We got ')' get next token anc check for ';'
    advance_token(token, lexer);
    if (!check_token(*token, TOKEN_SEMICOLON, NULL)) {
        free_ast_node(fn_call);
        return NULL;
    }
    return fn_call;
}

ASTNode* parse_builtin_fn_call(Lexer* lexer, Token** token, char* identifier) {
    if (strcmp(identifier, "ifj") != 0) {
        return NULL;
    }
    advance_token(token, lexer);
    if (!check_token(*token, TOKEN_IDENTIFIER, NULL)) {
        return NULL;
    }
    // Concat ifj, dot and new id
    int new_len = 3 + strlen((*token)->value) + 1 + 1; // 'ifj' + '.' + id + null terminator
    char* new_id = malloc(new_len*sizeof(char));
    if (new_id == NULL) {
        return NULL;
    }
    strcpy(new_id, identifier);
    strcat(new_id, ".");
    strcat(new_id, (*token)->value);
    
    ASTNode* builtin_fn_call = create_fn_call_node(new_id);
    free(new_id); // we dont need the new id anymore
    if (builtin_fn_call == NULL) {
        return NULL;
    }
    advance_token(token, lexer);
    if (!check_token(*token, TOKEN_L_PAREN, NULL)) {
        free(builtin_fn_call);
        return NULL;
    }
    // parse expression
    while (!check_token(*token, TOKEN_R_PAREN, NULL)) {
        advance_token(token, lexer);
    }
    advance_token(token, lexer);
    if (!check_token(*token, TOKEN_SEMICOLON, NULL)) {
        free_ast_node(builtin_fn_call);
        return NULL;
    }
    return builtin_fn_call;
}

ASTNode* parse_block(Lexer* lexer, Token** token) {
    if (!check_token(*token, TOKEN_L_BRACE, NULL)) {
        return NULL;
    }
    
    // Create block node
    ASTNode* block_node = create_block_node();
    if (block_node == NULL) {
        return NULL;
    }
    // Advance token before loop begins
    advance_token(token, lexer);
    // Loop untill right brace is encoutered
    while (!check_token(*token, TOKEN_R_BRACE, NULL)) {
        // Check for NULL pointer before switch
        if (*token == NULL) {
            return NULL;
        }
        switch ((*token)->token_type) {
            case TOKEN_IF: {
                // Parse if else block and append it to block node
                ASTNode* if_else_node = parse_if_else(lexer, token);
                if (if_else_node == NULL) {
                    free_ast_node(block_node);
                    return NULL;
                }
                if (append_node_to_block(block_node, if_else_node) != 0) {
                    free_ast_node(if_else_node);
                    free_ast_node(block_node);
                    return NULL;
                }
            }
                break;
            case TOKEN_WHILE: {
                ASTNode* while_node = parse_while(lexer, token);
                if (while_node == NULL) {
                    free_ast_node(block_node);
                    return NULL;
                }
                if (append_node_to_block(block_node, while_node) != 0) {
                    free_ast_node(while_node);
                    free_ast_node(block_node);
                    return NULL;
                }
                // While parsing ends on '}', we need to advance
                advance_token(token, lexer);
            }
                break;
            case TOKEN_CONST: {
                ASTNode* const_node = parse_const_decl(lexer, token);
                if (const_node == NULL) {
                    free_ast_node(block_node);
                    return NULL;
                }
                if (append_node_to_block(block_node, const_node) != 0) {
                    free_ast_node(const_node);
                    free_ast_node(block_node);
                    return NULL;
                }
                // Const parsing ends on ';', we need to advance
                advance_token(token, lexer);
            }
               break;
            case TOKEN_VAR: {
                // Dont need to advnace token like in const since its advanced within function
                ASTNode* var_node = parse_var_decl(lexer, token);
                if (var_node == NULL) {
                    free_ast_node(block_node);
                    return NULL;
                }
                if (append_node_to_block(block_node, var_node) != 0) {
                    free_ast_node(var_node);
                    free_ast_node(block_node);
                    return NULL;
                }
                // Var parsing ends on ';', we need to advance
                advance_token(token, lexer);
            }
               break;
            case TOKEN_IDENTIFIER: {
                // Save name of the identifier
                // What about built in fn?
                char* identifier = strdup((*token)->value);
                advance_token(token, lexer);
                if (check_token(*token, TOKEN_ASSIGN, NULL)) {
                    // parse expression
                }
                else if (check_token(*token, TOKEN_L_PAREN, NULL)) {
                    ASTNode* fn_call_node = parse_fn_call(lexer, token, identifier);
                    if (fn_call_node == NULL) {
                        free(identifier);
                        free_ast_node(block_node);
                        return NULL;
                    }
                    if (append_node_to_block(block_node, fn_call_node) != 0) {
                        free(identifier);
                        free_ast_node(fn_call_node);
                        free_ast_node(block_node);
                        return NULL;
                    }
                }
                else if (check_token(*token, TOKEN_DOT, NULL)) {
                    ASTNode* builtin_fn_call = parse_builtin_fn_call(lexer, token, identifier);
                    if (builtin_fn_call == NULL) {
                        free(identifier);
                        free_ast_node(block_node);
                        return NULL;
                    }
                    if (append_node_to_block(block_node, builtin_fn_call) != 0) {
                        free(identifier);
                        free_ast_node(builtin_fn_call);
                        free_ast_node(block_node);
                        return NULL;
                    }
                }
                else {
                    free(identifier);
                    free_ast_node(block_node);
                    return NULL;
                }
                free(identifier);
                advance_token(token, lexer); // assignment and fn call end on ';'
            }
               break;
            case TOKEN_RETURN:
                // expression
                break;
            default:
                free_ast_node(block_node);
                return NULL;
                break;
        }
        // in if token we are checkin for else, and if there is not this
        // advnace token would mess this up
        // advance_token(token, lexer);
    }

    return block_node;
}

ASTNode* parse_fn_decl(Lexer* lexer, Token** token) {
    advance_token(token, lexer);
    if (!check_token(*token, TOKEN_FN, NULL)) {
        return NULL;
    }
    advance_token(token, lexer);
    if (!check_token(*token, TOKEN_IDENTIFIER, NULL)) {
        return NULL;
    }
    // Create node for function declaration
    ASTNode* fn_decl_node =  create_fn_decl_node((*token)->value);
    if (fn_decl_node == NULL) {
        return NULL;
    }
    advance_token(token, lexer);
    if (!check_token(*token, TOKEN_L_PAREN, NULL)) {
        free_ast_node(fn_decl_node);
        return NULL;
    }
    advance_token(token, lexer);
    // Loop until we encouter right parenthesis or if something goes wrong within the loop
    while (!check_token(*token, TOKEN_R_PAREN, NULL)) {
        ASTNode* param_node = parse_fn_params(lexer, token);
        if (param_node == NULL) {
            free_ast_node(fn_decl_node);
            return NULL;
        }
        // Append parameter to function
        if (append_param_to_fn(fn_decl_node, param_node) != 0) {
            free_ast_node(fn_decl_node);
            return NULL;
        }
    }
    advance_token(token, lexer);

    // Check for nullable data type
    if (check_token(*token, TOKEN_Q_MARK, NULL)) {
        fn_decl_node->FnDecl.nullable = true;
        advance_token(token, lexer);
    }

    // check for NULL pointer to prevent SEGFAULT
    if (token == NULL || *token == NULL) {
        free_ast_node(fn_decl_node);
        return NULL;
    }
    // Check for return type
    switch ((*token)->token_type) {
        case TOKEN_VOID:
            fn_decl_node->FnDecl.return_type = AST_VOID;
            break;
        case TOKEN_I32:
            fn_decl_node->FnDecl.return_type = AST_I32;
            break;
        case TOKEN_F64:
            fn_decl_node->FnDecl.return_type = AST_F64;
            break;
        case TOKEN_U8:
            fn_decl_node->FnDecl.return_type = AST_U8;
            break;
        case TOKEN_SLICE:
            advance_token(token, lexer);
            if (!check_token(*token, TOKEN_U8, NULL)) {
                free_ast_node(fn_decl_node);
                return NULL;
            }
            fn_decl_node->FnDecl.return_type = AST_SLICE;
            break;
        // add more
        default: // Unexpected token syntax error
            free_ast_node(fn_decl_node);
            return NULL;
            break;
    }

    // Parse block
    advance_token(token, lexer);
    ASTNode* block_node = parse_block(lexer, token);
    if (block_node == NULL) {
        free_ast_node(fn_decl_node);
        return NULL;
    }
    fn_decl_node->FnDecl.block = block_node;

    return fn_decl_node;
}

ASTNode* parse_tokens(Lexer* lexer) {
    Token* token = get_token(lexer);
    if (token == NULL) {
        return NULL; // idk (empty code allowed ?)
    }
    
    // Prolog must be at the start so we check it first?
    if (!parse_prolog(lexer, &token)) {
        free_token(token);
        set_error(SYNTAX_ERROR);
        return NULL;
    }
    advance_token(&token, lexer);
    ASTNode* program_node = create_program_node();  // Create root (program node)
    // Loop until the token is EOF
    while (!check_token(token, TOKEN_EOF, NULL)) {
        // CONST
        if (check_token(token, TOKEN_CONST, NULL)) {
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
            ASTNode* fn_decl = parse_fn_decl(lexer, &token);
            if (fn_decl != NULL) {
                if (append_decl_to_prog(program_node, fn_decl) != 0) {
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

        advance_token(&token, lexer);
    }

    free_token(token);
    return program_node;

    // Error handle for go to
    error:
        // set error ot syntax error if no lexical or internal error was found before
        set_error(SYNTAX_ERROR);
        free_ast_node(program_node);
        free_token(token);
        return NULL;
}

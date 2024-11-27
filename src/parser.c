/** 
 * @file parser.c
 * @brief Implementation of a parser (syntax analysis)
 * @authors Michal Repcik (xrepcim00) Simon Bobko (xbobkos00)
*/
#define _POSIX_C_SOURCE 200809L // Used for strdup(), optimize 
#include "error.h"
#include "parser.h"
#include "stack_exp.h"
#include "ast_node_stack.h"

void advance_token(Token** token, Lexer* lexer) {
    if (token && *token) {
        free_token(*token);
    }
    *token = get_token(lexer);
    if (*token == NULL) {
        set_error(LEXICAL_ERROR);
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

int is_end_of_expression(int paren_counter, Token* token) {
    if (token == NULL) {
        return 1; // Consider NULL token as end of expression
    }
    return (check_token(token, TOKEN_COMMA, NULL) ||
            check_token(token, TOKEN_SEMICOLON, NULL) ||
            (paren_counter == 0 && check_token(token, TOKEN_R_PAREN, NULL)));
}


int get_precedence(TokenType op) {
    switch (op) {
        case TOKEN_EQU:
        case TOKEN_NOT_EQU:
            return 1;
        case TOKEN_LESS:
        case TOKEN_LESS_EQU:
        case TOKEN_GREATER:
        case TOKEN_GREATER_EQU:
            return 2;
        case TOKEN_PLUS:
        case TOKEN_MINUS:
            return 3;
        case TOKEN_MULT:
        case TOKEN_DIV:
            return 4;
        default:
            return 0;
    }
}

int is_operand_token(Token* token) {
    return token != NULL && (
        token->token_type == TOKEN_IDENTIFIER ||
        token->token_type == TOKEN_INTEGER ||
        token->token_type == TOKEN_FLOAT ||
        token->token_type == TOKEN_STRING ||
        token->token_type == TOKEN_NULL
    );
}

int is_operator_token(Token* token) {
    return token != NULL && (
        token->token_type == TOKEN_PLUS ||
        token->token_type == TOKEN_MINUS ||
        token->token_type == TOKEN_MULT ||
        token->token_type == TOKEN_DIV ||
        // Comparison operators
        token->token_type == TOKEN_LESS ||
        token->token_type == TOKEN_LESS_EQU ||      // Add this line
        token->token_type == TOKEN_GREATER ||
        token->token_type == TOKEN_GREATER_EQU ||   // Add this line
        token->token_type == TOKEN_EQU ||
        token->token_type == TOKEN_NOT_EQU
    );
}


ASTNode* parse_operand(Lexer* lexer, Token** token) {
    ASTNode* node = NULL;

    if (check_token(*token, TOKEN_IDENTIFIER, NULL)) {
        char* identifier = strdup((*token)->value);
        if (!identifier) {
            set_error(INTERNAL_ERROR);
            return NULL;
        }
        advance_token(token, lexer);
        if (*token == NULL || (*token)->token_type == TOKEN_EOF) {
            set_error(LEXICAL_ERROR);
            free(identifier);
            return NULL;
        }
        if (check_token(*token, TOKEN_L_PAREN, NULL)) {
            node = parse_fn_call(lexer, token, identifier);
        }
        else if (check_token(*token, TOKEN_DOT, NULL)) {
            node = parse_builtin_fn_call(lexer, token, identifier);
        }
        else {
            node = create_identifier_node(identifier);
        }
        free(identifier);
        if (!node) {  // If AST node creation failed
            set_error(INTERNAL_ERROR);
            return NULL;
        }
    }
    else if (check_token(*token, TOKEN_NULL, NULL)) {
        node = create_null_node();
        if (!node) {
            set_error(INTERNAL_ERROR);
            return NULL;
        }
        advance_token(token, lexer);
        if (*token == NULL || (*token)->token_type == TOKEN_EOF) {
            set_error(LEXICAL_ERROR);
            free_ast_node(node);
            return NULL;
        }
    }
    else if (check_token(*token, TOKEN_INTEGER, NULL)) {
        node = create_i32_node(atoi((*token)->value));
        if (!node) {
            set_error(INTERNAL_ERROR);
            return NULL;
        }
        advance_token(token, lexer);
        if (*token == NULL || (*token)->token_type == TOKEN_EOF) {
            free_ast_node(node);
            set_error(LEXICAL_ERROR);
            return NULL;
        }
    } 
    else if (check_token(*token, TOKEN_FLOAT, NULL)) {
        node = create_f64_node(atof((*token)->value));
        if (!node) {
            set_error(INTERNAL_ERROR);
            return NULL;
        }
        advance_token(token, lexer);
        if (*token == NULL || (*token)->token_type == TOKEN_EOF) {
            set_error(LEXICAL_ERROR);
            free_ast_node(node);
            return NULL;
        }
    } 
    else if (check_token(*token, TOKEN_STRING, NULL)) {
        node = create_string_node((*token)->value);
        if (!node) {
            set_error(INTERNAL_ERROR);
            return NULL;
        }
        advance_token(token, lexer);
        if (*token == NULL || (*token)->token_type == TOKEN_EOF) {
            set_error(LEXICAL_ERROR);
            free_ast_node(node);
            return NULL;
        }
    }
    else if (check_token(*token, TOKEN_L_PAREN, NULL)) {
        advance_token(token, lexer);
        if (*token == NULL || (*token)->token_type == TOKEN_EOF) {
            set_error(LEXICAL_ERROR);
            return NULL;
        }
        node = parse_expression(lexer, token);
        if (!node) {
            set_error(INTERNAL_ERROR);
            return NULL;
        }
        if (!check_token(*token, TOKEN_R_PAREN, NULL)) {
            set_error(SYNTAX_ERROR);
            free_ast_node(node);
            return NULL;
        }
        advance_token(token, lexer);
        if (*token == NULL || (*token)->token_type == TOKEN_EOF){
            set_error(LEXICAL_ERROR);
            free_ast_node(node);
            return NULL;
        }
    } 
    else {
        set_error(SYNTAX_ERROR);
        return NULL;
    }
    return node;
}

ASTNode* parse_expression(Lexer* lexer, Token** token) {
    // Initialize operator stack using stack.h
    StackPtr op_stack = init_stack();
    if (!op_stack) {
        set_error(INTERNAL_ERROR);
        return NULL;
    }

    // Initialize operand stack for ASTNode*
    ASTNodeStackPtr operand_stack = init_ast_node_stack();
    if (!operand_stack) {
        set_error(INTERNAL_ERROR);
        free_resources(op_stack);
        return NULL;
    }
    
    // Begin parsing tokens
    int paren_counter = 0;
    while (*token != NULL && !check_token(*token, TOKEN_EOF, NULL) && !is_end_of_expression(paren_counter, *token)) {
        if (is_operand_token(*token)) {
            // Create AST node for operand and push to operand stack
            ASTNode* operand = parse_operand(lexer, token);
            if (!operand) {
                // Handle error
                set_error(SYNTAX_ERROR);
                free_resources(op_stack);
                free_ast_node_stack(operand_stack);
                return NULL;
            }
            push_ast_node(operand_stack, operand);
        } 
        else if (is_operator_token(*token)) {
            int op = (*token)->token_type;

            while (!is_empty(op_stack->top) &&
                   (get_precedence(op) <= get_precedence(top(op_stack)))) {
                int top_op = top(op_stack);

                pop(op_stack);

                // Attempt to pop two operands
                ASTNode* right = pop_ast_node(operand_stack);
                ASTNode* left = pop_ast_node(operand_stack);

                if (!left || !right) {
                    set_error(SYNTAX_ERROR);
                    free_resources(op_stack);
                    free_ast_node_stack(operand_stack);
                    return NULL;
                }

                // Create binary operator node
                ASTNode* op_node = create_binary_op_node(top_op, left, right);
                if (!op_node) {
                    set_error(INTERNAL_ERROR);
                    free_resources(op_stack);
                    free_ast_node_stack(operand_stack);
                    return NULL;
                }
                push_ast_node(operand_stack, op_node);
            }

            // Push current operator onto operator stack
            push(op_stack, op);
            advance_token(token, lexer);
            if (*token == NULL || (*token)->token_type == TOKEN_EOF){
                set_error(LEXICAL_ERROR);
                free_resources(op_stack);
                free_ast_node_stack(operand_stack);
                return NULL;
            }
        } 
        else if (check_token(*token, TOKEN_L_PAREN, NULL)) {
            paren_counter++;
            push(op_stack, TOKEN_L_PAREN);
            advance_token(token, lexer);
            if (*token == NULL || (*token)->token_type == TOKEN_EOF) {
                set_error(LEXICAL_ERROR);
                free_resources(op_stack);
                free_ast_node_stack(operand_stack);
                return NULL;
            }

        } 
        else if (check_token(*token, TOKEN_R_PAREN, NULL)) {
            paren_counter--;

            while (!is_empty(op_stack->top) && top(op_stack) != TOKEN_L_PAREN) {
                int top_op = top(op_stack);

                pop(op_stack);

                // Attempt to pop two operands
                ASTNode* right = pop_ast_node(operand_stack);
                ASTNode* left = pop_ast_node(operand_stack);

                if (!left || !right) {
                    set_error(SYNTAX_ERROR);
                    free_resources(op_stack);
                    free_ast_node_stack(operand_stack);
                    return NULL;
                }

                // Create binary operator node
                ASTNode* op_node = create_binary_op_node(top_op, left, right);
                if (!op_node) {
                    set_error(INTERNAL_ERROR);
                    free_resources(op_stack);
                    free_ast_node_stack(operand_stack);
                    return NULL;
                }
                push_ast_node(operand_stack, op_node);
            }

            if (is_empty(op_stack->top)) {
                // Mismatched parentheses
                set_error(SYNTAX_ERROR);

                free_resources(op_stack);
                free_ast_node_stack(operand_stack);
                return NULL;
            }

            // Pop the left parenthesis from the operator stack
            pop(op_stack);
            advance_token(token, lexer);
            if (*token == NULL || (*token)->token_type == TOKEN_EOF) {
                set_error(LEXICAL_ERROR);
                free_resources(op_stack);
                free_ast_node_stack(operand_stack);
                return NULL;
            }

        } 
        else {
            // Unknown token encountered
            free_resources(op_stack);
            free_ast_node_stack(operand_stack);
            set_error(SYNTAX_ERROR);
            return NULL;
        }
    }
    if (*token == NULL) {
        set_error(LEXICAL_ERROR);
        free_resources(op_stack);
        free_ast_node_stack(operand_stack);
        return NULL;
    }

    // Pop remaining operators
    while (!is_empty(op_stack->top)) {
        int top_op = top(op_stack);

        pop(op_stack);

        if (top_op == TOKEN_L_PAREN || top_op == TOKEN_R_PAREN) {
            // Mismatched parentheses
            set_error(SYNTAX_ERROR);

            free_resources(op_stack);
            free_ast_node_stack(operand_stack);
            return NULL;
        }

        // Attempt to pop two operands
        ASTNode* right = pop_ast_node(operand_stack);
        ASTNode* left = pop_ast_node(operand_stack);

        if (!left || !right) {
            set_error(SYNTAX_ERROR);

            if (right) {
                free_ast_node(right);
            }
            if (left) {
                free_ast_node(left);
            }
            free_resources(op_stack);
            free_ast_node_stack(operand_stack);
            return NULL;
        }

        // Create binary operator node
        ASTNode* op_node = create_binary_op_node(top_op, left, right);
        if (!op_node) {
            set_error(INTERNAL_ERROR);

            free_ast_node(left);
            free_ast_node(right);

            free_resources(op_stack);
            free_ast_node_stack(operand_stack);
            return NULL;
        }
        push_ast_node(operand_stack, op_node);
    }

    // The result should be the only operand left
    if (operand_stack->top != 0) {
        set_error(SYNTAX_ERROR);

        free_resources(op_stack);
        free_ast_node_stack(operand_stack);
        return NULL;
    }

    ASTNode* expression_node = pop_ast_node(operand_stack);

    if (paren_counter != 0) {
        set_error(SYNTAX_ERROR);
        free_resources(op_stack);
        free_ast_node_stack(operand_stack);
        free_ast_node(expression_node);
        return NULL;
    }
    // Free the stacks
    free_resources(op_stack);
    free_ast_node_stack(operand_stack);

    return expression_node;
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

    advance_token(token, lexer);
    ASTNode* expression_node = parse_expression(lexer, token);
    if (expression_node == NULL) {
        free_ast_node(const_decl_node);
        return NULL;
    }

    // Connect expression node to const decl node
    const_decl_node->ConstDecl.expression = expression_node;

    if(!check_token(*token, TOKEN_SEMICOLON, NULL)) {
        free_ast_node(const_decl_node);
        return NULL;
    }

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

    advance_token(token, lexer);
    ASTNode* expression_node = parse_expression(lexer, token);
    if (expression_node == NULL) {
        free_ast_node(var_decl_node);
        return NULL;
    }

    var_decl_node->VarDecl.expression = expression_node;

    if (!check_token(*token, TOKEN_SEMICOLON, NULL)) {
        free_ast_node(var_decl_node);
        return NULL;
    }

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
    advance_token(token, lexer);
    ASTNode* expression_node = NULL;
    if (!check_token(*token, TOKEN_R_PAREN, NULL)) {
        expression_node = parse_expression(lexer, token);
        if (expression_node == NULL) {
            return NULL;
        }
    }
    if (!check_token(*token, TOKEN_R_PAREN, NULL)) {
        free_ast_node(expression_node);
        return NULL;
    }

    // Create if else node
    ASTNode* if_else_node = create_if_node();
    if (if_else_node == NULL) {
        free_ast_node(expression_node);
        return NULL;
    }
    // Assign expression to if else node
    if_else_node->IfElse.expression = expression_node;

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
    advance_token(token, lexer);
    ASTNode* expression_node = NULL;
    if (!check_token(*token, TOKEN_R_PAREN, NULL)) {
        expression_node = parse_expression(lexer, token);
        if (expression_node == NULL) {
            return NULL;
        }   
    }
    if (!check_token(*token, TOKEN_R_PAREN, NULL)) {
        free(expression_node);
        return NULL;
    }
    // Create while node
    ASTNode* while_node = create_while_node();
    if (while_node == NULL) {
        free(expression_node);
        return NULL;
    }
    // Add expression node to while node
    while_node->WhileCycle.expression = expression_node;
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

ASTNode* parse_fn_arg(Lexer* lexer, Token** token) {
    ASTNode* arg_node = create_arg_node();
    if (arg_node == NULL) {
        return NULL;
    }
    ASTNode* expression_node = parse_expression(lexer, token);
    if (expression_node == NULL) {
        free_ast_node(arg_node);
        return NULL;
    }
    // Add expression to argument node
    arg_node->Argument.expression = expression_node;
    return arg_node;
}

ASTNode* parse_fn_call(Lexer* lexer, Token** token, char* identifier) {
    // We are at '(' token
    ASTNode* fn_call = create_fn_call_node(identifier);
    if (fn_call == NULL) {
        return NULL;
    }
    // Parse fn args
    advance_token(token, lexer);
    while (!check_token(*token, TOKEN_R_PAREN, NULL)) {
        ASTNode* fn_arg_node = parse_fn_arg(lexer, token);
        if (fn_arg_node == NULL) {
            free_ast_node(fn_call);
            return NULL;
        }
        if (append_arg_to_fn(fn_call, fn_arg_node) != 0) {
            free_ast_node(fn_call);
            free_ast_node(fn_arg_node);
            return NULL;
        }
        if (check_token(*token, TOKEN_R_PAREN, NULL)) {
            break;
        }
        if (!check_token(*token, TOKEN_COMMA, NULL)) {
            free_ast_node(fn_call);
            free_ast_node(fn_arg_node);
            return NULL;
        }
        advance_token(token, lexer);
    }
    advance_token(token, lexer);   // Advance token for expr_parser
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
    builtin_fn_call->FnCall.is_builtin = true;
    advance_token(token, lexer);
    if (!check_token(*token, TOKEN_L_PAREN, NULL)) {
        free(builtin_fn_call);
        return NULL;
    }
    advance_token(token, lexer);
    // parse expression
    while (!check_token(*token, TOKEN_R_PAREN, NULL)) {
        ASTNode* fn_arg_node = parse_fn_arg(lexer, token);
            if (fn_arg_node == NULL) {
                free_ast_node(builtin_fn_call);
                return NULL;
            }
            if (append_arg_to_fn(builtin_fn_call, fn_arg_node) != 0) {
                free_ast_node(builtin_fn_call);
                free_ast_node(fn_arg_node);
                return NULL;
            }
            if (check_token(*token, TOKEN_R_PAREN, NULL)) {
                break;
            }
            if (!check_token(*token, TOKEN_COMMA, NULL)) {
                free_ast_node(builtin_fn_call);
                free_ast_node(fn_arg_node);
                return NULL;
            }
            advance_token(token, lexer);
    }
    advance_token(token, lexer); // advance token for expr_parser
    return builtin_fn_call;
}

ASTNode* parse_assignment(Lexer* lexer, Token** token, char* identifier) {
    ASTNode* assignment_node = create_assignment_node(identifier);
    if (assignment_node == NULL) {
        return NULL;
    }
    advance_token(token, lexer);
    ASTNode* expression_node = parse_expression(lexer, token);
    if (expression_node == NULL) {
        free_ast_node(assignment_node);
        return NULL;
    }
    if (!check_token(*token, TOKEN_SEMICOLON, NULL)) {
        free_ast_node(assignment_node);
        free_ast_node(expression_node);
        return NULL;
    }

    assignment_node->Assignment.expression = expression_node;

    return assignment_node;
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
                    ASTNode* assignment_node = parse_assignment(lexer, token, identifier);
                    if (assignment_node == NULL) {
                        free(identifier);
                        free_ast_node(block_node);
                        return NULL;
                    }
                    if (append_node_to_block(block_node, assignment_node) != 0) {
                        free(identifier);
                        free_ast_node(assignment_node);
                        free_ast_node(block_node);
                        return NULL;
                    }
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
                    if (!check_token(*token, TOKEN_SEMICOLON, NULL)) {
                        free(identifier);
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
                    if (!check_token(*token, TOKEN_SEMICOLON, NULL)) {
                        free(identifier);
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
            case TOKEN_RETURN: {
                ASTNode* return_node = create_return_node();
                if (return_node == NULL) {
                    free_ast_node(block_node);
                    return NULL;
                }
                advance_token(token, lexer);
                if (check_token(*token, TOKEN_SEMICOLON, NULL)) {
                    advance_token(token, lexer);
                    if (append_node_to_block(block_node, return_node) != 0) {
                        free_ast_node(block_node);
                        free_ast_node(return_node);
                        return NULL;
                    }
                    break;
                }
                ASTNode* expression_node = parse_expression(lexer, token);
                if (expression_node == NULL) {
                    free_ast_node(block_node);
                    free_ast_node(return_node);
                    return NULL;
                }
                return_node->Return.expression = expression_node;
                if (append_node_to_block(block_node, return_node) != 0) {
                    free_ast_node(block_node);
                    free_ast_node(return_node);
                    return NULL;
                }
                if (!check_token(*token, TOKEN_SEMICOLON, NULL)) {
                    free_ast_node(block_node);
                    return NULL;
                }
                advance_token(token, lexer);
                break;
            }
            default:
                free_ast_node(block_node);
                return NULL;
                break;
        }
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

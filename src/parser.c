/** 
 * @file parser.c
 * @brief Implementation of a parser (syntax analysis)
 * @authors Michal Repcik (xrepcim00)
*/

#define _POSIX_C_SOURCE 200809L // Used for strdup(), optimize later
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "lexer.h"
#include "ast.h"
#include "stack.h"
#include "ast_node_stack.h"
#include "error.h"


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
            set_error(SYNTAX_ERROR); // Set error to syntax error if value is different then expected
            return 0;   // Token value is different from expected value
        }
    }
    return 1;   // Return true
}

// ---------- Expression tree funcionality ----------

// Define operator precedence
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

// Define operator associativity (left-to-right)
int is_left_associative(TokenType op) {
    (void)op;
    return 1; // Assuming all operators are left-associative
}

int is_operand_token(Token* token) {
    return token != NULL && (
        token->token_type == TOKEN_IDENTIFIER ||
        token->token_type == TOKEN_I32 ||
        token->token_type == TOKEN_F64 ||
        token->token_type == TOKEN_STRING
    );
}

int is_operator_token(Token* token) {
    return token != NULL && (
        token->token_type == TOKEN_PLUS ||
        token->token_type == TOKEN_MINUS ||
        token->token_type == TOKEN_MULT ||
        token->token_type == TOKEN_DIV ||
        // Add other operators
        token->token_type == TOKEN_LESS ||
        token->token_type == TOKEN_GREATER ||
        token->token_type == TOKEN_EQU ||
        token->token_type == TOKEN_NOT_EQU
    );
}

ASTNode* parse_operand(Lexer* lexer, Token** token) {
    ASTNode* node = NULL;
    if (check_token(*token, TOKEN_IDENTIFIER, NULL)) {
        node = create_identifier_node((*token)->value);
    } else if (check_token(*token, TOKEN_I32, NULL)) {
        node = create_i32_node(atoi((*token)->value));
    } else if (check_token(*token, TOKEN_F64, NULL)) {
        node = create_f64_node(atof((*token)->value));
    } else if (check_token(*token, TOKEN_STRING, NULL)) {
        node = create_string_node((*token)->value);
    } else if (check_token(*token, TOKEN_L_PAREN, NULL)) {
        advance_token(token, lexer);
        node = parse_expression(lexer, token);
        if (!check_token(*token, TOKEN_R_PAREN, NULL)) {
            set_error(SYNTAX_ERROR);
            free_ast_node(node);
            return NULL;
        }
    } else {
        set_error(SYNTAX_ERROR);
        return NULL;
    }
    advance_token(token, lexer);
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

    while (1) {
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
        } else if (is_operator_token(*token)) {
            int op = (*token)->token_type;
            while (!is_empty(op_stack->top) &&
                   ((is_left_associative(op) && get_precedence(op) <= get_precedence(top(op_stack))) ||
                    (!is_left_associative(op) && get_precedence(op) < get_precedence(top(op_stack))))) {
                // Pop operator from op_stack and operands from operand_stack to create AST node
                int top_op = top(op_stack);
                pop(op_stack);

                ASTNode* right = pop_ast_node(operand_stack);
                ASTNode* left = pop_ast_node(operand_stack);

                if (!left || !right) {
                    set_error(SYNTAX_ERROR);
                    free_resources(op_stack);
                    free_ast_node_stack(operand_stack);
                    return NULL;
                }

                ASTNode* op_node = create_binary_op_node(top_op, left, right);
                if (!op_node) {
                    // Handle error
                    set_error(INTERNAL_ERROR);
                    free_resources(op_stack);
                    free_ast_node_stack(operand_stack);
                    return NULL;
                }
                push_ast_node(operand_stack, op_node);
            }
            push(op_stack, op);
            advance_token(token, lexer);
        } else if (check_token(*token, TOKEN_L_PAREN, NULL)) {
            push(op_stack, TOKEN_L_PAREN);
            advance_token(token, lexer);
        } else if (check_token(*token, TOKEN_R_PAREN, NULL)) {
            while (!is_empty(op_stack->top) && top(op_stack) != TOKEN_L_PAREN) {
                int top_op = top(op_stack);
                pop(op_stack);

                ASTNode* right = pop_ast_node(operand_stack);
                ASTNode* left = pop_ast_node(operand_stack);

                if (!left || !right) {
                    set_error(SYNTAX_ERROR);
                    free_resources(op_stack);
                    free_ast_node_stack(operand_stack);
                    return NULL;
                }

                ASTNode* op_node = create_binary_op_node(top_op, left, right);
                if (!op_node) {
                    // Handle error
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
            pop(op_stack); // Pop the left parenthesis
            advance_token(token, lexer);
        } else {
            // End of expression or unexpected token
            break;
        }
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

        ASTNode* right = pop_ast_node(operand_stack);
        ASTNode* left = pop_ast_node(operand_stack);

        if (!left || !right) {
            set_error(SYNTAX_ERROR);
            free_resources(op_stack);
            free_ast_node_stack(operand_stack);
            return NULL;
        }

        ASTNode* op_node = create_binary_op_node(top_op, left, right);
        if (!op_node) {
            // Handle error
            set_error(INTERNAL_ERROR);
            free_resources(op_stack);
            free_ast_node_stack(operand_stack);
            return NULL;
        }
        push_ast_node(operand_stack, op_node);
    }

    // The result should be the only operand left
    if (operand_stack->top != 0) {
        // Error: operands left without operators
        set_error(SYNTAX_ERROR);
        free_resources(op_stack);
        free_ast_node_stack(operand_stack);
        return NULL;
    }

    ASTNode* expression_node = pop_ast_node(operand_stack);

    // Free the stacks
    free_resources(op_stack);
    free_ast_node_stack(operand_stack);

    return expression_node;
}

// ---------- End Expression tree funcionality ----------


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
    // Parse succeeded
    return 0;
}

ASTNode* parse_const_decl(Lexer* lexer, Token** token) {
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
    advance_token(token, lexer);
    ASTNode* expression_node = parse_expression(lexer, token);
    if (!expression_node) {
        free_ast_node(const_decl_node);
        return NULL;
    }
    const_decl_node->ConstDecl.expression = expression_node;

    if (!check_token(*token, TOKEN_SEMICOLON, NULL)) {
        free_ast_node(const_decl_node);
        return NULL;
    }
    advance_token(token, lexer);
    return const_decl_node;

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
        if (*token == NULL) {
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
    if (!expression_node) {
        free_ast_node(var_decl_node);
        return NULL;
    }
    var_decl_node->VarDecl.expression = expression_node;

    if (!check_token(*token, TOKEN_SEMICOLON, NULL)) {
        free_ast_node(var_decl_node);
        return NULL;
    }
    advance_token(token, lexer);

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
        return NULL;
    }
    advance_token(token, lexer);
    // check for null pointer before acessing token type
    if (*token == NULL)
        return NULL;
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
        // add more
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
    // Create if else node
    ASTNode* if_else_node = create_if_node();
    if (if_else_node == NULL) {
        return NULL;
    }
    ASTNode* condition = parse_expression(lexer, token);
    if (!condition) {
        free_ast_node(if_else_node);
        return NULL;
    }
    if_else_node->IfElse.expression = condition;

    if (!check_token(*token, TOKEN_R_PAREN, NULL)) {
        free_ast_node(if_else_node);
        return NULL;
    }
    advance_token(token, lexer);


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
    if (!check_token(*token, TOKEN_WHILE, NULL)) {
        return NULL;
    }
    advance_token(token, lexer);

    if (!check_token(*token, TOKEN_L_PAREN, NULL)) {
        return NULL;
    }
    advance_token(token, lexer);

    // Create while node before parsing the condition
    ASTNode* while_node = create_while_node();
    if (while_node == NULL) {
        return NULL;
    }

    // Parse the condition expression
    ASTNode* condition = parse_expression(lexer, token);
    if (!condition) {
        free_ast_node(while_node);
        return NULL;
    }
    while_node->WhileCycle.expression = condition;

    // Check for ')'
    if (!check_token(*token, TOKEN_R_PAREN, NULL)) {
        free_ast_node(while_node);
        return NULL;
    }
    advance_token(token, lexer);

    // Check for element bind (if your language supports it)
    if (parse_element_bind(lexer, token, while_node) != 0) {
        free_ast_node(while_node);
        return NULL;
    }

    // Parse the loop body (block)
    ASTNode* block_node = parse_block(lexer, token);
    if (block_node == NULL) {
        free_ast_node(while_node);
        return NULL;
    }
    while_node->WhileCycle.block = block_node;

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

ASTNode* parse_block(Lexer* lexer, Token** token) {
    advance_token(token, lexer);
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
                // Since parse const checks for identifier we must advnace token
                advance_token(token, lexer);
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
                char* identifier = strdup((*token)->value);
                advance_token(token, lexer);
                if (check_token(*token, TOKEN_ASSIGN, NULL)) {
                    // parse assignment
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
    // check for NULL pointer to prevent SEGFAULT
    if (*token == NULL) {
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
        // add more
        default: // Unexpected token syntax error
            free_ast_node(fn_decl_node);
            return NULL;
            break;
    }

    // Parse block
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

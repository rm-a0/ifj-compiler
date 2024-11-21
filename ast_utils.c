/** 
 * @file ast_utils.c
 * @brief Implementation of debugging functions for AST and ASTStack
 * @authors Simon Bobko (xbobkos00)
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "error.h"
#include "ast.h"
#include "ast_node_stack.h"

/**
 * @brief Converts TokenType enum to its string representation.
 * @param token_type The TokenType enum value.
 * @return String representation of the TokenType.
 */
const char* token_type_to_string(TokenType token_type) {
    switch (token_type) {
        case INVALID: return "INVALID";
        case TOKEN_EOF: return "EOF";
        case TOKEN_CONST: return "CONST";
        case TOKEN_ELSE: return "ELSE";
        case TOKEN_FN: return "FN";
        case TOKEN_IF: return "IF";
        case TOKEN_I32: return "I32";
        case TOKEN_F64: return "F64";
        case TOKEN_NULL: return "NULL";
        case TOKEN_PUB: return "PUB";
        case TOKEN_RETURN: return "RETURN";
        case TOKEN_U8: return "U8";
        case TOKEN_VAR: return "VAR";
        case TOKEN_VOID: return "VOID";
        case TOKEN_WHILE: return "WHILE";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_STRING: return "STRING";
        case TOKEN_INTEGER: return "INTEGER";
        case TOKEN_FLOAT: return "FLOAT";
        case TOKEN_SLICE: return "SLICE";
        case TOKEN_L_PAREN: return "LEFT_PAREN";
        case TOKEN_R_PAREN: return "RIGHT_PAREN";
        case TOKEN_L_BRACE: return "LEFT_BRACE";
        case TOKEN_R_BRACE: return "RIGHT_BRACE";
        case TOKEN_DOT: return "DOT";
        case TOKEN_COMMA: return "COMMA";
        case TOKEN_COLON: return "COLON";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_PIPE: return "PIPE";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_MULT: return "MULTIPLY";
        case TOKEN_DIV: return "DIVIDE";
        case TOKEN_ASSIGN: return "ASSIGN";
        case TOKEN_Q_MARK: return "QUESTION_MARK";
        case TOKEN_LESS: return "LESS_THAN";
        case TOKEN_GREATER: return "GREATER_THAN";
        case TOKEN_EXCM: return "EXCLAMATION_MARK";
        case TOKEN_LESS_EQU: return "LESS_THAN_OR_EQUAL";
        case TOKEN_GREATER_EQU: return "GREATER_THAN_OR_EQUAL";
        case TOKEN_NOT_EQU: return "NOT_EQUAL";
        case TOKEN_EQU: return "EQUAL";
        case TOKEN_IMPORT: return "IMPORT";
        default: return "UNKNOWN_TOKEN_TYPE";
    }
}


/**
 * @brief Converts ASTNodeType enum to its string representation.
 * @param type The ASTNodeType enum value.
 * @return String representation of the ASTNodeType.
 */
const char* ast_node_type_to_string(ASTNodeType type) {
    switch (type) {
        case AST_PROGRAM: return "Program";
        case AST_FN_DECL: return "Function Declaration";
        case AST_PARAM: return "Parameter";
        case AST_VAR_DECL: return "Variable Declaration";
        case AST_CONST_DECL: return "Constant Declaration";
        case AST_BLOCK: return "Block";
        case AST_FN_CALL: return "Function Call";
        case AST_ARG: return "Argument";
        case AST_WHILE: return "While Loop";
        case AST_IF_ELSE: return "If-Else Statement";
        case AST_BIN_OP: return "Binary Operator";
        case AST_INT: return "Integer Literal";
        case AST_FLOAT: return "Float Literal";
        case AST_STRING: return "String Literal";
        case AST_IDENTIFIER: return "Identifier";
        case AST_RETURN: return "Return Statement";
        case AST_ASSIGNMENT: return "Assignment";
        case AST_NULL: return "Null";
        default: return "Unknown AST Node Type";
    }
}

/**
 * @brief Converts OperatorType enum to its string representation.
 * @param op_type The OperatorType enum value.
 * @return String representation of the OperatorType.
 */
const char* operator_type_to_string(OperatorType op_type) {
    switch (op_type) {
        case AST_PLUS: return "PLUS (+)";
        case AST_MINUS: return "MINUS (-)";
        case AST_MUL: return "MULTIPLY (*)";
        case AST_DIV: return "DIVIDE (/)";
        case AST_GREATER: return "GREATER THAN (>)";
        case AST_GREATER_EQU: return "GREATER THAN OR EQUAL (>=)";
        case AST_LESS: return "LESS THAN (<)";
        case AST_LESS_EQU: return "LESS THAN OR EQUAL (<=)";
        case AST_EQU: return "EQUAL (=)";
        case AST_NOT_EQU: return "NOT EQUAL (!=)";
        default: return "Unknown Operator Type";
    }
}

/**
 * @brief Converts DataType enum to its string representation.
 * @param data_type The DataType enum value.
 * @return String representation of the DataType.
 */
const char* data_type_to_string(DataType data_type) {
    switch (data_type) {
        case AST_UNSPECIFIED: return "Unspecified";
        case AST_VOID: return "Void";
        case AST_U8: return "u8";
        case AST_SLICE: return "Slice";
        case AST_I32: return "i32";
        case AST_F64: return "f64";
        default: return "Unknown DataType";
    }
}



// Helper function to print indentation
void print_indent(int indent_level) {
    for (int i = 0; i < indent_level; i++) {
        printf("  "); // Two spaces per indent level
    }
}

void print_ast_node(ASTNode* node, int indent_level) {
    if (node == NULL) {
        print_indent(indent_level);
        printf("NULL ASTNode Pointer\n");
        return;
    }

    print_indent(indent_level);
    printf("ASTNode Type: %s\n", ast_node_type_to_string(node->type));

    switch (node->type) {
        case AST_PROGRAM:
            print_indent(indent_level + 1);
            printf("Declarations (%d):\n", node->Program.decl_count);
            for (int i = 0; i < node->Program.decl_count; i++) {
                print_ast_node(node->Program.declarations[i], indent_level + 2);
            }
            break;

        case AST_FN_DECL:
            print_indent(indent_level + 1);
            printf("Function Name: %s\n", node->FnDecl.fn_name);
            print_indent(indent_level + 1);
            printf("Return Type: %s\n", data_type_to_string(node->FnDecl.return_type));
            print_indent(indent_level + 1);
            printf("Nullable: %s\n", node->FnDecl.nullable ? "Yes" : "No");
            print_indent(indent_level + 1);
            printf("Parameters (%d):\n", node->FnDecl.param_count);
            for (int i = 0; i < node->FnDecl.param_count; i++) {
                print_ast_node(node->FnDecl.params[i], indent_level + 2);
            }
            print_indent(indent_level + 1);
            printf("Function Body:\n");
            print_ast_node(node->FnDecl.block, indent_level + 2);
            break;

        case AST_PARAM:
            print_indent(indent_level + 1);
            printf("Parameter Name: %s\n", node->Param.identifier);
            print_indent(indent_level + 1);
            printf("Data Type: %s\n", data_type_to_string(node->Param.data_type));
            print_indent(indent_level + 1);
            printf("Nullable: %s\n", node->Param.nullable ? "Yes" : "No");
            break;

        case AST_VAR_DECL:
            print_indent(indent_level + 1);
            printf("Variable Name: %s\n", node->VarDecl.var_name);
            print_indent(indent_level + 1);
            printf("Data Type: %s\n", data_type_to_string(node->VarDecl.data_type));
            print_indent(indent_level + 1);
            printf("Nullable: %s\n", node->VarDecl.nullable ? "Yes" : "No");
            if (node->VarDecl.expression != NULL) {
                print_indent(indent_level + 1);
                printf("Initialization Expression:\n");
                print_ast_node(node->VarDecl.expression, indent_level + 2);
            } else {
                print_indent(indent_level + 1);
                printf("Initialization Expression: None\n");
            }
            break;

        case AST_CONST_DECL:
            print_indent(indent_level + 1);
            printf("Constant Name: %s\n", node->ConstDecl.const_name);
            print_indent(indent_level + 1);
            printf("Data Type: %s\n", data_type_to_string(node->ConstDecl.data_type));
            print_indent(indent_level + 1);
            printf("Nullable: %s\n", node->ConstDecl.nullable ? "Yes" : "No");
            if (node->ConstDecl.expression != NULL) {
                print_indent(indent_level + 1);
                printf("Initialization Expression:\n");
                print_ast_node(node->ConstDecl.expression, indent_level + 2);
            } else {
                print_indent(indent_level + 1);
                printf("Initialization Expression: None\n");
            }
            break;

        case AST_BLOCK:
            print_indent(indent_level + 1);
            printf("Block Nodes (%d):\n", node->Block.node_count);
            for (int i = 0; i < node->Block.node_count; i++) {
                print_ast_node(node->Block.nodes[i], indent_level + 2);
            }
            break;

        case AST_FN_CALL:
            print_indent(indent_level + 1);
            printf("Function Call: %s\n", node->FnCall.fn_name);
            print_indent(indent_level + 1);
            printf("Arguments (%d):\n", node->FnCall.arg_count);
            for (int i = 0; i < node->FnCall.arg_count; i++) {
                print_ast_node(node->FnCall.args[i], indent_level + 2);
            }
            break;

        case AST_ARG:
            print_indent(indent_level + 1);
            printf("Argument:\n");
            print_ast_node(node->Argument.expression, indent_level + 2);
            break;

        case AST_WHILE:
            print_indent(indent_level + 1);
            printf("While Loop:\n");
            print_indent(indent_level + 2);
            printf("Condition:\n");
            print_ast_node(node->WhileCycle.expression, indent_level + 3);
            if (node->WhileCycle.element_bind != NULL) {
                print_indent(indent_level + 2);
                printf("Element Bind: %s\n", node->WhileCycle.element_bind);
            }
            print_indent(indent_level + 2);
            printf("Loop Body:\n");
            print_ast_node(node->WhileCycle.block, indent_level + 3);
            break;

        case AST_IF_ELSE:
            print_indent(indent_level + 1);
            printf("If-Else Statement:\n");
            print_indent(indent_level + 2);
            printf("Condition:\n");
            print_ast_node(node->IfElse.expression, indent_level + 3);
            if (node->IfElse.element_bind != NULL) {
                print_indent(indent_level + 2);
                printf("Element Bind: %s\n", node->IfElse.element_bind);
            }
            print_indent(indent_level + 2);
            printf("If Block:\n");
            print_ast_node(node->IfElse.if_block, indent_level + 3);
            if (node->IfElse.else_block != NULL) {
                print_indent(indent_level + 2);
                printf("Else Block:\n");
                print_ast_node(node->IfElse.else_block, indent_level + 3);
            }
            break;

        case AST_BIN_OP:
            print_indent(indent_level + 1);
            printf("Binary Operator: %s\n", operator_type_to_string(node->BinaryOperator.operator));
            print_indent(indent_level + 1);
            printf("Left Operand:\n");
            print_ast_node(node->BinaryOperator.left, indent_level + 2);
            print_indent(indent_level + 1);
            printf("Right Operand:\n");
            print_ast_node(node->BinaryOperator.right, indent_level + 2);
            break;

        case AST_INT:
            print_indent(indent_level + 1);
            printf("Integer Value: %d\n", node->Integer.number);
            break;

        case AST_FLOAT:
            print_indent(indent_level + 1);
            printf("Float Value: %f\n", node->Float.number);
            break;

        case AST_STRING:
            print_indent(indent_level + 1);
            printf("String Value: \"%s\"\n", node->String.string);
            break;

        case AST_IDENTIFIER:
            print_indent(indent_level + 1);
            printf("Identifier: %s\n", node->Identifier.identifier);
            break;

        case AST_ASSIGNMENT:
            print_indent(indent_level + 1);
            printf("Assignment:\n");
            print_indent(indent_level + 2);
            printf("Identifier: %s\n", node->Assignment.identifier);
            print_indent(indent_level + 2);
            printf("Expression:\n");
            print_ast_node(node->Assignment.expression, indent_level + 3);
            break;

        case AST_RETURN:
            print_indent(indent_level + 1);
            printf("Return Statement:\n");
            if (node->Return.expression != NULL) {
                print_ast_node(node->Return.expression, indent_level + 2);
            } else {
                print_indent(indent_level + 2);
                printf("No expression returned.\n");
            }
            break;

        case AST_NULL:
            print_indent(indent_level + 1);
            printf("Null Literal\n");
            break;

        default:
            print_indent(indent_level + 1);
            printf("Details not implemented for this ASTNodeType.\n");
            break;
    }
}


void print_ast_node_stack(ASTNodeStackPtr stack) {
    if (is_empty_ast_node_stack(stack)) {
        printf("ASTNodeStack is empty.\n");
        return;
    }

    printf("ASTNodeStack contents (bottom to top):\n");
    for (int i = 0; i <= stack->top; i++) {
        printf("  [%d]:\n", i);
        print_ast_node(stack->arr[i], 2); // Start with indent_level = 2 for readability
    }
    printf("\n");
}

void save_ast_to_file(ASTNode* ast, const char* filename) {
    // Open the file for writing
    FILE* file = freopen(filename, "w", stdout);
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    // Print the AST to the file
    print_ast_node(ast, 0);

    // Restore stdout to its original state by reopening it to the console
    freopen("/dev/tty", "w", stdout); // For Unix/Linux
    // freopen("CON", "w", stdout); // For Windows
}

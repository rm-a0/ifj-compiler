#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../inc/ast.h"  // Include the header file containing ASTNode definitions
// #include "../inc/generator.h"

/**
 * @brief Creates an AST node for an integer (i32)
 * @param value The integer value to store in the node
 * @return Pointer to the created ASTNode
 */
ASTNode* create_integer_node(int value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        perror("Failed to allocate memory for integer node");
        exit(EXIT_FAILURE);
    }
    node->type = AST_INT;
    node->Integer.number = value;
    return node;
}

/**
 * @brief Creates an AST node for an identifier
 * @param name Name of the identifier
 * @return Pointer to the created ASTNode
 */
ASTNode* create_identifier_node(const char* name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        perror("Failed to allocate memory for identifier node");
        exit(EXIT_FAILURE);
    }
    node->type = AST_IDENTIFIER;
    node->Identifier.identifier = strdup(name);
    if (node->Identifier.identifier == NULL) {
        perror("Failed to allocate memory for identifier name");
        free(node);
        exit(EXIT_FAILURE);
    }
    return node;
}

/**
 * @brief Creates an AST node for a binary operator
 * @param operator Operator type (e.g., AST_PLUS, AST_MUL)
 * @param left Pointer to the left child node
 * @param right Pointer to the right child node
 * @return Pointer to the created ASTNode
 */
ASTNode* create_binary_operator_node(OperatorType operator, ASTNode* left, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        perror("Failed to allocate memory for binary operator node");
        exit(EXIT_FAILURE);
    }
    node->type = AST_BIN_OP;
    node->BinaryOperator.operator = operator;
    node->BinaryOperator.left = left;
    node->BinaryOperator.right = right;
    return node;
}

/**
 * @brief Recursively frees an AST node and its children
 * @param node Pointer to the ASTNode to free
 */
void free_ast_node(ASTNode* node) {
    if (node == NULL) {
        return;
    }

    switch (node->type) {
        case AST_BIN_OP:
            free_ast_node(node->BinaryOperator.left);
            free_ast_node(node->BinaryOperator.right);
            break;
        case AST_IDENTIFIER:
            free(node->Identifier.identifier);
            break;
        case AST_INT:
            // No dynamically allocated memory inside AST_INT nodes
            break;
        // Add cases for other node types if needed
        default:
            break;
    }

    free(node);
}

/**
 * @brief Returns a string representation of an OperatorType
 * @param operator OperatorType enum value
 * @return String representation of the operator
 */
const char* operator_type_to_string(OperatorType operator) {
    switch (operator) {
        case AST_PLUS:
            return "+";
        case AST_MINUS:
            return "-";
        case AST_MUL:
            return "*";
        case AST_DIV:
            return "/";
        case AST_GREATER:
            return ">";
        case AST_GREATER_EQU:
            return ">=";
        case AST_LESS:
            return "<";
        case AST_LESS_EQU:
            return "<=";
        case AST_EQU:
            return "==";
        case AST_NOT_EQU:
            return "!=";
        default:
            return "unknown_operator";
    }
}

/**
 * @brief Prints indentation spaces to the specified output file
 * @param level Indentation level
 * @param output_file Pointer to the output file
 */
void print_indent(int level, FILE* output_file) {
    for (int i = 0; i < level; ++i) {
        fprintf(output_file, "    ");  // 4 spaces per indent level
    }
}

/**
 * @brief Recursively prints the AST starting from the given node to the specified output file
 * @param node Pointer to the ASTNode to print
 * @param indent_level Current indentation level
 * @param output_file Pointer to the output file
 */
void print_ast(ASTNode* node, int indent_level, FILE* output_file) {
    if (node == NULL) {
        print_indent(indent_level, output_file);
        fprintf(output_file, "NULL\n");
        return;
    }

    switch (node->type) {
        case AST_BIN_OP:
            print_indent(indent_level, output_file);
            fprintf(output_file, "Binary Operator: %s\n", operator_type_to_string(node->BinaryOperator.operator));
            print_indent(indent_level, output_file);
            fprintf(output_file, "Left:\n");
            print_ast(node->BinaryOperator.left, indent_level + 1, output_file);
            print_indent(indent_level, output_file);
            fprintf(output_file, "Right:\n");
            print_ast(node->BinaryOperator.right, indent_level + 1, output_file);
            break;
        case AST_IDENTIFIER:
            print_indent(indent_level, output_file);
            fprintf(output_file, "Identifier: %s\n", node->Identifier.identifier);
            break;
        case AST_INT:
            print_indent(indent_level, output_file);
            fprintf(output_file, "Integer: %d\n", node->Integer.number);
            break;
        // Add cases for other node types if needed
        default:
            print_indent(indent_level, output_file);
            fprintf(output_file, "Unknown node type\n");
            break;
    }
}


int main() {
    // Create identifier nodes for 'a', 'b', and 'c'
    ASTNode* node_a = create_identifier_node("a");
    ASTNode* node_b = create_identifier_node("b");
    ASTNode* node_c = create_identifier_node("c");

    // Create integer node for '5'
    ASTNode* node_5 = create_integer_node(5);

    // Create the node for 'b * c'
    ASTNode* node_b_mul_c = create_binary_operator_node(AST_MUL, node_b, node_c);

    // Create the node for 'a + (b * c)'
    ASTNode* node_a_plus_b_mul_c = create_binary_operator_node(AST_PLUS, node_a, node_b_mul_c);

    // Create the node for '(a + b * c) - 5'
    ASTNode* root_node = create_binary_operator_node(AST_MINUS, node_a_plus_b_mul_c, node_5);

    // Open the output file
    FILE* output_file = fopen("testCG.out", "w");
    if (output_file == NULL) {
        perror("Failed to open testCG.out for writing");
        exit(EXIT_FAILURE);
    }

    // Output a confirmation message to the console
    printf("AST for expression has been successfully created.\n");

    // Print the AST to the file
    fprintf(output_file, "Abstract Syntax Tree for:\n");
    print_ast(root_node, 0, output_file);

    // Close the output file
    fclose(output_file);

    // Clean up and free the allocated memory
    free_ast_node(root_node);

    return 0;
}


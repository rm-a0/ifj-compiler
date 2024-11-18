#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "../inc/ast.h"  // Include the header file containing ASTNode definitions
#include "../inc/generator.h"
#include "../src/generator.c"

/**
 * @brief Function that appends an argument node to a function call node
 *
 * Reallocates memory for the pointer array of arguments inside the function call node
 * if the argument count reaches the argument capacity and appends the new argument to the array.
 *
 * @param[in, out] fn_call_node Pointer to a function call node
 * @param[in] arg_node Pointer to an argument node
 * @return 0 if success, otherwise return 1
 */
int append_arg_to_fn_call(ASTNode* fn_call_node, ASTNode* arg_node) {
    if (fn_call_node->type != AST_FN_CALL) {
        return 1; // Error: node is not a function call
    }
    if (fn_call_node->FnCall.arg_count >= fn_call_node->FnCall.arg_capacity) {
        int new_capacity = fn_call_node->FnCall.arg_capacity * 2;
        ASTNode** new_args = realloc(fn_call_node->FnCall.args, sizeof(ASTNode*) * new_capacity);
        if (new_args == NULL) {
            return 1; // Allocation failed
        }
        fn_call_node->FnCall.args = new_args;
        fn_call_node->FnCall.arg_capacity = new_capacity;
    }
    fn_call_node->FnCall.args[fn_call_node->FnCall.arg_count++] = arg_node;
    return 0;
}

/**
 * @brief Converts DataType enum to its string representation
 * @param data_type The DataType enum value
 * @return String representation of the data type
 */
const char* data_type_to_string(DataType data_type) {
    switch (data_type) {
        case AST_I32: return "i32";
        case AST_F64: return "f64";
        case AST_U8:
        case AST_SLICE: return "[]u8";
        case AST_VOID: return "void";
        // Add other data types as needed
        default: return "UNKNOWN_TYPE";
    }
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
        case AST_PROGRAM:
            print_indent(indent_level, output_file);
            fprintf(output_file, "Program Node:\n");
            print_indent(indent_level, output_file);
            fprintf(output_file, "Has Prolog: %s\n", node->Program.has_prolog ? "Yes" : "No");
            print_indent(indent_level, output_file);
            fprintf(output_file, "Declaration Count: %d\n", node->Program.decl_count);
            print_indent(indent_level, output_file);
            fprintf(output_file, "Declarations:\n");
            for (int i = 0; i < node->Program.decl_count; ++i) {
                print_ast(node->Program.declarations[i], indent_level + 1, output_file);
            }
            break;

        case AST_FN_DECL:
            print_indent(indent_level, output_file);
            fprintf(output_file, "Function Declaration:\n");
            print_indent(indent_level + 1, output_file);
            fprintf(output_file, "Function Name: %s\n", node->FnDecl.fn_name);
            print_indent(indent_level + 1, output_file);
            fprintf(output_file, "Return Type: %s\n", data_type_to_string(node->FnDecl.return_type));
            print_indent(indent_level + 1, output_file);
            fprintf(output_file, "Parameters (%d):\n", node->FnDecl.param_count);
            for (int i = 0; i < node->FnDecl.param_count; ++i) {
                print_ast(node->FnDecl.params[i], indent_level + 2, output_file);
            }
            print_indent(indent_level + 1, output_file);
            fprintf(output_file, "Function Body:\n");
            print_ast(node->FnDecl.block, indent_level + 2, output_file);
            break;

        case AST_PARAM:
            print_indent(indent_level, output_file);
            fprintf(output_file, "Parameter:\n");
            print_indent(indent_level + 1, output_file);
            fprintf(output_file, "Data Type: %s\n", data_type_to_string(node->Param.data_type));
            print_indent(indent_level + 1, output_file);
            fprintf(output_file, "Identifier: %s\n", node->Param.identifier);
            break;

        case AST_VAR_DECL:
            print_indent(indent_level, output_file);
            fprintf(output_file, "Variable Declaration:\n");
            print_indent(indent_level + 1, output_file);
            fprintf(output_file, "Variable Name: %s\n", node->VarDecl.var_name);
            print_indent(indent_level + 1, output_file);
            fprintf(output_file, "Data Type: %s\n", data_type_to_string(node->VarDecl.data_type));
            print_indent(indent_level + 1, output_file);
            fprintf(output_file, "Expression:\n");
            print_ast(node->VarDecl.expression, indent_level + 2, output_file);
            break;

        case AST_CONST_DECL:
            print_indent(indent_level, output_file);
            fprintf(output_file, "Constant Declaration:\n");
            print_indent(indent_level + 1, output_file);
            fprintf(output_file, "Constant Name: %s\n", node->ConstDecl.const_name);
            print_indent(indent_level + 1, output_file);
            fprintf(output_file, "Data Type: %s\n", data_type_to_string(node->ConstDecl.data_type));
            print_indent(indent_level + 1, output_file);
            fprintf(output_file, "Expression:\n");
            print_ast(node->ConstDecl.expression, indent_level + 2, output_file);
            break;

        case AST_BLOCK:
            print_indent(indent_level, output_file);
            fprintf(output_file, "Block Node:\n");
            print_indent(indent_level + 1, output_file);
            fprintf(output_file, "Node Count: %d\n", node->Block.node_count);
            for (int i = 0; i < node->Block.node_count; ++i) {
                print_ast(node->Block.nodes[i], indent_level + 1, output_file);
            }
            break;

        case AST_WHILE:
            print_indent(indent_level, output_file);
            fprintf(output_file, "While Cycle:\n");
            print_indent(indent_level + 1, output_file);
            fprintf(output_file, "Condition Expression:\n");
            print_ast(node->WhileCycle.expression, indent_level + 2, output_file);
            if (node->WhileCycle.element_bind) {
                print_indent(indent_level + 1, output_file);
                fprintf(output_file, "Element Bind: %s\n", node->WhileCycle.element_bind);
            }
            print_indent(indent_level + 1, output_file);
            fprintf(output_file, "Loop Body:\n");
            print_ast(node->WhileCycle.block, indent_level + 2, output_file);
            break;

        case AST_IF_ELSE:
            print_indent(indent_level, output_file);
            fprintf(output_file, "If-Else Statement:\n");
            print_indent(indent_level + 1, output_file);
            fprintf(output_file, "Condition Expression:\n");
            print_ast(node->IfElse.expression, indent_level + 2, output_file);
            if (node->IfElse.element_bind) {
                print_indent(indent_level + 1, output_file);
                fprintf(output_file, "Element Bind: %s\n", node->IfElse.element_bind);
            }
            print_indent(indent_level + 1, output_file);
            fprintf(output_file, "If Block:\n");
            print_ast(node->IfElse.if_block, indent_level + 2, output_file);
            if (node->IfElse.else_block) {
                print_indent(indent_level + 1, output_file);
                fprintf(output_file, "Else Block:\n");
                print_ast(node->IfElse.else_block, indent_level + 2, output_file);
            }
            break;

        case AST_FN_CALL:
            print_indent(indent_level, output_file);
            fprintf(output_file, "Function Call:\n");
            print_indent(indent_level + 1, output_file);
            fprintf(output_file, "Function Name: %s\n", node->FnCall.fn_name);
            print_indent(indent_level + 1, output_file);
            fprintf(output_file, "Arguments (%d):\n", node->FnCall.arg_count);
            for (int i = 0; i < node->FnCall.arg_count; ++i) {
                print_ast(node->FnCall.args[i], indent_level + 2, output_file);
            }
            break;

        case AST_ARG:
            print_indent(indent_level, output_file);
            fprintf(output_file, "Argument:\n");
            print_indent(indent_level + 1, output_file);
            fprintf(output_file, "Expression:\n");
            print_ast(node->Argument.expression, indent_level + 2, output_file);
            break;

        case AST_RETURN:
            print_indent(indent_level, output_file);
            fprintf(output_file, "Return Statement:\n");
            print_indent(indent_level + 1, output_file);
            fprintf(output_file, "Expression:\n");
            print_ast(node->Return.expression, indent_level + 2, output_file);
            break;

        case AST_BIN_OP:
            print_indent(indent_level, output_file);
            fprintf(output_file, "Binary Operator: %s\n", operator_type_to_string(node->BinaryOperator.operator));
            print_indent(indent_level + 1, output_file);
            fprintf(output_file, "Left Operand:\n");
            print_ast(node->BinaryOperator.left, indent_level + 2, output_file);
            print_indent(indent_level + 1, output_file);
            fprintf(output_file, "Right Operand:\n");
            print_ast(node->BinaryOperator.right, indent_level + 2, output_file);
            break;

        case AST_FLOAT:
            print_indent(indent_level, output_file);
            fprintf(output_file, "Float: %f\n", node->Float.number);
            break;

        case AST_INT:
            print_indent(indent_level, output_file);
            fprintf(output_file, "Integer: %d\n", node->Integer.number);
            break;

        case AST_STRING:
            print_indent(indent_level, output_file);
            fprintf(output_file, "String: \"%s\"\n", node->String.string);
            break;

        case AST_IDENTIFIER:
            print_indent(indent_level, output_file);
            fprintf(output_file, "Identifier: %s\n", node->Identifier.identifier);
            break;

        case AST_EXPRESSION:
            print_indent(indent_level, output_file);
            fprintf(output_file, "Expression Node:\n");
            print_indent(indent_level + 1, output_file);
            fprintf(output_file, "Binary Operator:\n");
            print_ast(node->Expression.binary_operator, indent_level + 2, output_file);
            break;

        default:
            print_indent(indent_level, output_file);
            fprintf(output_file, "Unknown node type: %d\n", node->type);
            break;
    }
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s -[test_number]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int test_number = atoi(argv[1] + 1); // Parsovanie čísla testu z argumentu, napr. "-1"
    if (test_number < 1 || test_number > 3) {
        fprintf(stderr, "Invalid test number: %s. Use -1, -2, or -3.\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    ASTNode* program_node = NULL;

    switch (test_number) {
        case 1:
            // Test 1: Pôvodný testovací kód
            program_node = create_program_node();

            // 'build' function declaration
            ASTNode* build_fn = create_fn_decl_node("build");
            build_fn->FnDecl.return_type = AST_SLICE;

            ASTNode* param_x = create_param_node(AST_SLICE, "x");
            ASTNode* param_y = create_param_node(AST_SLICE, "y");
            append_param_to_fn(build_fn, param_x);
            append_param_to_fn(build_fn, param_y);

            ASTNode* build_block = create_block_node();
            build_fn->FnDecl.block = build_block;

            ASTNode* concat_call = create_fn_call_node("ifj.concat");
            append_arg_to_fn_call(concat_call, create_identifier_node("x"));
            append_arg_to_fn_call(concat_call, create_identifier_node("y"));

            ASTNode* const_res = create_const_decl_node(AST_SLICE, "res");
            const_res->ConstDecl.expression = concat_call;
            append_node_to_block(build_block, const_res);

            ASTNode* return_node = create_return_node();
            return_node->Return.expression = create_identifier_node("res");
            append_node_to_block(build_block, return_node);

            append_decl_to_prog(program_node, build_fn);

            // 'main' function declaration
            ASTNode* main_fn = create_fn_decl_node("main");
            main_fn->FnDecl.return_type = AST_VOID;

            ASTNode* main_block = create_block_node();
            main_fn->FnDecl.block = main_block;

            ASTNode* string_call_ahoj = create_fn_call_node("ifj.string");
            append_arg_to_fn_call(string_call_ahoj, create_string_node("ahoj "));

            ASTNode* const_a = create_const_decl_node(AST_SLICE, "a");
            const_a->ConstDecl.expression = string_call_ahoj;
            append_node_to_block(main_block, const_a);

            ASTNode* string_call_svete = create_fn_call_node("ifj.string");
            append_arg_to_fn_call(string_call_svete, create_string_node("svete"));

            ASTNode* var_ct = create_var_decl_node(AST_SLICE, "ct");
            var_ct->VarDecl.expression = string_call_svete;
            append_node_to_block(main_block, var_ct);

            ASTNode* build_call = create_fn_call_node("build");
            append_arg_to_fn_call(build_call, create_identifier_node("a"));
            append_arg_to_fn_call(build_call, create_identifier_node("ct"));

            ASTNode* assign_ct = create_var_decl_node(AST_SLICE, "ct");
            assign_ct->VarDecl.expression = build_call;
            append_node_to_block(main_block, assign_ct);

            ASTNode* write_call = create_fn_call_node("ifj.write");
            append_arg_to_fn_call(write_call, create_identifier_node("ct"));
            append_node_to_block(main_block, write_call);

            append_decl_to_prog(program_node, main_fn);
            break;

        case 2:
            // Test 2: Načítanie dvoch reťazcov, ich konkatenácia a výpis
            program_node = create_program_node();

            ASTNode* main_fn_2 = create_fn_decl_node("main");
            main_fn_2->FnDecl.return_type = AST_VOID;

            ASTNode* main_block_2 = create_block_node();
            main_fn_2->FnDecl.block = main_block_2;

            ASTNode* readstr_call_1 = create_fn_call_node("ifj.readstr");
            ASTNode* var_str1 = create_var_decl_node(AST_SLICE, "str1");
            var_str1->VarDecl.expression = readstr_call_1;
            append_node_to_block(main_block_2, var_str1);

            ASTNode* readstr_call_2 = create_fn_call_node("ifj.readstr");
            ASTNode* var_str2 = create_var_decl_node(AST_SLICE, "str2");
            var_str2->VarDecl.expression = readstr_call_2;
            append_node_to_block(main_block_2, var_str2);

            ASTNode* concat_call_2 = create_fn_call_node("ifj.concat");
            append_arg_to_fn_call(concat_call_2, create_identifier_node("str1"));
            append_arg_to_fn_call(concat_call_2, create_identifier_node("str2"));

            ASTNode* var_result = create_var_decl_node(AST_SLICE, "result");
            var_result->VarDecl.expression = concat_call_2;
            append_node_to_block(main_block_2, var_result);

            ASTNode* write_call_2 = create_fn_call_node("ifj.write");
            append_arg_to_fn_call(write_call_2, create_identifier_node("result"));
            append_node_to_block(main_block_2, write_call_2);

            append_decl_to_prog(program_node, main_fn_2);
            break;

        case 3:
            // Test 3: Načítanie dvoch hodnôt a ich výpis
            program_node = create_program_node();

            ASTNode* main_fn_3 = create_fn_decl_node("main");
            main_fn_3->FnDecl.return_type = AST_VOID;

            ASTNode* main_block_3 = create_block_node();
            main_fn_3->FnDecl.block = main_block_3;

            ASTNode* read32_call = create_fn_call_node("ifj.read32");
            ASTNode* var_num1 = create_var_decl_node(AST_I32, "num1");
            var_num1->VarDecl.expression = read32_call;
            append_node_to_block(main_block_3, var_num1);

            ASTNode* read32_call_2 = create_fn_call_node("ifj.read32");
            ASTNode* var_num2 = create_var_decl_node(AST_I32, "num2");
            var_num2->VarDecl.expression = read32_call_2;
            append_node_to_block(main_block_3, var_num2);

            ASTNode* write_call_3 = create_fn_call_node("ifj.write");
            append_arg_to_fn_call(write_call_3, create_identifier_node("num1"));
            append_node_to_block(main_block_3, write_call_3);

            ASTNode* write_call_4 = create_fn_call_node("ifj.write");
            append_arg_to_fn_call(write_call_4, create_identifier_node("num2"));
            append_node_to_block(main_block_3, write_call_4);

            append_decl_to_prog(program_node, main_fn_3);
            break;
    }

    // Open the output file
    FILE* output_file = fopen("testCG.out", "w");
    if (output_file == NULL) {
        perror("Failed to open testCG.out for writing");
        exit(EXIT_FAILURE);
    }

    // Print the AST to the output file
    fprintf(output_file, "Abstract Syntax Tree for Test %d:\n", test_number);
    print_ast(program_node, 0, output_file);
    fclose(output_file);

    // Generate code
    printf("\033[44mGenerated code for Test %d:\033[0m\n", test_number);
    generate_code(program_node);

    return 0;
}


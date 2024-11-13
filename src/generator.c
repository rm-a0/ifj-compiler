/**
 * @file parser.c
 * @brief Implementation of a generator.
 * @authors Martin Kandera (xkande00)
*/

#include "../inc/generator.h"
#include "../inc/generator_instructions.h"



void generate_code_in_node(ASTNode* node){
    if (node == NULL) return;

    switch (node->type) {
        case AST_PROGRAM:
            for (int i = 0; i < node->Program.decl_count; ++i) {
                generate_code_in_node(node->Program.declarations[i]);
                print_new_line();
            }
            break;

        case AST_FN_DECL:
            label(node->FnDecl.fn_name);
            create_frame();
            push_frame();
            for (int i = 0; i < node->FnDecl.param_count; ++i) {
                generate_code_in_node(node->FnDecl.params[i]);
            }
            generate_code_in_node(node->FnDecl.block);
            pop_frame();
            return_f();
            break;

        case AST_PARAM:
            def_var(node->Param.identifier);
            break;

        case AST_VAR_DECL:
            def_var(node->VarDecl.var_name);
            if (node->VarDecl.expression) {
                generate_code_in_node(node->VarDecl.expression);
                pops(node->VarDecl.var_name);
            }
            break;

        case AST_CONST_DECL:
            def_var(node->ConstDecl.const_name);
            if (node->ConstDecl.expression) {
                generate_code_in_node(node->ConstDecl.expression);
                pops(node->ConstDecl.const_name);
            }
            break;

        case AST_BLOCK:
            for (int i = 0; i < node->Block.node_count; ++i) {
                generate_code_in_node(node->Block.nodes[i]);
            }
            break;

        case AST_FN_CALL:
            for (int i = 0; i < node->FnCall.arg_count; ++i) {
                generate_code_in_node(node->FnCall.args[i]);
            }
            call(node->FnCall.fn_name);
            break;

        case AST_ARG:
            generate_code_in_node(node->Argument.expression);
            break;

        case AST_RETURN:
            generate_code_in_node(node->Return.expression);
            break;

        case AST_BIN_OP:
            generate_code_in_node(node->BinaryOperator.left);
            generate_code_in_node(node->BinaryOperator.right);
            switch (node->BinaryOperator.operator) {
                case AST_PLUS: add_s("result", "LF@left", "LF@right"); break;
                case AST_MINUS: sub_s("result", "LF@left", "LF@right"); break;
                case AST_MUL: mul_s("result", "LF@left", "LF@right"); break;
                case AST_DIV: div_s("result", "LF@left", "LF@right"); break;
                default: print_with_indent("UNKNOWN_OPERATOR"); break;
            }
            break;

        case AST_INT:
            printf("PUSHS int@%d\n", node->Integer.number);
            break;

        case AST_FLOAT:
            printf("PUSHS float@%f\n", node->Float.number);
            break;

        case AST_STRING:
            printf("PUSHS string@%s\n", node->String.string);
            break;

        case AST_IDENTIFIER:
            printf("PUSHS LF@%s\n", node->Identifier.identifier);
            break;

        default:
            print_with_indent("UNKNOWN_NODE_TYPE");
            break;
    }
}






int generate_code(ASTNode* root){
    if (root == NULL) return 51; // ast root == NULL

    printf(".IFJcode24\n");
    generate_code_in_node(root);
    /*
     * for (int i = 0; i < root->Program.decl_count; ++i) {
        printf("deklaracia #%i\n", (root->Program.decl_count - i));
    }
     */

    return 0;
}

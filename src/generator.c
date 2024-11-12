/**
 * @file parser.c
 * @brief Implementation of a generator.
 * @authors Martin Kandera (xkande00)
*/

#include "../inc/generator.h"



void generate_code_in_node(ASTNode* node){
    if (node == NULL) return;

    switch (node->type) {
        case AST_PROGRAM:
            for (int i = 0; i < node->Program.decl_count; ++i) {
                generate_code_in_node(node->Program.declarations[i]);
            }
            break;
        case AST_FN_DECL:
            label(node->FnDecl.fn_name);
            create_frame();
            push_frame();
            //TODO: generate_code_in_node(node->FnDecl.body);
            pop_frame();
            print_new_line();
            break;
        case AST_VAR_DECL:
            def_var(node->VarDecl.var_name);
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

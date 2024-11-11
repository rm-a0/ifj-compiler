/**
 * @file parser.c
 * @brief Implementation of a generator.
 * @authors Martin Kandera (xkande00)
*/

#include "../inc/generator.h"

void generate_assignment(ASTNode* node){

}

void generate_expression(ASTNode* node){

}

void generate_if(ASTNode* node){

}

void generate_loop(ASTNode* node){

}
void find_internal_symbols(ASTNode* node){

}

int generate_code(ASTNode* root){
    if (root == NULL) return 51; // ast root == NULL

    for (int i = 0; i < root->Program.decl_count; ++i) {
        printf("deklaracia #%i\n", (root->Program.decl_count - i));


    }

    return 0;
}

#include <stdio.h>
#include <string.h>
#include "../inc/parser.h"
#include "../inc/ast.h"
#include "../inc/stack.h"

// 1. push frames functions to the root stack + check if there is a main function without params
// 2. iterate over root stack, for every frame create its own ScopeStack

void semantic_analysis(ASTNode* program) {
    switch (program->type) {
        case AST_PROGRAM:
            // Pre-order
            RootStack *root_stack = init_root_stack();

            // Traverse children (Postorder)
            int number_of_functions = program->Program.decl_count;

            for (int i = 0; i < number_of_functions; i++) {
                // add_function(root_stack, );
            }

            // semantic_analysis(root_stack->functions[i]);
            // exit_root_decl();
            break;

        case AST_FN_DECL:


        // case NODE_VARIABLE_DECLARATION:
        //     // Preorder: Add variable to symbol table
        //     add_variable_symbol(node->variableName, node->type);
        //     break;

        // case NODE_EXPRESSION:
        //     // Postorder: Evaluate child nodes first
        //     for (int i = 0; i < node->childCount; i++) {
        //         semantic_analysis(node->children[i]);
        //     }

        //     // Perform type checking or other semantic checks here
        //     check_expression_type(node);
        //     break;

        // // Handle other node types (e.g., loops, conditionals, etc.)
    }
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../inc/ast.h"
#include "../../inc/symtable.h"
#include "../../inc/semantic_analysis.h"
#include "../../inc/stack.h"

int main() {
    // Initialize global symbol table
    SymbolTable *global_table = init_symbol_table();

    // Create the root node for the program
    ASTNode *program_node = create_program_node();

    // Create the main function declaration node
    ASTNode *main_fn_node = create_fn_decl_node("main");
    main_fn_node->FnDecl.return_type = AST_VOID;
    main_fn_node->FnDecl.block = create_block_node();

    // Create the ifj.write(str1) function call node
    ASTNode *fn_call_node = create_fn_call_node("ifj.write");

    // Create the argument for the function call (str1)
    ASTNode *arg_node = create_arg_node();
    arg_node->Argument.expression = create_identifier_node("str1");

    // Append the argument to the function call
    append_arg_to_fn(fn_call_node, arg_node);

    // Add the function call to the main function's block
    append_node_to_block(main_fn_node->FnDecl.block, fn_call_node);

    // Add the main function to the program
    append_decl_to_prog(program_node, main_fn_node);

    // Perform semantic analysis on the program
    printf("Starting semantic analysis...\n");
    ScopeStack *local_stack = init_scope_stack();

    // Run semantic analysis and expect it to fail due to undefined variable "str1"
    semantic_analysis(program_node, global_table, local_stack);

    printf("Semantic analysis completed successfully (this should not happen).\n");

    // Cleanup
    free_ast_node(program_node);
    free_symbol_table(global_table);
    free_scope_stack(local_stack);

    return 0;
}

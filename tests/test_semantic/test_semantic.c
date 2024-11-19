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

    // Create a constant declaration node for `ifj`
    ASTNode *ifj_const_node = create_const_decl_node(AST_UNSPECIFIED, "ifj");
    // Simulate the import by assigning an expression (e.g., a placeholder string)
    ifj_const_node->ConstDecl.expression = create_string_node("@import(\"ifj24.zig\")");
    append_decl_to_prog(program_node, ifj_const_node);

    // Create a global variable declaration for `x`
    ASTNode *var_decl_x = create_var_decl_node(AST_I32, "x");
    var_decl_x->VarDecl.expression = create_i32_node(1);
    append_decl_to_prog(program_node, var_decl_x);

    // Create the main function declaration node
    ASTNode *main_fn_node = create_fn_decl_node("main");
    main_fn_node->FnDecl.return_type = AST_VOID;
    main_fn_node->FnDecl.block = create_block_node();

    // Add a parameter `y : i32` to the main function
    ASTNode *param_y = create_param_node(AST_I32, "y");
    append_param_to_fn(main_fn_node, param_y);

    // Create a variable declaration for `z` in the main function
    ASTNode *var_decl_z = create_var_decl_node(AST_I32, "z");

    // Create the expression `x + y`
    ASTNode *binary_op = create_binary_op_node(AST_PLUS, create_identifier_node("x"), create_identifier_node("y"));
    var_decl_z->VarDecl.expression = binary_op;

    // Add the variable declaration `z` to the main function's block
    append_node_to_block(main_fn_node->FnDecl.block, var_decl_z);

    // Create an `if` statement with the condition `z < 10`
    ASTNode *if_node = create_if_node();
    if_node->IfElse.expression = create_binary_op_node(AST_LESS, create_identifier_node("z"), create_i32_node(10));

    // Create the block for the `if` statement
    if_node->IfElse.if_block = create_block_node();

    // Create a recursive call to `main(z)`
    ASTNode *recursive_call = create_fn_call_node("main");

    // Add `z` as an argument to the recursive call
    ASTNode *arg_node_z = create_arg_node();
    arg_node_z->Argument.expression = create_identifier_node("z");
    append_arg_to_fn(recursive_call, arg_node_z);

    // Add the recursive call to the `if` block
    append_node_to_block(if_node->IfElse.if_block, recursive_call);

    // Add the `if` statement to the main function's block
    append_node_to_block(main_fn_node->FnDecl.block, if_node);

    // Add the `main` function to the program
    append_decl_to_prog(program_node, main_fn_node);

    // Perform semantic analysis on the program
    printf("Starting semantic analysis...\n");
    ScopeStack *local_stack = init_scope_stack();

    // Run semantic analysis (expect it to succeed with the recursive function)
    semantic_analysis(program_node, global_table, local_stack);
    printf("Semantic analysis completed successfully.\n");

    // Cleanup
    free_ast_node(program_node);
    free_symbol_table(global_table);
    free_scope_stack(local_stack);

    return 0;
}

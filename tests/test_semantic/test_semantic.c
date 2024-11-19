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

    // Create a function declaration node for `sum`
    ASTNode *sum_fn_node = create_fn_decl_node("sum");
    sum_fn_node->FnDecl.return_type = AST_I32;
    sum_fn_node->FnDecl.block = create_block_node();

    // Add parameters `x` and `y` to the `sum` function
    ASTNode *x_param_node = create_param_node(AST_I32, "x");
    ASTNode *y_param_node = create_param_node(AST_I32, "y");
    append_param_to_fn(sum_fn_node, x_param_node);
    append_param_to_fn(sum_fn_node, y_param_node);

    // Add the return statement `return x + y;` to the `sum` function
    ASTNode *return_node = create_return_node();
    ASTNode *binary_op_node = create_binary_op_node(AST_PLUS, create_identifier_node("x"), create_identifier_node("y"));
    return_node->Return.expression = binary_op_node;
    append_node_to_block(sum_fn_node->FnDecl.block, return_node);

    // Add the `sum` function to the program
    append_decl_to_prog(program_node, sum_fn_node);

    // Create the `main` function declaration node
    ASTNode *main_fn_node = create_fn_decl_node("main");
    main_fn_node->FnDecl.return_type = AST_VOID;
    main_fn_node->FnDecl.block = create_block_node();

    // Add the variable declaration `var a = sum(1, 2.5);` to the `main` function
    ASTNode *var_decl_node = create_var_decl_node(AST_UNSPECIFIED, "a");
    ASTNode *sum_call_node = create_fn_call_node("sum");

    // Add arguments `1` and `2.5` to the `sum` function call
    ASTNode *arg1_node = create_arg_node();
    arg1_node->Argument.expression = create_i32_node(1);
    append_arg_to_fn(sum_call_node, arg1_node);

    ASTNode *arg2_node = create_arg_node();
    arg2_node->Argument.expression = create_f64_node(2.5); // Incorrect type (expected i32)
    append_arg_to_fn(sum_call_node, arg2_node);

    // Assign the function call to the variable declaration
    var_decl_node->VarDecl.expression = sum_call_node;
    append_node_to_block(main_fn_node->FnDecl.block, var_decl_node);

    // Add the `main` function to the program
    append_decl_to_prog(program_node, main_fn_node);

    // Perform semantic analysis on the program
    printf("Starting semantic analysis...\n");
    ScopeStack *local_stack = init_scope_stack();

    // Run semantic analysis (expect it to fail due to wrong parameter type in `sum` function call)
    semantic_analysis(program_node, global_table, local_stack);
    printf("Semantic analysis completed successfully (this should not happen).\n");

    // Cleanup
    free_ast_node(program_node);
    free_symbol_table(global_table);
    free_scope_stack(local_stack);

    return 0;
}

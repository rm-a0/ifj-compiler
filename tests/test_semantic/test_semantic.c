#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../inc/ast.h"
#include "../../inc/symtable.h"
#include "../../inc/semantic_analysis.h"
#include "../../inc/stack.h"
#include "../../inc/token.h"

int main() {
    // Initialize global symbol table
    SymbolTable *global_table = init_symbol_table();

    // Create the root node for the program
    ASTNode *program_node = create_program_node();

    // Import declaration for `ifj`
    ASTNode *ifj_import_node = create_const_decl_node(AST_UNSPECIFIED, "ifj");
    ifj_import_node->ConstDecl.expression = create_string_node("@import(\"ifj24.zig\")");
    append_decl_to_prog(program_node, ifj_import_node);

    // Function declaration for `sum`
    ASTNode *sum_fn_node = create_fn_decl_node("sum");
    sum_fn_node->FnDecl.return_type = AST_I32;
    sum_fn_node->FnDecl.block = create_block_node();

    // Add parameters to `sum`
    ASTNode *param_x = create_param_node(AST_I32, "x");
    ASTNode *param_y = create_param_node(AST_I32, "y");
    append_param_to_fn(sum_fn_node, param_x);
    append_param_to_fn(sum_fn_node, param_y);

    // Add return statement to `sum`
    ASTNode *return_node = create_return_node();
    ASTNode *binary_op_node = create_binary_op_node(TOKEN_PLUS, create_identifier_node("x"), create_identifier_node("y"));
    return_node->Return.expression = binary_op_node;
    append_node_to_block(sum_fn_node->FnDecl.block, return_node);

    // Add `sum` function to the program
    append_decl_to_prog(program_node, sum_fn_node);

    // Function declaration for `main`
    ASTNode *main_fn_node = create_fn_decl_node("main");
    main_fn_node->FnDecl.return_type = AST_VOID;
    main_fn_node->FnDecl.block = create_block_node();

    // Declare `x` as i32
    ASTNode *var_x_node = create_var_decl_node(AST_I32, "x");
    var_x_node->VarDecl.expression = create_i32_node(1);
    append_node_to_block(main_fn_node->FnDecl.block, var_x_node);

    // Declare `y` as f64
    ASTNode *var_y_node = create_var_decl_node(AST_F64, "y");
    var_y_node->VarDecl.expression = create_f64_node(2.5);
    append_node_to_block(main_fn_node->FnDecl.block, var_y_node);

    // Declare `a` and assign result of `sum(x, y)`
    ASTNode *var_a_node = create_var_decl_node(AST_UNSPECIFIED, "a");
    ASTNode *sum_call_node = create_fn_call_node("sum");

    // Add arguments to the `sum` function call
    ASTNode *arg_x = create_arg_node();
    arg_x->Argument.expression = create_identifier_node("x");
    append_arg_to_fn(sum_call_node, arg_x);

    ASTNode *arg_y = create_arg_node();
    arg_y->Argument.expression = create_identifier_node("y");
    append_arg_to_fn(sum_call_node, arg_y);

    var_a_node->VarDecl.expression = sum_call_node;
    append_node_to_block(main_fn_node->FnDecl.block, var_a_node);

    // Add `main` function to the program
    append_decl_to_prog(program_node, main_fn_node);

    // Perform semantic analysis
    printf("Starting semantic analysis...\n");
    ScopeStack *local_stack = init_scope_stack();

    semantic_analysis(program_node, global_table, local_stack);

    printf("Semantic analysis completed successfully.\n");

    // Cleanup
    free_ast_node(program_node);
    free_symbol_table(global_table);
    free_scope_stack(local_stack);

    return 0;
}

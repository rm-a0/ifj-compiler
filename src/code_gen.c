#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "symtable.h"
#include "code_gen.h"

// Global variables for code generation
int label_counter = 0;               // For generating unique labels
SymbolTable* global_symtable = NULL; // Global symbol table


// ---------- HELPER FUNCTIONS ------------ //

char* generate_unique_label(const char* base) {
    char* label = malloc(64);
    sprintf(label, "%s_%d", base, label_counter++);
    return label;
}


void generate_function_declaration(ASTNode* fn_decl_node) {
    const char* fn_name = fn_decl_node->FnDecl.fn_name;

    // Label for the function
    printf("\nLABEL %s\n", fn_name);
    printf("PUSHFRAME\n");

    // Define parameters in LF
    for (int i = 0; i < fn_decl_node->FnDecl.param_count; i++) {
        ASTNode* param = fn_decl_node->FnDecl.params[i];
        const char* param_name = param->Param.identifier;

        // Add parameter to symbol table
        add_variable_symbol(global_symtable, param_name, param->Param.data_type, false, 0);

        // Declare parameter in LF
        printf("DEFVAR LF@%s\n", param_name);
        printf("MOVE LF@%s LF@%s\n", param_name, param_name);
    }

    // Generate code for the function body
    generate_block(fn_decl_node->FnDecl.block);

    // Function return
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void generate_global_variable(ASTNode* var_decl_node) {
    const char* var_name = var_decl_node->VarDecl.var_name;

    // Declare variable in GF
    printf("DEFVAR GF@%s\n", var_name);

    // If there's an initialization expression
    if (var_decl_node->VarDecl.expression != NULL) {
        // Generate code for the expression
        generate_expression(var_decl_node->VarDecl.expression);

        // Pop the value from the stack and move it to the variable
        printf("POPS GF@%s\n", var_name);
    } else {
        // Initialize variable with a default value (e.g., nil)
        printf("MOVE GF@%s nil@nil\n", var_name);
    }
}

void generate_global_constant(ASTNode* const_decl_node) {
    const char* const_name = const_decl_node->ConstDecl.const_name;

    // Declare constant in GF
    printf("DEFVAR GF@%s\n", const_name);

    // Generate code for the expression (constants must have an initialization)
    if (const_decl_node->ConstDecl.expression != NULL) {
        // Generate code for the expression
        generate_expression(const_decl_node->ConstDecl.expression);

        // Pop the value from the stack and move it to the constant
        printf("POPS GF@%s\n", const_name);
    } else {
        // Error: Constants must be initialized
        fprintf(stderr, "Error: Constant '%s' must be initialized.\n", const_name);
        exit(1);
    }
}

void generate_block(ASTNode* block_node) {
    for (int i = 0; i < block_node->Block.node_count; i++) {
        ASTNode* stmt = block_node->Block.nodes[i];
        generate_statement(stmt);
    }
}

void generate_statement(ASTNode* stmt_node) {
    switch (stmt_node->type) {
        case AST_VAR_DECL:
            generate_local_variable(stmt_node);
            break;

        case AST_ASSIGNMENT:
            generate_assignment(stmt_node);
            break;

        case AST_IF_ELSE:
            generate_if_else(stmt_node);
            break;

        case AST_WHILE:
            generate_while_loop(stmt_node);
            break;

        case AST_FN_CALL:
            generate_function_call(stmt_node);
            break;

        case AST_RETURN:
            generate_return(stmt_node);
            break;

        // Handle other statement types

        default:
            fprintf(stderr, "Error: Unknown statement type.\n");
            exit(1);
    }
}

void generate_local_variable(ASTNode* var_decl_node) {
    const char* var_name = var_decl_node->VarDecl.var_name;

    // Declare variable in LF
    printf("DEFVAR LF@%s\n", var_name);

    // Add variable to symbol table
    add_variable_symbol(global_symtable, var_name, var_decl_node->VarDecl.data_type, false, 0);

    // If there's an initialization expression
    if (var_decl_node->VarDecl.expression != NULL) {
        // Generate code for the expression
        generate_expression(var_decl_node->VarDecl.expression);

        // Pop the value from the stack and move it to the variable
        printf("POPS LF@%s\n", var_name);
    } else {
        // Initialize variable with a default value (e.g., nil)
        printf("MOVE LF@%s nil@nil\n", var_name);
    }
}

void generate_assignment(ASTNode* assignment_node) {
    const char* var_name = assignment_node->Assignment.identifier;

    // Lookup the variable in the symbol table to determine its frame
    Symbol* sym = lookup_symbol(global_symtable, var_name);
    if (sym == NULL) {
        fprintf(stderr, "Error: Variable '%s' not declared.\n", var_name);
        exit(1);
    }

    const char* frame = (sym->var.is_constant) ? "GF" : "LF"; // Simplified logic

    // Generate code for the expression
    generate_expression(assignment_node->Assignment.expression);

    // Assign the value to the variable
    printf("POPS %s@%s\n", frame, var_name);
}

void generate_expression(ASTNode* expr_node) {
    switch (expr_node->type) {
        case AST_INT:
            printf("PUSHS int@%d\n", expr_node->Integer.number);
            break;

        case AST_FLOAT:
            printf("PUSHS float@%a\n", expr_node->Float.number);
            break;

        case AST_STRING:
            printf("PUSHS string@%s\n", escape_string(expr_node->String.string));
            break;

        case AST_IDENTIFIER: {
            const char* var_name = expr_node->Identifier.identifier;

            // Lookup the variable to get its frame
            Symbol* sym = lookup_symbol(global_symtable, var_name);
            if (sym == NULL) {
                fprintf(stderr, "Error: Variable '%s' not declared.\n", var_name);
                exit(1);
            }

            const char* frame = (sym->var.is_constant) ? "GF" : "LF"; // Simplified logic

            printf("PUSHS %s@%s\n", frame, var_name);
            break;
        }

        case AST_BIN_OP:
            generate_expression(expr_node->BinaryOperator.left);
            generate_expression(expr_node->BinaryOperator.right);
            generate_binary_operation(expr_node->BinaryOperator.operator);
            break;

        // Handle other expression types

        default:
            fprintf(stderr, "Error: Unknown expression type.\n");
            exit(1);
    }
}

void generate_binary_operation(OperatorType operator) {
    switch (operator) {
        case AST_PLUS:
            printf("ADDS\n");
            break;

        case AST_MINUS:
            printf("SUBS\n");
            break;

        case AST_MUL:
            printf("MULS\n");
            break;

        case AST_DIV:
            printf("DIVS\n");
            break;

        // Handle other operators

        default:
            fprintf(stderr, "Error: Unknown binary operator.\n");
            exit(1);
    }
}

char* escape_string(const char* str) {
    // Allocate a buffer with sufficient size
    size_t len = strlen(str);
    char* escaped = malloc(len * 4 + 1); // Worst case: every character is escaped
    char* dest = escaped;

    for (const char* src = str; *src != '\0'; src++) {
        if (*src == '\\' || *src == '#' || (unsigned char)*src <= 32 || (unsigned char)*src == 127) {
            // Escape character
            dest += sprintf(dest, "\\%03d", (unsigned char)*src);
        } else {
            *dest++ = *src;
        }
    }
    *dest = '\0';
    return escaped;
}

void cleanup_code_generator() {
    // Free the global symbol table
    free_symbol_table(global_symtable);
}


// ---------- HELPER FUNCTIONS ------------ //

void generate_program(ASTNode* program_node, SymbolTable* symbol_table) {
    printf(".IFJcode24\n");
    global_symtable = symbol_table;

    // Traverse the top-level declarations
    for (int i = 0; i < program_node->Program.decl_count; i++) {
        ASTNode* decl = program_node->Program.declarations[i];

        switch (decl->type) {
            case AST_FN_DECL:
                // Add function to symbol table
                add_function_symbol(global_symtable, decl->FnDecl.fn_name, decl->FnDecl.return_type);
                // Generate code for function declaration
                generate_function_declaration(decl);
                break;

            case AST_VAR_DECL:
                // Add global variable to symbol table
                add_variable_symbol(global_symtable, decl->VarDecl.var_name, decl->VarDecl.data_type, false, 0);
                // Generate code for global variable declaration
                generate_global_variable(decl);
                break;

            case AST_CONST_DECL:
                // Add constant to symbol table
                add_variable_symbol(global_symtable, decl->ConstDecl.const_name, decl->ConstDecl.data_type, true, 0);
                // Generate code for constant declaration
                generate_global_constant(decl);
                break;

            default:
                // Handle other possible declarations
                fprintf(stderr, "Error: Unknown top-level declaration.\n");
                exit(1);
        }
    }

    // Optionally, generate code to call the main function if required
    printf("CALL main\n");
    printf("EXIT int@0\n");
}

void generate_function_call(ASTNode* fn_call_node) {
    printf("CREATEFRAME\n");

    // Pass arguments
    for (int i = 0; i < fn_call_node->FnCall.arg_count; i++) {
        ASTNode* arg = fn_call_node->FnCall.args[i];
        generate_expression(arg->Argument.expression);

        // Assume parameter names are arg1, arg2, etc.
        char param_name[10];
        sprintf(param_name, "arg%d", i + 1);

        // Declare parameter in TF
        printf("DEFVAR TF@%s\n", param_name);

        // Pop the value from the stack and move it to TF
        printf("POPS TF@%s\n", param_name);
    }

    // Call the function
    printf("CALL %s\n", fn_call_node->FnCall.fn_name);
}

void generate_if_else(ASTNode* if_else_node) {
    // Generate unique labels
    char* else_label = generate_unique_label("else");
    char* end_label = generate_unique_label("endif");

    // Generate code for the condition expression
    generate_expression(if_else_node->IfElse.expression);

    // Compare the result to bool@true
    printf("PUSHS bool@true\n");
    printf("JUMPIFNEQS %s\n", else_label);

    // Generate code for the true branch (if block)
    generate_block(if_else_node->IfElse.if_block);

    // After the if block, jump to the end label
    printf("JUMP %s\n", end_label);

    // Else label
    printf("LABEL %s\n", else_label);

    // Generate code for the false branch (else block) if it exists
    if (if_else_node->IfElse.else_block != NULL) {
        generate_block(if_else_node->IfElse.else_block);
    }

    // End label
    printf("LABEL %s\n", end_label);

    // Free the allocated labels
    free(else_label);
    free(end_label);
}

void generate_while_loop(ASTNode* while_node) {
    // Generate unique labels
    char* start_label = generate_unique_label("while_start");
    char* end_label = generate_unique_label("while_end");

    // Start label
    printf("LABEL %s\n", start_label);

    // Generate code for the condition expression
    generate_expression(while_node->WhileCycle.expression);

    // Compare the result to bool@true
    printf("PUSHS bool@true\n");
    printf("JUMPIFNEQS %s\n", end_label);

    // Generate code for the loop body
    generate_block(while_node->WhileCycle.block);

    // Jump back to the start of the loop
    printf("JUMP %s\n", start_label);

    // End label
    printf("LABEL %s\n", end_label);

    // Free the allocated labels
    free(start_label);
    free(end_label);
}

void generate_return(ASTNode* return_node) {
    // Assuming functions return value is stored in LF@%retval
    const char* retval_var = "%retval";

    // Check if there's a return expression
    if (return_node->Return.expression != NULL) {
        // Evaluate the return expression
        generate_expression(return_node->Return.expression);

        // Declare the return value variable if not already declared
        // For simplicity, declare it here (ensure it's declared only once)
        printf("DEFVAR LF@%s\n", retval_var);

        // Pop the value from the stack and move it to LF@%retval
        printf("POPS LF@%s\n", retval_var);
    }

    // Clean up and return
    printf("POPFRAME\n");
    printf("RETURN\n");
}





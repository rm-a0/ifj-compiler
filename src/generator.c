/**
 * @file parser.c
 * @brief Implementation of a generator.
 * @authors Martin Kandera (xkande00)
*/

#include "../inc/generator.h"
#include "../inc/generator_instructions.h"

#define MAX_VAR_NAME_LENGTH 128     // Maximálna dĺžka názvu premennej v GF
#define MAX_GF_VAR_COUNT 1024       // Maximálny počet globálnych premenných
#define MAX_LF_VAR_COUNT 1024       // Maximálny počet lokálnych premenných
char* gf_vars[MAX_GF_VAR_COUNT] = {NULL}; // Inicializácia všetkých prvkov na NULL
char* lf_vars[MAX_LF_VAR_COUNT] = {NULL}; // Inicializácia všetkých prvkov na NULL

bool is_it_global(const char* var_name){
    for (int i = 0; i < MAX_GF_VAR_COUNT; ++i) {
        if (gf_vars[i] != NULL && strcmp(gf_vars[i], var_name) == 0) {
            return true;
        }
    }
    return false; // Žiadna zhoda
}
void add_to_global(const char* var_name){
    for (int i = 0; i < MAX_GF_VAR_COUNT; ++i) {
        if (gf_vars[i] == NULL) {
            gf_vars[i] = malloc(MAX_VAR_NAME_LENGTH);
            if (gf_vars[i] == NULL) {
                fprintf(stderr, "ERROR: Memory allocation failed\n");
                exit(99); //interná chyba programu - chyba alokácie pamäte
            }
            strncpy(gf_vars[i], var_name, MAX_VAR_NAME_LENGTH - 1);
            gf_vars[i][MAX_VAR_NAME_LENGTH - 1] = '\0';
            return;
        }
    }
    // Chyba: prekročený limit globálnych premenných
    fprintf(stderr, "ERROR: Exceeded limit of global variables\n");
    exit(54);
}
void free_var_arrays() {
    for (int i = 0; i < MAX_GF_VAR_COUNT; ++i) {
        if (gf_vars[i] != NULL) {
            free(gf_vars[i]);
            gf_vars[i] = NULL;
        }
    }
    for (int i = 0; i < MAX_LF_VAR_COUNT; ++i) {
        if (lf_vars[i] != NULL) {
            free(lf_vars[i]);
            lf_vars[i] = NULL;
        }
    }
}
bool is_it_local(const char* var_name){
    for (int i = 0; i < MAX_LF_VAR_COUNT; ++i) {
        if (lf_vars[i] != NULL && strcmp(lf_vars[i], var_name) == 0) {
            return true;
        }
    }
    return false; // Žiadna zhoda
}
void add_to_local(const char* var_name){
    for(int i = 0; i < MAX_LF_VAR_COUNT; i++){
        if (lf_vars[i] == NULL) {
            lf_vars[i] = malloc(MAX_VAR_NAME_LENGTH);
            if (lf_vars[i] == NULL) {
                fprintf(stderr, "ERROR: Memory allocation failed\n");
                exit(99); //interná chyba programu - chyba alokácie pamäte
            }
            strncpy(lf_vars[i], var_name, MAX_VAR_NAME_LENGTH - 1);
            lf_vars[i][MAX_VAR_NAME_LENGTH - 1] = '\0';
            return;
        }
    }
}


void generate_code_in_node(ASTNode* node){
    if (node == NULL) return;

    switch (node->type) {
        case AST_PROGRAM:
            for (int i = 0; i < node->Program.decl_count; ++i) {
                if (strcmp(node->Program.declarations[i]->FnDecl.fn_name, "main") == 0) {
                    label("main");
                    create_frame();
                    push_frame();
                    generate_code_in_node(node->Program.declarations[i]);  // Generovanie kódu `main`
                    pop_frame();    // ?? netusim ci tu treba naspat pop-ovat ramec
                    printf("EXIT int@0\n");  // Ukončí program ?? - je to nutné?
                }
                else if(node->Program.declarations[i]->type == AST_VAR_DECL)  {
                    def_var(node->Program.declarations[i]->VarDecl.var_name, "gf");
                    add_to_global(node->Program.declarations[i]->VarDecl.var_name);
                    if (node->Program.declarations[i]->VarDecl.expression) {
                        generate_code_in_node(node->Program.declarations[i]->VarDecl.expression);
                        pops(node->Program.declarations[i]->VarDecl.var_name);
                    }
                } else if(node->Program.declarations[i]->type == AST_CONST_DECL) {
                    def_var(node->Program.declarations[i]->ConstDecl.const_name, "gf");
                    add_to_global(node->Program.declarations[i]->ConstDecl.const_name);
                    if (node->Program.declarations[i]->ConstDecl.expression) {
                        generate_code_in_node(node->Program.declarations[i]->ConstDecl.expression);
                        pops(node->Program.declarations[i]->ConstDecl.const_name);
                    }
                }
                else {
                    label(node->Program.declarations[i]->FnDecl.fn_name);
                    push_frame();
                    generate_code_in_node(node->Program.declarations[i]);  // Ostatné funkcie
                    pop_frame();
                    return_f();
                    print_new_line();
                }
            }
            break;

        case AST_FN_DECL:
            for (int i = 0; i < node->FnDecl.param_count; ++i) {
                def_var(node->FnDecl.params[i]->Param.identifier, "lf");
                add_to_local(node->FnDecl.params[i]->Param.identifier);
                pops(node->FnDecl.params[i]->Param.identifier);
            }

            generate_code_in_node(node->FnDecl.block);

            if(node->FnDecl.return_type == AST_VOID) {

            } else {
                //printf("\033[33mPushovanie returnu\033[0m\n");
                //pushs(node->Return.expression->Identifier.identifier);

            }
            break;

        case AST_PARAM:
            def_var(node->Param.identifier, "lf");
            add_to_local(node->Param.identifier);
            // ?? neviem ci toto tu nie je zbytocne
            break;

        case AST_VAR_DECL:
            if (!(is_it_global(node->VarDecl.var_name) || is_it_local(node->VarDecl.var_name))){
                def_var(node->VarDecl.var_name, "lf");
                add_to_local(node->VarDecl.var_name);
            }

            if (node->ConstDecl.expression) {
                if (strcmp(node->ConstDecl.expression->FnCall.fn_name, "ifj.string") == 0) {
                    if (is_it_global(node->ConstDecl.const_name)) {
                        printf("MOVE GF@%s string@%s\n", node->ConstDecl.const_name, node->ConstDecl.expression->FnCall.args[0]->String.string);
                    } else {
                        printf("MOVE LF@%s string@%s\n", node->ConstDecl.const_name, node->ConstDecl.expression->FnCall.args[0]->String.string);
                    }
                } else {
                    generate_code_in_node(node->ConstDecl.expression);
                    pops(node->ConstDecl.const_name);
                }
            }
            break;

        case AST_CONST_DECL:
            if (!(is_it_global(node->VarDecl.var_name) || is_it_local(node->VarDecl.var_name))) {
                def_var(node->VarDecl.var_name, "lf");
                add_to_local(node->VarDecl.var_name);
            }

            if(strcmp(node->VarDecl.expression->FnCall.fn_name, "ifj.string") == 0){

                if (is_it_global(node->VarDecl.var_name)) {
                    printf("MOVE GF@%s string@%s\n",node->VarDecl.var_name, node->VarDecl.expression->FnCall.args[0]->String.string);
                } else {
                    printf("MOVE LF@%s string@%s\n",node->VarDecl.var_name, node->VarDecl.expression->FnCall.args[0]->String.string);
                }
                break;
            }
            if (node->ConstDecl.expression) {
                generate_code_in_node(node->ConstDecl.expression);
                pops(node->ConstDecl.const_name);
            }
            break;

        case AST_BLOCK:
            for (int i = 0; i < node->Block.node_count; ++i) {
                ASTNode* block_node = node->Block.nodes[i];

                if (block_node->type == AST_CONST_DECL || block_node->type == AST_VAR_DECL) {
                    if (block_node->VarDecl.expression &&
                        block_node->VarDecl.expression->type == AST_FN_CALL &&
                        strcmp(block_node->VarDecl.expression->FnCall.fn_name, "ifj.concat") == 0) {

                        const char* result = block_node->type == AST_CONST_DECL
                                             ? block_node->ConstDecl.const_name
                                             : block_node->VarDecl.var_name;

                        const char* arg1 = block_node->VarDecl.expression->FnCall.args[0]->Identifier.identifier;
                        const char* arg2 = block_node->VarDecl.expression->FnCall.args[1]->Identifier.identifier;

                        if (!(is_it_global(result) || is_it_local(result))) {
                            def_var(result, "lf");
                            add_to_local(result);
                        }

                        concat(result, arg1, arg2);

                        continue;
                    }
                }

                generate_code_in_node(block_node);
            }
            break;

        case AST_FN_CALL:
            //TODO: upraviť že "ifj.write" -> "WRITE" atď.
            // Ak funkcia je WRITE, CONCAT alebo READ, upravíme názov priamo

            const char* fn_name = node->FnCall.fn_name;
            if (strcmp(fn_name, "ifj.write") == 0) {
                generate_code_in_node(node->FnCall.args[0]);
                write(node->FnCall.args[0]->Identifier.identifier);
                return;
            } else if(strcmp(fn_name, "ifj.string") == 0){
                pushs(node->FnCall.args[0]->Identifier.identifier);
                //move(node->FnCall.args[0]->Identifier.identifier, "string@");
                return;
            }
            else
            {
                for (int i = node->FnCall.arg_count; i > 0; --i) {      // Generovanie kódu argumentov
                    pushs(node->FnCall.args[i-1]->Identifier.identifier);
                    generate_code_in_node(node->FnCall.args[i]);
                }
                call(fn_name);  // Volanie funkcie
            }
            break;

        case AST_ARG:
            generate_code_in_node(node->Argument.expression);
            break;

        case AST_RETURN:
            if (node->Return.expression && node->Return.expression->type == AST_IDENTIFIER) {
                const char* return_var = node->Return.expression->Identifier.identifier;
                if (return_var != NULL) {
                    pushs(return_var);
                } else {
                    printf("\033[31mERROR: Return expression is NULL\033[0m\n");
                }
            } else {
                printf("\033[31mERROR: Invalid return expression\033[0m\n");
            }
            break;

        case AST_BIN_OP:
            generate_code_in_node(node->BinaryOperator.left);
            generate_code_in_node(node->BinaryOperator.right);
            switch (node->BinaryOperator.operator) {
                case AST_PLUS: add_s("result"); break;
                case AST_MINUS: sub_s("result"); break;
                case AST_MUL: mul_s("result"); break;
                case AST_DIV: div_s("result"); break;
                default: print_with_indent("UNKNOWN_OPERATOR"); break;
            }
            break;

        case AST_INT:
            //printf("PUSHS int@%d\n", node->Integer.number);
            break;

        case AST_FLOAT:
            //printf("PUSHS float@%f\n", node->Float.number);
            break;

        case AST_STRING:
            //printf("PUSHS string@%s\n", node->String.string);
            break;

        case AST_IDENTIFIER:
            //printf("PUSHS LF@%s\n", node->Identifier.identifier);
            break;

        default:
            print_with_indent("UNKNOWN_NODE_TYPE");
            break;
    }
}


int generate_code(ASTNode* root){
    if (root == NULL) return 51; // ast root == NULL

    printf(".IFJcode24\n");
    printf("JUMP main\n");
    print_new_line();

    generate_code_in_node(root);

    free_var_arrays();

    return 0;
}

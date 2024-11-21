/**
 * @file parser.c
 * @brief Implementation of a generator.
 * @authors Martin Kandera (xkande00)
*/

#include "../inc/generator.h"
#include "../inc/generator_instructions.h"

#define MAX_VAR_NAME_LENGTH 128             // Maximálna dĺžka názvu premennej v GF
#define MAX_GF_VAR_COUNT 1024               // Maximálny počet globálnych premenných
#define MAX_LF_VAR_COUNT 1024               // Maximálny počet lokálnych premenných
char* gf_vars[MAX_GF_VAR_COUNT] = {NULL};   // Inicializácia všetkých prvkov na NULL
char* lf_vars[MAX_LF_VAR_COUNT] = {NULL};   // Inicializácia všetkých prvkov na NULL

static int if_counter = 1420; // Počiatočné číslovanie pre unikátne labely
static int while_counter = 1420; // Počiatočné číslovanie pre unikátne labely
static int tmp_counter = 0; // Počiatočné číslovanie pre unikátne premenné
static bool read_used = false; // Bol použitý read?


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
                    print_new_line();
                    if(read_used == true) {
                        label("read_not_valid");
                        printf("WRITE string@Nebyla\\032zadana\\032platna\\032hodnota\\010\n");
                        printf("EXIT int@4\n"); // ?? neviem aky exit code pre nil dať
                    }
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
                if(node->VarDecl.expression->type == AST_INT){
                    printf("MOVE %s%s int@%d\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name, node->VarDecl.expression->Integer.number);
                    break;
                }
                else if(node->VarDecl.expression->type == AST_FLOAT){
                    printf("MOVE %s%s float@%a\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name, node->VarDecl.expression->Float.number);
                    break;
                }
                else if(node->VarDecl.expression->type == AST_STRING){
                    printf("MOVE %s%s string@%s\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name, node->VarDecl.expression->String.string);
                    break;
                }
                /*else if(node->VarDecl.expression->type == AST_FN_CALL){
                    generate_code_in_node(node->VarDecl.expression);
                    pops(node->VarDecl.var_name);
                }*/
                if (strcmp(node->ConstDecl.expression->FnCall.fn_name, "ifj.string") == 0) {
                    if (is_it_global(node->ConstDecl.const_name)) {
                        printf("MOVE GF@%s string@%s\n", node->ConstDecl.const_name, node->ConstDecl.expression->FnCall.args[0]->Argument.expression->String.string);
                    } else {
                        printf("MOVE LF@%s string@%s\n", node->ConstDecl.const_name, node->ConstDecl.expression->FnCall.args[0]->Argument.expression->String.string);
                    }
                } else if(strcmp(node->ConstDecl.expression->FnCall.fn_name, "ifj.readstr") == 0){
                    read_used = true;
                    printf("READ %s%s string\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name);
                    printf("PUSHS %s%s\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name);
                    printf("PUSHS nil@nil\n");
                    printf("JUMPIFEQS read_not_valid\n");
                } else if(strcmp(node->ConstDecl.expression->FnCall.fn_name, "ifj.readi32") == 0){
                    read_used = true;
                    printf("READ %s%s int\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name);
                    printf("PUSHS %s%s\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name);
                    printf("PUSHS nil@nil\n");
                    printf("JUMPIFEQS read_not_valid\n");
                } else if(strcmp(node->ConstDecl.expression->FnCall.fn_name, "ifj.readf64") == 0){
                    read_used = true;
                    printf("READ %s%s float\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name);
                    printf("PUSHS %s%s\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name);
                    printf("PUSHS nil@nil\n");
                    printf("JUMPIFEQS read_not_valid\n");
                }
                else{
                    generate_code_in_node(node->ConstDecl.expression);
                    pops(node->ConstDecl.const_name);
                }
            }
            break;

        case AST_CONST_DECL:
            if (!(is_it_global(node->VarDecl.var_name) || is_it_local(node->VarDecl.var_name))){
                def_var(node->VarDecl.var_name, "lf");
                add_to_local(node->VarDecl.var_name);
            }
            if (node->ConstDecl.expression) {

                if(node->VarDecl.expression->type == AST_INT){
                    printf("MOVE %s%s int@%d\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name, node->VarDecl.expression->Integer.number);
                    break;
                }
                else if(node->VarDecl.expression->type == AST_FLOAT){
                    printf("MOVE %s%s float@%a\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name, node->VarDecl.expression->Float.number);
                    break;
                }
                else if(node->VarDecl.expression->type == AST_STRING){
                    printf("MOVE %s%s string@%s\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name, node->VarDecl.expression->String.string);
                    break;
                }

                if (strcmp(node->ConstDecl.expression->FnCall.fn_name, "ifj.string") == 0) {
                    if (is_it_global(node->ConstDecl.const_name)) {
                        printf("MOVE GF@%s string@%s\n", node->ConstDecl.const_name, node->ConstDecl.expression->FnCall.args[0]->Argument.expression->String.string);
                    } else {
                        printf("MOVE LF@%s string@%s\n", node->ConstDecl.const_name, node->ConstDecl.expression->FnCall.args[0]->Argument.expression->String.string);
                    }
                } else if(strcmp(node->ConstDecl.expression->FnCall.fn_name, "ifj.readstr") == 0){
                    read_used = true;
                    printf("READ %s%s string\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name);
                    printf("PUSHS %s%s\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name);
                    printf("PUSHS nil@nil\n");
                    printf("JUMPIFEQS read_not_valid\n");
                } else if(strcmp(node->ConstDecl.expression->FnCall.fn_name, "ifj.readi32") == 0){
                    read_used = true;
                    printf("READ %s%s int\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name);
                    printf("PUSHS %s%s\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name);
                    printf("PUSHS nil@nil\n");
                    printf("JUMPIFEQS read_not_valid\n");
                } else if(strcmp(node->ConstDecl.expression->FnCall.fn_name, "ifj.readf64") == 0){
                    read_used = true;
                    printf("READ %s%s float\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name);
                    printf("PUSHS %s%s\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name);
                    printf("PUSHS nil@nil\n");
                    printf("JUMPIFEQS read_not_valid\n");
                }
                else{
                    generate_code_in_node(node->ConstDecl.expression);
                    pops(node->ConstDecl.const_name);
                }
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

                        const char* arg1 = block_node->VarDecl.expression->FnCall.args[0]->Argument.expression->Identifier.identifier;
                        const char* arg2 = block_node->VarDecl.expression->FnCall.args[1]->Argument.expression->Identifier.identifier;

                        if (!(is_it_global(result) || is_it_local(result))) {
                            def_var(result, "lf");
                            add_to_local(result);
                        }
                        if(block_node->VarDecl.expression->FnCall.args[0]->type == AST_ARG){
                            //printf("string");
                            //debug function
                        }
                        concat(result, arg1, arg2);
                        generate_code_in_node(block_node);
                        break;
                    }
                }
                if (block_node->type == AST_CONST_DECL || block_node->type == AST_VAR_DECL) {
                    if (block_node->VarDecl.expression &&
                        block_node->VarDecl.expression->type == AST_FN_CALL &&
                        strcmp(block_node->VarDecl.expression->FnCall.fn_name, "ifj.strlen") == 0) {

                        const char* result = block_node->type == AST_CONST_DECL
                                             ? block_node->ConstDecl.const_name
                                             : block_node->VarDecl.var_name;

                        const char* arg1 = block_node->VarDecl.expression->FnCall.args[0]->Argument.expression->Identifier.identifier;

                        if (!(is_it_global(result) || is_it_local(result))) {
                            def_var(result, "lf");
                            add_to_local(result);
                        }
                        if(block_node->VarDecl.expression->FnCall.args[0]->type == AST_ARG){
                            //printf("string");
                            //debug function
                        }
                        printf("STRLEN %s%s %s%s\n", frame_prefix(result), result, frame_prefix(arg1), arg1);
                        generate_code_in_node(block_node);
                        break;
                    }
                }
                generate_code_in_node(block_node);
            }
            break;

        case AST_FN_CALL:
            const char* fn_name = node->FnCall.fn_name;

            // Vestavěná funkce i2f
            if (strcmp(fn_name, "ifj.i2f") == 0) {
                generate_code_in_node(node->FnCall.args[0]->Argument.expression);
                printf("INT2FLOATS\n");
                break;
            }
                // Vestavěná funkce f2i
            if (strcmp(fn_name, "ifj.f2i") == 0) {
                generate_code_in_node(node->FnCall.args[0]->Argument.expression);
                printf("FLOAT2INTS\n");
                break;
            }
                // Vestavěná funkce length

            if (strcmp(fn_name, "ifj.length") == 0) {
                generate_code_in_node(node->FnCall.args[0]->Argument.expression);
                //printf("STRLEN %s
                //pr
            }

            if ((strcmp(fn_name, "ifj.write") == 0)||(strcmp(fn_name, "ifj.writef64") == 0) ) {
                //generate_code_in_node(node->FnCall.args[0]);
                //write(node->FnCall.args[0]->Argument.expression->Identifier.identifier);
                ASTNode *expression = node->FnCall.args[0]->Argument.expression;
                switch (expression->type) {
                    case AST_STRING: {
                        char *escaped_string = escape_string(expression->String.string);
                        printf("WRITE string@%s\n", escaped_string);
                        free(escaped_string);
                        break;
                    }
                    case AST_INT:
                        printf("WRITE int@%d\n", expression->Integer.number);
                        break;
                    case AST_FLOAT:
                        printf("WRITE float@%f\n", expression->Float.number);
                        break;
                    case AST_IDENTIFIER:
                        printf("WRITE %s%s\n", frame_prefix(expression->Identifier.identifier),
                               expression->Identifier.identifier);
                        break;
                    default:
                        fprintf(stderr, "ERROR: Unsupported argument type for ifj.write\n");
                        exit(99);
                }
            }
            else
            {
                for (int i = node->FnCall.arg_count; i > 0; --i) {      // Generovanie kódu argumentov
                    pushs(node->FnCall.args[i-1]->Argument.expression->Identifier.identifier);
                    generate_code_in_node(node->FnCall.args[i]);
                }
                call(fn_name);  // Volanie funkcie
            }
            break;
        case AST_ASSIGNMENT:
            generate_code_in_node(node->Assignment.expression);
            if (is_it_global(node->Assignment.identifier)) {
                pops(node->Assignment.identifier);
            } else {
                pops(node->Assignment.identifier);
            }
            break;
        case AST_IF_ELSE:
            int current_if = if_counter++; // Unikátne číslo pre aktuálny if-else blok

            if(node->IfElse.element_bind != NULL){
                printf("DEFVAR LF@%s\n", node->IfElse.element_bind);
            }

            generate_code_in_node(node->IfElse.expression);
            printf("PUSHS bool@false\n");
            printf("JUMPIFEQ else_block_%d\n", current_if);
            if (node->IfElse.element_bind != NULL) printf("MOVE LF@%s %s%s\n", node->IfElse.element_bind, frame_prefix(node->IfElse.expression->Identifier.identifier),node->IfElse.expression->Identifier.identifier);

            // Telo IF
            generate_code_in_node(node->IfElse.if_block);
            printf("JUMP end_block_%d\n", current_if);

            // ELSE blok (ak existuje)
            printf("LABEL else_block_%d\n", current_if);
            if (node->IfElse.element_bind != NULL) printf("MOVE LF@%s %s%s\n", node->IfElse.element_bind, frame_prefix(node->IfElse.expression->Identifier.identifier),node->IfElse.expression->Identifier.identifier);
            if (node->IfElse.else_block) {
                generate_code_in_node(node->IfElse.else_block);
            }

            // Koniec celého bloku
            printf("LABEL end_block_%d\n", current_if);
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
                    printf("ERROR: Return expression is NULL\n");
                }
            } else {
                printf("ERROR: Invalid return expression\n");
            }
            break;

        case AST_BIN_OP:
            generate_code_in_node(node->BinaryOperator.left);
            generate_code_in_node(node->BinaryOperator.right);

            switch (node->BinaryOperator.operator) {
                case AST_PLUS: printf("ADDS\n"); break;
                case AST_MINUS: printf("SUBS\n"); break;
                case AST_MUL: printf("MULS\n"); break;
                case AST_DIV: printf("DIVS\n"); break;
                case AST_GREATER: printf("GTS\n"); break;
                case AST_GREATER_EQU:
                    printf("LTS\n");
                    printf("NOTS\n");
                    break;
                case AST_LESS: printf("LTS\n"); break;
                case AST_LESS_EQU:
                    printf("GTS\n");
                    printf("NOTS\n");
                    break;
                case AST_EQU: printf("EQS\n"); break;
                case AST_NOT_EQU:
                    printf("EQS\n");
                    printf("NOTS\n");
                    break;
                default:
                    fprintf(stderr, "ERROR: Unknown binary operator type\n");
                    exit(99);
            }
            break;

        case AST_WHILE: {
            int current_while = while_counter++; // Unikátne číslo pre while slučku
            if(node->WhileCycle.element_bind != NULL){
                printf("DEFVAR LF@%s\n", node->WhileCycle.element_bind);
            }
            // Vytváranie labelov
            printf("LABEL while_start_%d\n", current_while); // Začiatok while cyklu

            // Generovanie kódu pre podmienku
            generate_code_in_node(node->WhileCycle.expression);
            printf("PUSHS bool@true\n"); // Očakávame, že podmienka vráti bool
            printf("JUMPIFNEQ while_end_%d\n", current_while); // Ak nie je splnená, skáčeme na koniec
            if (node->WhileCycle.element_bind != NULL) printf("MOVE LF@%s %s%s\n", node->WhileCycle.element_bind, frame_prefix(node->WhileCycle.expression->Identifier.identifier),node->WhileCycle.expression->Identifier.identifier);

            // Generovanie tela slučky
            generate_code_in_node(node->WhileCycle.block);
            printf("JUMP while_start_%d\n", current_while); // Návrat na začiatok

            // Label pre koniec while slučky
            printf("LABEL while_end_%d\n", current_while);
            break;
        }

        case AST_INT:
            printf("PUSHS int@%d\n", node->Integer.number);
            break;

        case AST_FLOAT:
            printf("PUSHS float@%f\n", node->Float.number);
            break;

        case AST_STRING: {
            char* escaped_string = escape_string(node->String.string);
            printf("PUSHS string@%s\n", escaped_string);
            free(escaped_string);
            break;
        }

        case AST_IDENTIFIER:
            printf("PUSHS %s%s\n", frame_prefix(node->Identifier.identifier), node->Identifier.identifier);
            break;

        default:
            print_with_indent("UNKNOWN_NODE_TYPE");
            break;
    }
}

int generate_code(ASTNode* root){
    if (root == NULL) return 51; // ast root == NULL

    printf(".IFJcode24\n");
    printf("JUMP main\n");    print_new_line();

    generate_code_in_node(root);

    free_var_arrays();
    return 0;
}
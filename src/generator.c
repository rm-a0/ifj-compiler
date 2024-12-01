/**
 * @file parser.c
 * @brief Implementation of a generator.
 * @authors Martin Kandera (xkande00)
*/

#include "generator.h"
#include "generator_instructions.h"

#define MAX_VAR_NAME_LENGTH 128             // Maximálna dĺžka názvu premennej v GF
#define MAX_GF_VAR_COUNT 1024               // Maximálny počet globálnych premenných
#define MAX_LF_VAR_COUNT 1024               // Maximálny počet lokálnych premenných
char* lf_vars[MAX_LF_VAR_COUNT] = {NULL};   // Inicializácia všetkých prvkov na NULL

static int if_counter = 1420;       // Počiatočné číslovanie pre unikátne labely
static int while_counter = 1420;    // Počiatočné číslovanie pre unikátne labely
int tmp_counter = 128;       // Počiatočné číslovanie pre unikátne premenné

int get_tmp_counter() {
    return tmp_counter;
}

void increment_tmp_counter() {
    tmp_counter++;
}

WhileStack while_stack = { .top = NULL };    // Globálny zásobník pre while cykly


// Vráti počet while-cyklov v zásobníku
int while_stack_size() {
    int size = 0;
    WhileStackNode *current = while_stack.top;
    while (current != NULL) {
        size++;
        current = current->next;
    }
    return size;
}

// Získa hodnotu čísla while na konkrétnej pozícii (od 0 po vrchol)
int get_while_at_index(int index) {
    int i = 0;
    WhileStackNode *current = while_stack.top;
    while (current != NULL) {
        if (i == index) {
            return current->while_number;
        }
        i++;
        current = current->next;
    }
    fprintf(stderr, "Chyba: Index mimo rozsahu zásobníka while.\n");
    exit(99);
}

// Pridanie čísla while-u do globálneho zásobníka
void add_while_stack(int while_number) {
    WhileStackNode *new_node = (WhileStackNode *)malloc(sizeof(WhileStackNode));
    if (!new_node) {
        fprintf(stderr, "Chyba: Nepodarilo sa alokovať pamäť pre uzol zásobníka.\n");
        exit(99); // Interná chyba
    }
    new_node->while_number = while_number;
    new_node->next = while_stack.top;
    while_stack.top = new_node;
}

// Odstránenie čísla while-u z globálneho zásobníka
void remove_while_stack() {
    if (while_stack.top == NULL) {
        fprintf(stderr, "Chyba: Zásobník while cyklov je prázdny.\n");
        exit(99); // Interná chyba
    }
    WhileStackNode *temp = while_stack.top;
    while_stack.top = while_stack.top->next;
    free(temp);
}

// Získanie aktuálneho čísla while-u z globálneho zásobníka
int actual_while() {
    if (while_stack.top == NULL) {
        fprintf(stderr, "Chyba: Zásobník while cyklov je prázdny.\n");
        exit(99); // Interná chyba
    }
    return while_stack.top->while_number;
}

// Uvoľnenie celého zásobníka
void free_while_stack() {
    WhileStackNode *current = while_stack.top;
    while (current) {
        WhileStackNode *temp = current;
        current = current->next;
        free(temp);
    }
    while_stack.top = NULL;
}


void free_var_arrays() {
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
                    gen_create_frame();
                    gen_push_frame();
                    generate_code_in_node(node->Program.declarations[i]);  // Generovanie kódu `main`
                    gen_pop_frame();    // ?? netusim ci tu treba naspat pop-ovat ramec
                    printf("EXIT int@0\n");  // Ukončí program ?? - je to nutné?
                    print_new_line();
                }
                else {
                    label(node->Program.declarations[i]->FnDecl.fn_name);
                    gen_create_frame();
                    gen_push_frame();
                    generate_code_in_node(node->Program.declarations[i]);  // Ostatné funkcie
                    gen_pop_frame();
                    return_f();
                    print_new_line();
                }
            }
            break;

        case AST_FN_DECL:
            for (int i = 0; i < node->FnDecl.param_count; ++i) {
                def_var(node->FnDecl.params[i]->Param.identifier);
                add_to_local(node->FnDecl.params[i]->Param.identifier);
                pops(node->FnDecl.params[i]->Param.identifier);
            }
            generate_code_in_node(node->FnDecl.block);
            break;

        case AST_PARAM:
            def_var(node->Param.identifier);
            add_to_local(node->Param.identifier);
            // ?? neviem ci toto tu nie je zbytocne
            break;

        case AST_VAR_DECL:
        case AST_CONST_DECL: {
            if (!(is_it_local(node->VarDecl.var_name))) {
                def_var(node->VarDecl.var_name);
                add_to_local(node->VarDecl.var_name);
            }


            if (node->ConstDecl.expression) {

                if (node->VarDecl.expression->type == AST_INT) {
                    printf("MOVE %s%s int@%d\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name,
                           node->VarDecl.expression->Integer.number);
                    break;
                } else if (node->VarDecl.expression->type == AST_FLOAT) {
                    printf("MOVE %s%s float@%a\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name,
                           node->VarDecl.expression->Float.number);
                    break;
                } else if (node->VarDecl.expression->type == AST_STRING) {
                    char *escape_string_string = escape_string(node->VarDecl.expression->String.string);
                    printf("MOVE %s%s string@%s\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name,
                           escape_string_string);
                    free(escape_string_string);
                    break;
                } else if (node->VarDecl.expression->type == AST_BIN_OP) {
                    printf("PUSHS int@1\n");
                    generate_code_in_node(node->VarDecl.expression);
                    pops(node->VarDecl.var_name);

                    break;
                }

                if (strcmp(node->ConstDecl.expression->FnCall.fn_name, "ifj.string") == 0) {
                        printf("MOVE LF@%s string@%s\n", node->ConstDecl.const_name,
                               node->ConstDecl.expression->FnCall.args[0]->Argument.expression->String.string);

                } else if (strcmp(node->ConstDecl.expression->FnCall.fn_name, "ifj.readstr") == 0) {
                    printf("READ %s%s string\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name);

                } else if (strcmp(node->ConstDecl.expression->FnCall.fn_name, "ifj.readi32") == 0) {
                    printf("READ %s%s int\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name);

                } else if (strcmp(node->ConstDecl.expression->FnCall.fn_name, "ifj.readf64") == 0) {
                    printf("READ %s%s float\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name);

                } else {
                    generate_code_in_node(node->ConstDecl.expression);
                    pops(node->ConstDecl.const_name);
                }
            }
            break;
        }
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

                        if (!(is_it_local(result))) {
                            def_var(result);
                            add_to_local(result);
                        }
                        //if(block_node->VarDecl.expression->FnCall.args[0]->type == AST_ARG){
                            //printf("string");
                            //debug function
                        //}
                        concat(result, arg1, arg2);
                        continue;

                    }
                }
                if (block_node->type == AST_CONST_DECL || block_node->type == AST_VAR_DECL) {
                    if (block_node->VarDecl.expression &&
                        block_node->VarDecl.expression->type == AST_FN_CALL &&
                        strcmp(block_node->VarDecl.expression->FnCall.fn_name, "ifj.length") == 0) {

                        const char* result = block_node->type == AST_CONST_DECL
                                             ? block_node->ConstDecl.const_name
                                             : block_node->VarDecl.var_name;

                        const char* arg1 = block_node->VarDecl.expression->FnCall.args[0]->Argument.expression->Identifier.identifier;

                        if (!(is_it_local(result))) {
                            def_var(result);
                            add_to_local(result);
                        }
                        if(block_node->VarDecl.expression->FnCall.args[0]->type == AST_ARG){
                            //printf("string");
                            //debug function
                        }
                        printf("STRLEN %s%s %s%s\n", frame_prefix(result), result, frame_prefix(arg1), arg1);
                        generate_code_in_node(block_node);
                        continue;
                    }
                }
                generate_code_in_node(block_node);
            }
            break;
        case AST_FN_CALL :{
            const char *fn_name = node->FnCall.fn_name;

            /*if (strcmp(node->FnCall.fn_name, "ifj.string") == 0) {
                printf("MOVE LF@%s string@%s\n", node->ConstDecl.const_name, node->ConstDecl.expression->FnCall.args[0]->Argument.expression->String.string);
                break;

            } else if(strcmp(node->FnCall.fn_name, "ifj.readstr") == 0){
                printf("READ %s%s string\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name);
                break;

            } else if(strcmp(node->FnCall.fn_name, "ifj.readi32") == 0){
                printf("READ %s%s int\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name);
                break;

            } else if(strcmp(node->FnCall.fn_name, "ifj.readf64") == 0){
                printf("READ %s%s float\n", frame_prefix(node->VarDecl.var_name), node->VarDecl.var_name);
                break;

            }*/

            if (strcmp(fn_name, "ifj.length") == 0) {
                // TO-DO: nepotrebný push ale neviem sa ho ľahko zbaviť
                generate_code_in_node(node->FnCall.args[0]->Argument.expression);
                char temp_var[32];
                snprintf(temp_var, sizeof(temp_var), "LF@tmp_length_%d", tmp_counter++);
                def_var(temp_var);
                printf("STRLEN %s %s%s\n", temp_var,
                       frame_prefix(node->FnCall.args[0]->Argument.expression->Identifier.identifier),
                       node->FnCall.args[0]->Argument.expression->Identifier.identifier);
                printf("PUSHS %s\n", temp_var);
                break;
            }
            if (strcmp(fn_name, "ifj.substring") == 0) {
                // Generovanie argumentov: s, i, j
                generate_code_in_node(node->FnCall.args[0]->Argument.expression); // s
                generate_code_in_node(node->FnCall.args[1]->Argument.expression); // i
                generate_code_in_node(node->FnCall.args[2]->Argument.expression); // j

                // Pridanie pomocných premenných
                char temp_s[32], temp_i[32], temp_j[32], temp_result[32], temp_char[32];
                snprintf(temp_s, sizeof(temp_s), "LF@tmp_s_%d", tmp_counter);
                snprintf(temp_i, sizeof(temp_i), "LF@tmp_i_%d", tmp_counter);
                snprintf(temp_j, sizeof(temp_j), "LF@tmp_j_%d", tmp_counter);
                snprintf(temp_char, sizeof(temp_char), "LF@tmp_char_%d", tmp_counter);
                snprintf(temp_result, sizeof(temp_result), "LF@tmp_result_%d", tmp_counter);

                def_var(temp_s);
                def_var(temp_i);
                def_var(temp_j);
                def_var(temp_char);
                def_var(temp_result);


                // Uloženie argumentov do pomocných premenných
                printf("POPS %s\n", temp_j);
                printf("POPS %s\n", temp_i);
                printf("POPS %s\n", temp_s);

                // Inicializácia výsledného reťazca
                printf("MOVE %s string@\n", temp_result);

                // Kontroly pre validáciu
                // neviem či to máme kontrolovať ??
               /* printf("PUSHS %s\n", temp_i);
                printf("PUSHS int@0\n");
                printf("LTS\n");
                printf("JUMPIFNEQ invalid_substring\n");

                printf("PUSHS %s\n", temp_j);
                printf("PUSHS int@0\n");
                printf("LTS\n");
                printf("JUMPIFNEQ invalid_substring\n");

                printf("PUSHS %s\n", temp_i);
                printf("PUSHS %s\n", temp_j);
                printf("GTS\n");
                printf("JUMPIFNEQ invalid_substring\n");*/

                /*printf("STRLEN %s %s\n", temp_char, temp_s); // Dĺžka reťazca
                printf("PUSHS %s\n", temp_i);
                printf("PUSHS %s\n", temp_char);
                printf("GTS\n");
                printf("JUMPIFNEQ invalid_substring\n");

                printf("PUSHS %s\n", temp_j);
                printf("PUSHS %s\n", temp_char);
                printf("GTS\n");
                printf("JUMPIFNEQ invalid_substring\n");*/

                // Ak je validné, vytvor substring
                printf("LABEL substring_loop_start_%i\n", while_counter);
                printf("PUSHS %s\n", temp_i);
                printf("PUSHS %s\n", temp_j);
                printf("LTS\n");
                printf("PUSHS bool@false\n");
                printf("JUMPIFEQS substring_loop_end_%i\n", while_counter);

                printf("GETCHAR %s %s %s\n", temp_char, temp_s, temp_i); // Získanie znaku na indexe `i`
                printf("CONCAT %s %s %s\n", temp_result, temp_result, temp_char); // Pridanie znaku do výsledku
                printf("ADD %s %s int@1\n", temp_i, temp_i); // Zvýšenie `i`
                printf("JUMP substring_loop_start_%i\n", while_counter);

                printf("LABEL substring_loop_end_%i\n", while_counter);
                printf("PUSHS %s\n", temp_result); // Push výsledný substring na zásobník
                /*printf("JUMP end_substring\n");

                // Neplatný podreťazec
                printf("LABEL invalid_substring\n");
                printf("PUSHS nil@nil\n");

                printf("LABEL end_substring\n");*/

                while_counter++;
                break;
            }
            if (strcmp(fn_name, "ifj.strcmp") == 0) {
                // Generate code for both string arguments
                generate_code_in_node(node->FnCall.args[0]->Argument.expression);
                generate_code_in_node(node->FnCall.args[1]->Argument.expression);

                // Create a temporary variable for comparison result
                char temp_var[32];
                snprintf(temp_var, sizeof(temp_var), "LF@tmp_cmp_%d", tmp_counter);
                def_var(temp_var);
                char temp_label_equal[32], temp_label_greater[32], temp_label_end[32];
                snprintf(temp_label_equal, sizeof(temp_label_equal), "strcmp_equal_%d", tmp_counter);
                snprintf(temp_label_greater, sizeof(temp_label_greater), "strcmp_greater_%d", tmp_counter);
                snprintf(temp_label_end, sizeof(temp_label_end), "strcmp_end_%d", tmp_counter);
                tmp_counter++;


                // Compare the strings
                printf("JUMPIFEQS %s\n", temp_label_equal);

                // If s1 < s2
                printf("LT %s %s%s %s%s\n", temp_var,
                       frame_prefix(node->FnCall.args[0]->Argument.expression->Identifier.identifier),
                       node->FnCall.args[0]->Argument.expression->Identifier.identifier,
                       frame_prefix(node->FnCall.args[1]->Argument.expression->Identifier.identifier),
                       node->FnCall.args[1]->Argument.expression->Identifier.identifier);
                printf("PUSHS %s\n", temp_var);
                printf("PUSHS bool@true\n");
                printf("JUMPIFNEQS %s\n", temp_label_greater);

                // Set temp_var to -1 (s1 < s2)
                printf("MOVE %s int@-1\n", temp_var);
                printf("JUMP %s\n", temp_label_end);

                // If s1 == s2
                printf("LABEL %s\n", temp_label_equal);
                printf("MOVE %s int@0\n", temp_var);
                printf("JUMP %s\n", temp_label_end);

                // If s1 > s2
                printf("LABEL %s\n", temp_label_greater);
                printf("MOVE %s int@1\n", temp_var);

                // End label
                printf("LABEL %s\n", temp_label_end);

                // Push the result
                printf("PUSHS %s\n", temp_var);
                break;
            }
            if (strcmp(fn_name, "ifj.ord") == 0) {
                // Generate code for string and index
                generate_code_in_node(node->FnCall.args[0]->Argument.expression);
                generate_code_in_node(node->FnCall.args[1]->Argument.expression);

                // Create a temporary variable for the result
                char temp_var[32];
                snprintf(temp_var, sizeof(temp_var), "LF@tmp_ord_%d", tmp_counter++);
                def_var(temp_var);

                // Get the character and convert to ASCII
                printf("STRI2INT %s %s%s %s%s\n", temp_var,
                       frame_prefix(node->FnCall.args[0]->Argument.expression->Identifier.identifier),
                       node->FnCall.args[0]->Argument.expression->Identifier.identifier,
                       frame_prefix(node->FnCall.args[1]->Argument.expression->Identifier.identifier),
                       node->FnCall.args[1]->Argument.expression->Identifier.identifier);

                // Push the result
                printf("PUSHS %s\n", temp_var);
                break;
            }
            if (strcmp(fn_name, "ifj.chr") == 0) {
                // Generate code for the ASCII value
                generate_code_in_node(node->FnCall.args[0]->Argument.expression);

                // Create a temporary variable for the result
                char temp_var[32];
                snprintf(temp_var, sizeof(temp_var), "LF@tmp_chr_%d", tmp_counter++);
                def_var(temp_var);

                // Convert ASCII to character
                printf("INT2CHAR %s %s%s\n", temp_var,
                       frame_prefix(node->FnCall.args[0]->Argument.expression->Identifier.identifier),
                       node->FnCall.args[0]->Argument.expression->Identifier.identifier);

                // Push the result
                printf("PUSHS %s\n", temp_var);
                break;
            }

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
            }

            if ((strcmp(fn_name, "ifj.write") == 0) || (strcmp(fn_name, "ifj.writef64") == 0)) {
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
                    case AST_BIN_OP:
                    case AST_FN_CALL: {
                        generate_code_in_node(expression);
                        char temp_var[32];
                        snprintf(temp_var, sizeof(temp_var), "tm_write%d", tmp_counter);
                        def_var(temp_var);//printf("DEFVAR LF@tm_write%i\n", tmp_counter);
                        printf("POPS LF@tm_write%i\n", tmp_counter);
                        printf("WRITE LF@tm_write%i\n", tmp_counter);
                        tmp_counter++;
                        break;
                    }
                    default:
                        fprintf(stderr, "ERROR: Unsupported argument type for ifj.write\n");
                        exit(99);
                }
            } else {
                for (int i = node->FnCall.arg_count; i > 0; --i) {      // Generovanie kódu argumentov
                    pushs(node->FnCall.args[i - 1]->Argument.expression->Identifier.identifier);
                    generate_code_in_node(node->FnCall.args[i]);
                }
                call(fn_name);  // Volanie funkcie
            }
            break;
        }
        case AST_ASSIGNMENT:

            if (node->Assignment.expression) {
                if(node->Assignment.expression->type == AST_INT){
                    printf("MOVE %s%s int@%d\n", frame_prefix(node->Assignment.identifier), node->Assignment.identifier, node->Assignment.expression->Integer.number);
                    break;
                }
                else if(node->Assignment.expression->type == AST_FLOAT){
                    printf("MOVE %s%s float@%a\n", frame_prefix(node->Assignment.identifier), node->Assignment.identifier, node->Assignment.expression->Float.number);
                    break;
                }
                else if(node->Assignment.expression->type == AST_STRING){
                    char* escape_string_string = escape_string(node->Assignment.expression->String.string);
                    printf("MOVE %s%s string@%s\n", frame_prefix(node->Assignment.identifier), node->Assignment.identifier,
                           escape_string_string);
                    free(escape_string_string);
                    break;
                } else if(node->Assignment.expression->type == AST_BIN_OP){
                    printf("PUSHS int@1\n");
                    generate_code_in_node(node->Assignment.expression);
                    pops(node->Assignment.identifier);

                    break;
                }
                if (node->Assignment.expression->type == AST_FN_CALL) {

                    if (strcmp(node->Assignment.expression->FnCall.fn_name, "ifj.string") == 0) {
                        printf("MOVE LF@%s string@%s\n", node->Assignment.identifier,
                               node->Assignment.expression->FnCall.args[0]->Argument.expression->String.string);
                    } else if (strcmp(node->Assignment.expression->FnCall.fn_name, "ifj.readstr") == 0) {
                        printf("READ %s%s string\n", frame_prefix(node->Assignment.identifier),
                               node->Assignment.identifier);

                    } else if (strcmp(node->Assignment.expression->FnCall.fn_name, "ifj.readi32") == 0) {
                        printf("READ %s%s int\n", frame_prefix(node->Assignment.identifier),
                               node->Assignment.identifier);

                    } else if (strcmp(node->Assignment.expression->FnCall.fn_name, "ifj.readf64") == 0) {
                        printf("READ %s%s float\n", frame_prefix(node->Assignment.identifier),
                               node->Assignment.identifier);

                    } else if (strcmp(node->Assignment.expression->FnCall.fn_name, "ifj.concat") == 0) {
                        const char *result = node->Assignment.identifier;
                        const char *arg1 = node->Assignment.expression->FnCall.args[0]->Argument.expression->Identifier.identifier;
                        const char *arg2 = node->Assignment.expression->FnCall.args[1]->Argument.expression->Identifier.identifier;
                        concat(result, arg1, arg2);
                        break;
                    }
                    // TODO: add other functions
                    else {
                        generate_code_in_node(node->Assignment.expression);
                        pops(node->Assignment.identifier);
                    }
                    break;
                }
                generate_code_in_node(node->Assignment.expression);
                pops(node->Assignment.identifier);
            }
            break;

        case AST_IF_ELSE: {
            int current_if = if_counter++; // Unikátne číslo pre aktuálny if-else blok

            // Definícia pre element_bind, ak existuje
            if (node->WhileCycle.element_bind != NULL) {
                if(!(is_it_local(node->WhileCycle.element_bind))){
                    def_var(node->WhileCycle.element_bind);    //printf("DEFVAR LF@%s\n", node->WhileCycle.element_bind);
                    add_to_local(node->WhileCycle.element_bind);
                }
            }

            // Generovanie výrazu v podmienke
            generate_code_in_node(node->IfElse.expression);

            // Rozhodovanie podľa typu výrazu

            //treba zabaliť do if-u že ak je tam element bind a či nie
            switch (node->IfElse.expression->type) {
                case AST_IDENTIFIER:
                case AST_INT:
                case AST_FLOAT:
                case AST_STRING: {
                    // Porovnanie s nil
                    printf("PUSHS nil@nil\n");
                    printf("JUMPIFEQS else_block_%d\n", current_if);
                    break;
                }
                default: {
                    // Porovnanie s bool@false
                    printf("PUSHS bool@false\n");
                    printf("JUMPIFEQS else_block_%d\n", current_if);
                    break;
                }
            }

            // Ak je element_bind definovaný, nastav jeho hodnotu
            if (node->IfElse.element_bind != NULL) {
                printf("MOVE LF@%s %s%s\n", node->IfElse.element_bind,
                       frame_prefix(node->IfElse.expression->Identifier.identifier),
                       node->IfElse.expression->Identifier.identifier);
            }

            // Generovanie tela IF
            generate_code_in_node(node->IfElse.if_block);
            printf("JUMP end_block_%d\n", current_if);

            // ELSE blok (ak existuje)
            printf("LABEL else_block_%d\n", current_if);
            if (node->IfElse.element_bind != NULL) {
                printf("MOVE LF@%s %s%s\n", node->IfElse.element_bind,
                       frame_prefix(node->IfElse.expression->Identifier.identifier),
                       node->IfElse.expression->Identifier.identifier);
            }
            if (node->IfElse.else_block) {
                generate_code_in_node(node->IfElse.else_block);
            }

            // Koniec celého bloku
            printf("LABEL end_block_%d\n", current_if);
            break;
        }

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
                generate_code_in_node(node->Return.expression);
            }
            break;

        case AST_BIN_OP:

            if(node->BinaryOperator.operator == AST_DIV){
                generate_code_in_node(node->BinaryOperator.left);
                generate_code_in_node(node->BinaryOperator.right);
                char temp_d_2[32], temp_t_2[32], temp_d_1[32], temp_t_1[32], temp_d_4[32], temp_d_3[32], temp_t_3[32];
                snprintf(temp_d_2, sizeof(temp_d_2), "tmp_div_2_%d", tmp_counter);
                snprintf(temp_t_2, sizeof(temp_t_2), "tmp_type_2_%d", tmp_counter);
                snprintf(temp_d_1, sizeof(temp_d_1), "tmp_div_1_%d", tmp_counter);
                snprintf(temp_t_1, sizeof(temp_t_1), "tmp_type_1_%d", tmp_counter);
                snprintf(temp_d_4, sizeof(temp_d_4), "tmp_div_4_%d", tmp_counter);
                snprintf(temp_d_3, sizeof(temp_d_3), "tmp_div_3_%d", tmp_counter);
                snprintf(temp_t_3, sizeof(temp_t_3), "tmp_type_3_%d", tmp_counter);


                def_var(temp_d_2);      //printf("DEFVAR LF@tmp_div_2_%i\n", tmp_counter);        //129
                def_var(temp_t_2);      //printf("DEFVAR LF@tmp_type_2_%i\n", tmp_counter);       //129
                printf("POPS LF@tmp_div_2_%i\n", tmp_counter);           //129
                printf("TYPE LF@tmp_type_2_%i LF@tmp_div_2_%i\n", tmp_counter, tmp_counter); //129
                def_var(temp_d_1);      //printf("DEFVAR LF@tmp_div_1_%i\n", tmp_counter);        //129
                def_var(temp_t_1);      //printf("DEFVAR LF@tmp_type_1_%i\n", tmp_counter);       //129
                printf("POPS LF@tmp_div_1_%i\n", tmp_counter);           //129
                printf("TYPE LF@tmp_type_1_%i LF@tmp_div_1_%i\n", tmp_counter, tmp_counter); //129
                printf("PUSHS LF@tmp_div_1_%i\n", tmp_counter);          //129
                printf("JUMPIFEQ label_div_1_%i LF@tmp_type_1_%i string@float\n", tmp_counter, tmp_counter); //129
                printf("INT2FLOATS\n");                                 //129
                printf("LABEL label_div_1_%i\n", tmp_counter);          //129
                printf("PUSHS LF@tmp_div_2_%i\n", tmp_counter);          //129
                printf("JUMPIFEQ label_div_2_%i LF@tmp_type_2_%i string@float\n", tmp_counter, tmp_counter); //129
                printf("INT2FLOATS\n");
                printf("LABEL label_div_2_%i\n", tmp_counter);          //129
                printf("DIVS\n");
                tmp_counter++;
                def_var(temp_d_4);      //printf("DEFVAR LF@tmp_div_4_%i\n", tmp_counter - 1 );        //129
                //printf("DEFVAR LF@tmp_type_4_%i\n", tmp_counter - 1 );       //129
                printf("POPS LF@tmp_div_4_%i\n", tmp_counter - 1 );           //129
                def_var(temp_d_3);      //printf("DEFVAR LF@tmp_div_3_%i\n", tmp_counter - 1 );        //129
                def_var(temp_t_3);      //printf("DEFVAR LF@tmp_type_3_%i\n", tmp_counter - 1 );       //129
                printf("POPS LF@tmp_div_3_%i\n", tmp_counter - 1 );           //129
                printf("TYPE LF@tmp_type_3_%i LF@tmp_div_3_%i\n", tmp_counter - 1, tmp_counter - 1); //129
                printf("PUSHS LF@tmp_div_3_%i\n", tmp_counter - 1);          //129
                printf("PUSHS LF@tmp_div_4_%i\n", tmp_counter - 1);          //129
                printf("JUMPIFEQ label_div_4_%i LF@tmp_type_3_%i string@float\n", tmp_counter - 1, tmp_counter - 1); //129
                printf("FLOAT2INTS\n");
                printf("LABEL label_div_4_%i\n", tmp_counter - 1);          //129

                break;

            }
            generate_code_in_node(node->BinaryOperator.left);
            generate_code_in_node(node->BinaryOperator.right);

            switch (node->BinaryOperator.operator) {
                case AST_PLUS: printf("ADDS\n"); break;
                case AST_MINUS: printf("SUBS\n"); break;
                case AST_MUL: printf("MULS\n"); break;
                case AST_DIV: printf("DIVS\n"); break;  // k tomuto by nemalo nikdy dojst
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


            // Definícia element_bind, ak existuje
            if (node->WhileCycle.element_bind != NULL) {
                if(!(is_it_local(node->WhileCycle.element_bind))){
                    def_var(node->WhileCycle.element_bind);
                    add_to_local(node->WhileCycle.element_bind);
                }

            }

            // Label pre začiatok while cyklu
            char temp_while_cnt[32];
            snprintf(temp_while_cnt, sizeof(temp_while_cnt), "while_cnt_tmp_%d", current_while);
            def_var(temp_while_cnt);
            printf("MOVE LF@%s int@0\n", temp_while_cnt);
            add_while_stack(current_while);
            printf("LABEL while_start_%d\n", current_while);

            // Generovanie výrazu v podmienke
            generate_code_in_node(node->WhileCycle.expression);

            // Rozhodovanie podľa typu výrazu
            switch (node->WhileCycle.expression->type) {
                case AST_IDENTIFIER:
                case AST_INT:
                case AST_FLOAT:
                case AST_STRING: {
                    // Porovnanie s nil
                    printf("PUSHS nil@nil\n");
                    printf("JUMPIFEQS while_end_%d\n", current_while);
                    break;
                }
                default: {
                    // Porovnanie s bool@false
                    printf("PUSHS bool@false\n");
                    printf("JUMPIFEQS while_end_%d\n", current_while);
                    break;
                }
            }

            // Ak je element_bind definovaný, nastav jeho hodnotu
            if (node->WhileCycle.element_bind != NULL) {
                printf("MOVE LF@%s %s%s\n", node->WhileCycle.element_bind,
                       frame_prefix(node->WhileCycle.expression->Identifier.identifier),
                       node->WhileCycle.expression->Identifier.identifier);
            }

            // Generovanie tela while cyklu
            generate_code_in_node(node->WhileCycle.block);

            // Návrat na začiatok while cyklu
            printf("ADD LF@%s LF@%s int@1\n", temp_while_cnt, temp_while_cnt);
            printf("JUMP while_start_%d\n", current_while);

            // Label pre koniec while cyklu
            printf("LABEL while_end_%d\n", current_while);

            remove_while_stack();
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
    free_while_stack();
    return 0;
}

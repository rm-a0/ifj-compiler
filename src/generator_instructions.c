/**
 * @file parser.c
 * @brief Implementation of a generator_instruction.
 * @authors Martin Kandera (xkande00)
*/

#include "generator_instructions.h"
#include "generator.h"

char* escape_string(const char* input) {
    if (input == NULL) return NULL;

    size_t length = strlen(input);
    size_t buffer_size = length * 4 + 1; // Predpokladáme max. veľkosť po rozšírení
    char* output = malloc(buffer_size);
    if (output == NULL) {
        fprintf(stderr, "ERROR: Memory allocation failed\n");
        exit(99);
    }
    //register_allocation(output);
    size_t j = 0;
    for (size_t i = 0; i < length; i++) {
        if (input[i] == '\\' && input[i + 1] == 'n') {
            // Ak sa nájde sekvencia "\n", nahradiť "\010"
            strcpy(&output[j], "\\010");
            j += 4;
            i++; // Preskočíme aj znak 'n'
        } else {
            switch (input[i]) {
                case ' ':
                    strcpy(&output[j], "\\032");
                    j += 4;
                    break;
                case '\\':
                    strcpy(&output[j], "\\092");
                    j += 4;
                    break;
                case '#':
                    strcpy(&output[j], "\\035");
                    j += 4;
                    break;
                default:
                    output[j++] = input[i];
            }
        }
    }
    output[j] = '\0'; // Ukončenie reťazca
    return output;
}


const char* frame_prefix(const char* var) {
    return "LF@";
}

void print_with_indent(const char* text) {
    printf("    %s\n", text);  // four spaces before text
}
void print_new_line() {
    printf("\n");
}
void label(const char* label_name){
    printf("LABEL %s\n", label_name);
}
void gen_create_frame(){
    printf("CREATEFRAME\n");
}
void gen_push_frame(){
    printf("PUSHFRAME\n");
}
void gen_pop_frame(){
    printf("POPFRAME\n");
}
void def_var(const char* var_name) {
    // Ak sa nachádzame vo while-cykle, pridáme podmienky pre deklaráciu
    if (while_stack.top != NULL) {
        //int this_while = actual_while();
        int unique_tmp = tmp_counter++; // Unikátne číslo pre DEFVAR
        int stack_size = while_stack_size();

        // Prechádzanie celého zásobníka while
        for (int i = 0; i < stack_size; i++) {
            int current_while = get_while_at_index(i);
            printf("JUMPIFNEQ while_end_declaration_%d_%d LF@while_cnt_tmp_%d int@0\n",
                   actual_while(), unique_tmp, current_while);
        }

        printf("DEFVAR LF@%s\n", var_name);

        // Label na koniec deklarácie
        printf("LABEL while_end_declaration_%d_%d\n", actual_while(), unique_tmp);
    } else {
        // Normálna definícia premennej mimo while cyklu
        printf("DEFVAR LF@%s\n", var_name);
    }
}
void call(const char* func){
    printf("CALL %s\n", func);
}
void return_f(){
    printf("RETURN\n");
}
void pushs(const char* var) {
    printf("PUSHS %s%s\n", frame_prefix(var), var);
}
void pops(const char* var) {
    printf("POPS %s%s\n", frame_prefix(var), var);
}
void and(const char* var, const char* symb1, const char* symb2) {
    printf("AND %s%s %s %s\n", frame_prefix(var), var, symb1, symb2);
}
void or(const char* var, const char* symb1, const char* symb2) {
    printf("OR %s%s %s %s\n", frame_prefix(var), var, symb1, symb2);
}
void not(const char* var, const char* symb) {
    printf("NOT %s%s %s\n", frame_prefix(var), var, symb);
}
void concat(const char* var, const char* symb1, const char* symb2) {
    printf("CONCAT %s%s %s%s %s%s\n",
           frame_prefix(var), var,
           frame_prefix(symb1), symb1,
           frame_prefix(symb2), symb2);
}
void type(const char* var, const char* symb){
    printf("TYPE LF@%s %s\n", var, symb);
}
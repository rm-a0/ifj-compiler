/**
 * @file parser.c
 * @brief Implementation of a generator_instruction.
 * @authors Martin Kandera (xkande00)
*/

#include "../inc/generator_instructions.h"

void print_with_indent(const char* text) {
    printf("    %s\n", text);  // four spaces before text
}
void print_new_line() {
    printf("\n");
}
void label(const char* label_name){
    printf("LABEL %s\n", label_name);
}
void create_frame(){
    printf("CREATEFRAME\n");
}
void push_frame(){
    printf("PUSHFRAME\n");
}
void pop_frame(){
    printf("POPFRAME\n");
}
void def_var(const char* var_name, const char* frame_type) {
    if (strcmp(frame_type, "lf") == 0) {
        printf("DEFVAR LF@%s\n", var_name);
    } else if (strcmp(frame_type, "gf") == 0) {
        if (!is_it_global((char*)var_name)) {  // Pretypovanie
            add_to_global((char*)var_name);   // Pretypovanie
            printf("DEFVAR GF@%s\n", var_name);
        } else {
            //fprintf(stderr, "WARNING: Global variable '%s' already defined, skipping.\n", var_name);
        }
    } else {
        fprintf(stderr, "ERROR: Invalid frame type '%s' in def_var with '%s'\n", frame_type, var_name);
        exit(53);
    }
}
void move(const char* var, const char* symb) {
    if (is_it_global(var)) {
        printf("MOVE GF@%s %s\n", var, symb);
    } else {
        printf("MOVE LF@%s %s\n", var, symb);
    }
}
void call(const char* func){
    printf("CALL %s\n", func);
}
void return_f(){
    printf("RETURN\n");
}
void pushs(const char* var) {
    if (is_it_global(var)) {
        printf("PUSHS GF@%s\n", var);
    } else {
        printf("PUSHS LF@%s\n", var);
    }
}

void pops(const char* var) {
    if (is_it_global(var)) {
        printf("POPS GF@%s\n", var);
    } else {
        printf("POPS LF@%s\n", var);
    }
}
void clears(){
    printf("CLEARS\n");
}
void add(const char* var, const char* symb1, const char* symb2) {
    printf("ADD %s %s %s\n", is_it_global(var) ? "GF" : "LF", var, symb1, symb2);
}
void sub(const char* var, const char* symb1, const char* symb2) {
    printf("SUB %s %s %s\n", is_it_global(var) ? "GF" : "LF", var, symb1, symb2);
}
void mul(const char* var, const char* symb1, const char* symb2) {
    printf("MUL %s %s %s\n", is_it_global(var) ? "GF" : "LF", var, symb1, symb2);
}
void div_f(const char* var, const char* symb1, const char* symb2) {
    printf("DIV %s %s %s\n", is_it_global(var) ? "GF" : "LF", var, symb1, symb2);
}
void idiv(const char* var, const char* symb1, const char* symb2) {
    printf("IDIV %s %s %s\n", is_it_global(var) ? "GF" : "LF", var, symb1, symb2);
}
void add_s(const char* var) {
    if (is_it_global((char*)var)) {
        printf("ADDS GF@%s\n", var);
    } else {
        printf("ADDS LF@%s\n", var);
    }
}
void sub_s(const char* var) {
    if (is_it_global((char*)var)) {
        printf("SUBS GF@%s\n", var);
    } else {
        printf("SUBS LF@%s\n", var);
    }
}
void mul_s(const char* var) {
    if (is_it_global((char*)var)) {
        printf("MULS GF@%s\n", var);
    } else {
        printf("MULS LF@%s\n", var);
    }
}
void div_s(const char* var) {
    if (is_it_global((char*)var)) {
        printf("DIVS GF@%s\n", var);
    } else {
        printf("DIVS LF@%s\n", var);
    }
}
void idiv_s(const char* var) {
    if (is_it_global((char*)var)) {
        printf("IDIVS GF@%s\n", var);
    } else {
        printf("IDIVS LF@%s\n", var);
    }
}
void lt(const char* var, const char* symb1, const char* symb2) {
    if (is_it_global((char*)var)) {
        printf("LT GF@%s %s %s\n", var, symb1, symb2);  // Globálna premenná
    } else {
        printf("LT LF@%s %s %s\n", var, symb1, symb2);  // Lokálna premenná
    }
}
void gt(const char* var, const char* symb1, const char* symb2) {
    if (is_it_global((char*)var)) {
        printf("GT GF@%s %s %s\n", var, symb1, symb2);  // Globálna premenná
    } else {
        printf("GT LF@%s %s %s\n", var, symb1, symb2);  // Lokálna premenná
    }
}
void eq(const char* var, const char* symb1, const char* symb2) {
    if (is_it_global((char*)var)) {
        printf("EQ GF@%s %s %s\n", var, symb1, symb2);  // Globálna premenná
    } else {
        printf("EQ LF@%s %s %s\n", var, symb1, symb2);  // Lokálna premenná
    }
}
void lts(const char* label) {
    if (is_it_global((char*)label)) {
        printf("LTS GF@%s\n", label);  // Globálny label
    } else {
        printf("LTS LF@%s\n", label);  // Lokálny label
    }
}
void gts(const char* label) {
    if (is_it_global((char*)label)) {
        printf("GTS GF@%s\n", label);  // Globálny label
    } else {
        printf("GTS LF@%s\n", label);  // Lokálny label
    }
}
void eqs(const char* label) {
    if (is_it_global((char*)label)) {
        printf("EQS GF@%s\n", label);  // Globálny label
    } else {
        printf("EQS LF@%s\n", label);  // Lokálny label
    }
}
void and(const char* var, const char* symb1, const char* symb2) {
    printf("AND %s %s %s\n",
           is_it_global((char*)var) ? "GF@" : "LF@", var, symb1, symb2);
}
void or(const char* var, const char* symb1, const char* symb2) {
    printf("OR %s %s %s\n",
           is_it_global((char*)var) ? "GF@" : "LF@", var, symb1, symb2);
}
void not(const char* var, const char* symb) {
    printf("NOT %s %s\n",
           is_it_global((char*)var) ? "GF@" : "LF@", var, symb);
}
void ands(const char* var) {
    printf("ANDS %s%s\n",
           is_it_global((char*)var) ? "GF@" : "LF@", var);
}
void ors(const char* var) {
    printf("ORS %s%s\n",
           is_it_global((char*)var) ? "GF@" : "LF@", var);
}
void nots(const char* var) {
    printf("NOTS %s%s\n",
           is_it_global((char*)var) ? "GF@" : "LF@", var);
}
void int2float(const char* var, const char* symb){
    printf("INT2FLOAT LF@%s %s\n", var, symb);
}
void float2int(const char* var, const char* symb){
    printf("FLOAT2INT LF@%s %s\n", var, symb);
}
void int2char(const char* var, const char* symb){
    printf("INT2CHAR LF@%s %s\n", var, symb);
}
void stri2int(const char* var, const char* symb1, const char* symb2){
    printf("STRI2INT LF@%s %s %s\n", var, symb1, symb2);
}
//TODO: stuck-y majú len labely, nie symboly
void int2floats(const char* var, const char* symb){
    printf("INT2FLOATS LF@%s %s\n", var, symb);
}
void float2ints(const char* var, const char* symb){
    printf("FLOAT2INTS LF@%s %s\n", var, symb);
}
void int2chars(const char* var, const char* symb){
    printf("INT2CHARS LF@%s %s\n", var, symb);
}
void stri2ints(const char* var, const char* symb1, const char* symb2){
    printf("STRI2INTS LF@%s %s %s\n", var, symb1, symb2);
}
void read(const char* var, const char* type){
    printf("READ LF@%s %s\n", var, type);
}
void write(const char* symb) {
    if (is_it_global((char*)symb)) {
        printf("WRITE GF@%s\n", symb);  // Globálna premenná
    } else {
        printf("WRITE LF@%s\n", symb);  // Lokálna premenná
    }
}
void concat(const char* var, const char* symb1, const char* symb2){
    printf("CONCAT LF@%s %s %s\n", var, symb1, symb2);
}
void str_len(const char* var, const char* symb){
    printf("STRLEN LF@%s %s\n", var, symb);
}
void get_char(const char* var, const char* symb1, const char* symb2){
    printf("GETCHAR LF@%s %s %s\n", var, symb1, symb2);
}
void set_char(const char* var, const char* symb1, const char* symb2){
    printf("SETCHAR LF@%s %s %s\n", var, symb1, symb2);
}
void type(const char* var, const char* symb){
    printf("TYPE LF@%s %s\n", var, symb);
}
void jump(const char* label){
    printf("JUMP %s\n", label);
}

//TODO: stuck-y majú len labely, nie symboly
void jump_eq(const char* label, const char* symb1, const char* symb2){
    printf("JUMPIFEQ %s %s %s\n", label, symb1, symb2);
}
void jump_neq(const char* label, const char* symb1, const char* symb2){
    printf("JUMPIFNEQ %s %s %s\n", label, symb1, symb2);
}
void jump_eqs(const char* label, const char* symb1, const char* symb2){
    printf("JUMPIFEQS %s %s %s\n", label, symb1, symb2);
}
void jump_neqs(const char* label, const char* symb1, const char* symb2){
    printf("JUMPIFNEQS %s %s %s\n", label, symb1, symb2);
}
void exit_f(){
    printf("EXIT\n");
}
void break_f(){
    printf("BREAK\n");
}
void dprint(const char* symb){
    printf("DPRINT %s\n", symb);
}
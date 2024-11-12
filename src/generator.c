/**
 * @file parser.c
 * @brief Implementation of a generator.
 * @authors Martin Kandera (xkande00)
*/

#include "../inc/generator.h"

void print_with_indent(const char* text) {
    printf("    %s\n", text);  // four spaces before text
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
void def_var (const char* var){
    printf("DEFVAR LF@%s", var);
}
void move(const char* var, const char* symb){
    printf("MOVE LF@%s %s\n", var, symb);
}
void call(const char* func){
    printf("CALL %s\n", func);
}
void return_f(){
    printf("RETURN\n");
}
void pushs(const char* symb){
    printf("PUSHS %s\n", symb);
}
void pops(const char* var){
    printf("POPS %s\n", var);
}
void clears(){
    printf("CLEARS\n");
}
void add(const char* var, const char* symb1, const char* symb2){
    printf("ADD LF@%s %s %s\n", var, symb1, symb2);
}
void sub(const char* var, const char* symb1, const char* symb2){
    printf("SUB LF@%s %s %s\n", var, symb1, symb2);
}
void mul(const char* var, const char* symb1, const char* symb2){
    printf("MUL LF@%s %s %s\n", var, symb1, symb2);
}
void div_f(const char* var, const char* symb1, const char* symb2){
    printf("DIV LF@%s %s %s\n", var, symb1, symb2);
}
void idiv(const char* var, const char* symb1, const char* symb2){
    printf("IDIV LF@%s %s %s\n", var, symb1, symb2);
}
void add_s(const char* var, const char* symb1, const char* symb2){
    printf("ADDS LF@%s %s %s\n", var, symb1, symb2);
}
void sub_s(const char* var, const char* symb1, const char* symb2){
    printf("SUBS LF@%s %s %s\n", var, symb1, symb2);
}
void mul_s(const char* var, const char* symb1, const char* symb2){
    printf("MULS LF@%s %s %s\n", var, symb1, symb2);
}
void div_s(const char* var, const char* symb1, const char* symb2){
    printf("DIVS LF@%s %s %s\n", var, symb1, symb2);
}
void idiv_s(const char* var, const char* symb1, const char* symb2){
    printf("IDIVS LF@%s %s %s\n", var, symb1, symb2);
}
void lt(const char* var, const char* symb1, const char* symb2){
    printf("LT LF@%s %s %s\n", var, symb1, symb2);
}
void gt(const char* var, const char* symb1, const char* symb2){
    printf("GT LF@%s %s %s\n", var, symb1, symb2);
}
void eq(const char* var, const char* symb1, const char* symb2){
    printf("EQ LF@%s %s %s\n", var, symb1, symb2);
}
void lts(const char* var, const char* symb1, const char* symb2){
    printf("LTS LF@%s %s %s\n", var, symb1, symb2);
}
void gts(const char* var, const char* symb1, const char* symb2){
    printf("GTS LF@%s %s %s\n", var, symb1, symb2);
}
void eqs(const char* var, const char* symb1, const char* symb2){
    printf("EQS LF@%s %s %s\n", var, symb1, symb2);
}
void and(const char* var, const char* symb1, const char* symb2){
    printf("AND LF@%s %s %s\n", var, symb1, symb2);
}
void or(const char* var, const char* symb1, const char* symb2){
    printf("OR LF@%s %s %s\n", var, symb1, symb2);
}
void not(const char* var, const char* symb){
    printf("NOT LF@%s %s\n", var, symb);
}
void ands(const char* var, const char* symb1, const char* symb2){
    printf("ANDS LF@%s %s %s\n", var, symb1, symb2);
}
void ors(const char* var, const char* symb1, const char* symb2){
    printf("ORS LF@%s %s %s\n", var, symb1, symb2);
}
void nots(const char* var, const char* symb){
    printf("NOTS LF@%s %s\n", var, symb);
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
void write(const char* symb){
    printf("WRITE %s\n", symb);
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








void generate_assignment(ASTNode* node){

}

void generate_expression(ASTNode* node){

}

void generate_if(ASTNode* node){

}

void generate_loop(ASTNode* node){

}
void generate_code_in_node(ASTNode* node){
/*    switch (node->type) {
        case AST_PROGRAM:
            printf("")

    }
    */
}

int generate_code(ASTNode* root){
    if (root == NULL) return 51; // ast root == NULL

    printf(".IFJcode24\n");
    generate_code_in_node(root);
    /*
     * for (int i = 0; i < root->Program.decl_count; ++i) {
        printf("deklaracia #%i\n", (root->Program.decl_count - i));
    }
     */

    return 0;
}

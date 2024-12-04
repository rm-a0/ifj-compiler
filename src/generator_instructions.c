/**
 * @file parser.c
 * @brief Implementation of a generator_instruction.
 * @authors Martin Kandera (xkande00)
*/

#include "generator_instructions.h"
#include "generator.h"

/**
 * @brief Escapes special characters in a string for use in IFJcode24.
 * @param input The input string to escape.
 * @return A dynamically allocated escaped string. The caller must free the memory.
 */
char* escape_string(const char* input) {
    if (input == NULL) return NULL;

    size_t length = strlen(input);
    size_t buffer_size = length * 4 + 1; // Maximum size after escaping.
    char* output = malloc(buffer_size);
    if (output == NULL) {
        fprintf(stderr, "ERROR: Memory allocation failed\n");
        exit(99);
    }

    size_t j = 0;
    for (size_t i = 0; i < length; i++) {
        if (input[i] == '\n') {
            // Replace newline character with "\010".
            strcpy(&output[j], "\\010");
            j += 4;
        } else if (input[i] == '\\' && input[i + 1] == 'n') {
            // Replace "\n" with "\010".
            strcpy(&output[j], "\\010");
            j += 4;
            i++; // Skip the next character.
        } else {
            switch (input[i]) {
                // Escape special characters.
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
    output[j] = '\0'; // Null-terminate the string.
    return output;
}

/**
 * @brief Returns the prefix used for variables in the local frame.
 * @param var The variable name.
 * @return The frame prefix string (always "LF@" in this implementation).
 */
const char* frame_prefix(const char* var) {
    return "LF@";
}

/**
 * @brief Prints text with indentation for better readability.
 * @param text The text to print.
 */
void print_with_indent(const char* text) {
    printf("    %s\n", text);  // four spaces before text
}

/**
 * @brief Prints a new line for better readability.
 */
void print_new_line() {
    printf("\n");
}

/**
 * @brief Generates a label instruction.
 * @param label_name The name of the label.
 */
void label(const char* label_name){
    printf("LABEL %s\n", label_name);
}

/**
 * @brief Generates a CREATEFRAME instruction.
 */
void gen_create_frame(){
    printf("CREATEFRAME\n");
}

/**
 * @brief Generates a PUSHFRAME instruction.
 */
void gen_push_frame(){
    printf("PUSHFRAME\n");
}

/**
 * @brief Generates a POPFRAME instruction.
 */
void gen_pop_frame(){
    printf("POPFRAME\n");
}

/**
 * @brief Defines a variable in the local frame. Handles special cases for variables in while loops.
 * @param var_name The name of the variable.
 */
void def_var(const char* var_name) {
    // check if the declaration is inside a while loop
    if (while_stack.top != NULL) {
        int unique_tmp = tmp_counter++; // Unique temporary variable for the while loop
        int stack_size = while_stack_size();

        // Check all nested while loops for unique variable declarations.
        for (int i = 0; i < stack_size; i++) {
            int current_while = get_while_at_index(i);
            printf("JUMPIFNEQ while_end_declaration_%d_%d LF@while_cnt_tmp_%d int@0\n",
                   actual_while(), unique_tmp, current_while);
        }

        printf("DEFVAR LF@%s\n", var_name);

        // label for the end of the declaration
        printf("LABEL while_end_declaration_%d_%d\n", actual_while(), unique_tmp);
    } else {
        // No while loop, just declare the variable
        printf("DEFVAR LF@%s\n", var_name);
    }
}

/**
 * @brief Generates a CALL instruction for a function.
 * @param func The function name to call.
 */
void call(const char* func){
    printf("CALL %s\n", func);
}

/**
 * @brief Generates a RETURN instruction.
 */
void return_f(){
    printf("RETURN\n");
}

/**
 * @brief Pushes a variable onto the stack.
 * @param var The variable name.
 */
void pushs(const char* var) {
    printf("PUSHS %s%s\n", frame_prefix(var), var);
}

/**
 * @brief Pops a value from the stack into a variable.
 * @param var The variable name.
 */
void pops(const char* var) {
    printf("POPS %s%s\n", frame_prefix(var), var);
}

/**
 * @brief Generates an AND operation instruction.
 * @param var The variable to store the result.
 * @param symb1 The first operand.
 * @param symb2 The second operand.
 */
void and(const char* var, const char* symb1, const char* symb2) {
    printf("AND %s%s %s %s\n", frame_prefix(var), var, symb1, symb2);
}

/**
 * @brief Generates an OR operation instruction.
 * @param var The variable to store the result.
 * @param symb1 The first operand.
 * @param symb2 The second operand.
 */
void or(const char* var, const char* symb1, const char* symb2) {
    printf("OR %s%s %s %s\n", frame_prefix(var), var, symb1, symb2);
}

/**
 * @brief Generates a NOT operation instruction.
 * @param var The variable to store the result.
 * @param symb The operand.
 */
void not(const char* var, const char* symb) {
    printf("NOT %s%s %s\n", frame_prefix(var), var, symb);
}

/**
 * @brief Generates a CONCAT operation instruction to concatenate two strings.
 * @param var The variable to store the result.
 * @param symb1 The first string operand.
 * @param symb2 The second string operand.
 */
void concat(const char* var, const char* symb1, const char* symb2) {
    printf("CONCAT %s%s %s%s %s%s\n",
           frame_prefix(var), var,
           frame_prefix(symb1), symb1,
           frame_prefix(symb2), symb2);
}

/**
 * @brief Generates a TYPE instruction to determine the type of a value.
 * @param var The variable to store the type.
 * @param symb The symbol whose type is determined.
 */
void type(const char* var, const char* symb){
    printf("TYPE LF@%s %s\n", var, symb);
}
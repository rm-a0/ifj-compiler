/**
 * @file parser.c
 * @brief Implementation of a generator.
 * @authors Martin Kandera (xkande00)
*/

#include <setjmp.h>
#include "generator.h"
#include "generator_instructions.h"

jmp_buf error_buf;                  // Buffer for error handling

#define MAX_VAR_NAME_LENGTH 128             // Maximum length of a variable name in the global frame (GF)
#define MAX_GF_VAR_COUNT 1024               // Maximum number of global variables
#define MAX_LF_VAR_COUNT 1024               // Maximum number of local variables
char* lf_vars[MAX_LF_VAR_COUNT] = {NULL};   // Initialization of all elements to NULL for the local frame array

static int if_counter = 1420;       // Initial numbering for unique labels used in if statements
static int while_counter = 1420;    // Initial numbering for unique labels used in while loops
int tmp_counter = 128;              // Initial numbering for unique temporary variables

/**
 * @brief Error handler for the generator.
 */
void generator_error_handler(int error_code) {
    printf("debug2 %i\n", error_code);
    longjmp(error_buf, error_code);
}
/**
 * @brief Get the current value of the temporary variable counter.
 * @return Current temporary variable counter value.
 */
int get_tmp_counter() {
    return tmp_counter;
}

/**
 * @brief Increment the temporary variable counter by 1.
 */
void increment_tmp_counter() {
    tmp_counter++;
}

/**
 * @brief Global stack structure for managing nested while loops.
 */
WhileStack while_stack = { .top = NULL };


/**
 * @brief Get the current size of the while loop stack.
 * @return Number of while loops currently in the stack.
 */
int while_stack_size() {
    int size = 0;
    WhileStackNode *current = while_stack.top;
    while (current != NULL) {
        size++;
        current = current->next;
    }
    return size;
}

/**
 * @brief Retrieve the while loop identifier at a specific index in the stack.
 * @param index Index in the stack (0 is the bottom).
 * @return Identifier of the while loop at the given index.
 */
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

/**
 * @brief Add a new while loop identifier to the global stack.
 * @param while_number The unique identifier for the while loop.
 */
void add_while_stack(int while_number) {
    WhileStackNode *new_node = (WhileStackNode *)malloc(sizeof(WhileStackNode));
    if (!new_node) {
        fprintf(stderr, "Chyba: Nepodarilo sa alokovať pamäť pre uzol zásobníka.\n");
        exit(99);
    }
    new_node->while_number = while_number;
    new_node->next = while_stack.top;
    while_stack.top = new_node;
}

/**
 * @brief Remove the topmost while loop identifier from the global stack.
 */
void remove_while_stack() {
    if (while_stack.top == NULL) {
        fprintf(stderr, "Chyba: Zásobník while cyklov je prázdny.\n");
        exit(99);
    }
    WhileStackNode *temp = while_stack.top;
    while_stack.top = while_stack.top->next;
    free(temp);
}

/**
 * @brief Retrieve the identifier of the current (topmost) while loop.
 * @return Identifier of the current while loop.
 */
int actual_while() {
    if (while_stack.top == NULL) {
        fprintf(stderr, "Chyba: Zásobník while cyklov je prázdny.\n");
        exit(99);
    }
    return while_stack.top->while_number;
}

/**
 * @brief Free the entire while stack, releasing all allocated memory.
 */
void free_while_stack() {
    WhileStackNode *current = while_stack.top;
    while (current) {
        WhileStackNode *temp = current;
        current = current->next;
        free(temp);
    }
    while_stack.top = NULL;
}

/**
 * @brief Dynamic array structure for managing variables in the local frame.
 */
typedef struct LocalFrameArray {
    char **variables;    ///< Array of variable names.
    size_t size;         ///< Current number of variables.
    size_t capacity;     ///< Capacity of the array.
} LocalFrameArray;

/**
 * @brief Global dynamic local frame for managing local variables.
 */
static LocalFrameArray local_frame = {NULL, 0, 0};

/**
 * @brief Initialize the dynamic local frame array.
 */
void init_local_frame() {
    local_frame.size = 0;
    local_frame.capacity = 8; // Počiatočná kapacita
    local_frame.variables = malloc(local_frame.capacity * sizeof(char *));
    if (local_frame.variables == NULL) {
        generator_error_handler(99);
    }
}

/**
 * @brief Free all memory associated with the dynamic local frame array.
 */
void free_local_frame() {
    for (size_t i = 0; i < local_frame.size; ++i) {
        free(local_frame.variables[i]);
    }
    free(local_frame.variables);
    local_frame.variables = NULL;
    local_frame.size = 0;
    local_frame.capacity = 0;
}

/**
 * @brief Clear all variables in the local frame without deallocating the array.
 */
void clear_local_frame() {
    for (size_t i = 0; i < local_frame.size; ++i) {
        free(local_frame.variables[i]);
    }
    local_frame.size = 0;
}

/**
 * @brief Add a variable to the local frame array.
 * @param var_name Name of the variable to add.
 */
void add_to_local(const char* var_name) {
    // Check if the variable already exists
    for (size_t i = 0; i < local_frame.size; ++i) {
        if (strcmp(local_frame.variables[i], var_name) == 0) {
            return; // Variable already exists, no need to add it
        }
    }

    // Expand the array if necessary
    if (local_frame.size >= local_frame.capacity) {
        local_frame.capacity *= 2; // Double the capacity
        local_frame.variables = realloc(local_frame.variables, local_frame.capacity * sizeof(char *));
        if (local_frame.variables == NULL) {
            generator_error_handler(99);
        }
    }

    // Add the new variable with dynamic allocation
    size_t name_length = strlen(var_name) + 1; // Include null terminator
    local_frame.variables[local_frame.size] = malloc(name_length);
    if (local_frame.variables[local_frame.size] == NULL) {
        generator_error_handler(99);
    }
    strncpy(local_frame.variables[local_frame.size], var_name, name_length);
    local_frame.size++;
}

/**
 * @brief Check if a variable exists in the local frame.
 * @param var_name Name of the variable to check.
 * @return true if the variable exists, false otherwise.
 */
bool is_it_local(const char* var_name) {
    for (size_t i = 0; i < local_frame.size; ++i) {
        if (strcmp(local_frame.variables[i], var_name) == 0) {
            return true; // Variable exists
        }
    }
    return false;  // Variable does not exist
}

/**
 * @brief Generate code for each node in the AST recursively.
 * @param node The current AST node to process.
 */
void generate_code_in_node(ASTNode* node){
    if (node == NULL) return;
    // Code generation logic for different AST node types
    switch (node->type) {
        case AST_PROGRAM:
            // Code generation for the entire program
            for (int i = 0; i < node->Program.decl_count; ++i) {
                if (strcmp(node->Program.declarations[i]->FnDecl.fn_name, "main") == 0) {
                    label("main");
                    gen_create_frame();
                    gen_push_frame();
                    generate_code_in_node(node->Program.declarations[i]);  // generating `main` function body
                    gen_pop_frame();
                    printf("EXIT int@0\n");
                    print_new_line();
                }
                else {
                    label(node->Program.declarations[i]->FnDecl.fn_name);
                    gen_create_frame();
                    gen_push_frame();
                    generate_code_in_node(node->Program.declarations[i]);  // generating other functions body
                    gen_pop_frame();
                    return_f();
                    print_new_line();
                    clear_local_frame();
                }
            }
            break;

        case AST_FN_DECL:
            // Code generation for function declarations
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
            break;

        case AST_VAR_DECL:
        case AST_CONST_DECL: {
            // Handle variable or constant declarations.
            if (!(is_it_local(node->VarDecl.var_name))) {
                def_var(node->VarDecl.var_name);
                add_to_local(node->VarDecl.var_name);
            }

            // Check if the variable has an initialization expression.
            if (node->ConstDecl.expression) {
                // Handle different types of initialization expressions.
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
                // Handle function calls and built-in functions.
                if (strcmp(node->ConstDecl.expression->FnCall.fn_name, "ifj.string") == 0) {
                    printf("MOVE LF@%s string@%s\n", node->ConstDecl.const_name,
                           escape_string(node->ConstDecl.expression->FnCall.args[0]->Argument.expression->String.string));

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
            // Process a block of statements.
            for (int i = 0; i < node->Block.node_count; ++i) {
                ASTNode* block_node = node->Block.nodes[i];
                // Special handling for string concatenation
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
                        concat(result, arg1, arg2);
                        continue;

                    }
                }
                // Special handling for string length
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
            // Generate code for an if-else construct.
            const char *fn_name = node->FnCall.fn_name;
            // Handle built-in functions
            if (strcmp(fn_name, "ifj.length") == 0) {
                generate_code_in_node(node->FnCall.args[0]->Argument.expression);
                char temp_var[32];
                snprintf(temp_var, sizeof(temp_var), "tmp_length_%d", tmp_counter++);
                def_var(temp_var);
                printf("STRLEN LF@%s %s%s\n", temp_var,
                       frame_prefix(node->FnCall.args[0]->Argument.expression->Identifier.identifier),
                       node->FnCall.args[0]->Argument.expression->Identifier.identifier);
                printf("PUSHS LF@%s\n", temp_var);
                break;
            }
            if (strcmp(fn_name, "ifj.substring") == 0) {
                // Generate code for the arguments
                generate_code_in_node(node->FnCall.args[0]->Argument.expression); // s
                generate_code_in_node(node->FnCall.args[1]->Argument.expression); // i
                generate_code_in_node(node->FnCall.args[2]->Argument.expression); // j

                // Create temporary variables for the arguments
                char temp_s[32], temp_i[32], temp_j[32], temp_result[32], temp_char[32];
                snprintf(temp_s, sizeof(temp_s), "tmp_s_%d", tmp_counter);
                snprintf(temp_i, sizeof(temp_i), "tmp_i_%d", tmp_counter);
                snprintf(temp_j, sizeof(temp_j), "tmp_j_%d", tmp_counter);
                snprintf(temp_char, sizeof(temp_char), "tmp_char_%d", tmp_counter);
                snprintf(temp_result, sizeof(temp_result), "tmp_result_%d", tmp_counter);

                def_var(temp_s);
                def_var(temp_i);
                def_var(temp_j);
                def_var(temp_char);
                def_var(temp_result);


                // Pop arguments from the stack
                printf("POPS LF@%s\n", temp_j);
                printf("POPS LF@%s\n", temp_i);
                printf("POPS LF@%s\n", temp_s);

                printf("MOVE LF@%s string@\n", temp_result);

                // Check if the arguments are valid
                printf("LABEL substring_loop_start_%i\n", while_counter);
                printf("PUSHS LF@%s\n", temp_i);
                printf("PUSHS LF@%s\n", temp_j);
                printf("LTS\n");
                printf("PUSHS bool@false\n");
                printf("JUMPIFEQS substring_loop_end_%i\n", while_counter);

                printf("GETCHAR LF@%s LF@%s LF@%s\n", temp_char, temp_s, temp_i); // Získanie znaku na indexe `i`
                printf("CONCAT LF@%s LF@%s LF@%s\n", temp_result, temp_result, temp_char); // Pridanie znaku do výsledku
                printf("ADD LF@%s LF@%s int@1\n", temp_i, temp_i); // Zvýšenie `i`
                printf("JUMP substring_loop_start_%i\n", while_counter);

                printf("LABEL substring_loop_end_%i\n", while_counter);
                printf("PUSHS LF@%s\n", temp_result); // Push výsledný substring na zásobník


                while_counter++;
                break;
            }
            if (strcmp(fn_name, "ifj.strcmp") == 0) {
                // Generate code for both string arguments
                generate_code_in_node(node->FnCall.args[0]->Argument.expression);
                generate_code_in_node(node->FnCall.args[1]->Argument.expression);

                // Create a temporary variable for comparison result
                char temp_var[32];
                snprintf(temp_var, sizeof(temp_var), "tmp_cmp_%d", tmp_counter);
                def_var(temp_var);
                char temp_label_equal[32], temp_label_greater[32], temp_label_end[32];
                snprintf(temp_label_equal, sizeof(temp_label_equal), "strcmp_equal_%d", tmp_counter);
                snprintf(temp_label_greater, sizeof(temp_label_greater), "strcmp_greater_%d", tmp_counter);
                snprintf(temp_label_end, sizeof(temp_label_end), "strcmp_end_%d", tmp_counter);
                tmp_counter++;


                // Compare the strings
                printf("JUMPIFEQS %s\n", temp_label_equal);

                // If s1 < s2
                printf("LT LF@%s %s%s %s%s\n", temp_var,
                       frame_prefix(node->FnCall.args[0]->Argument.expression->Identifier.identifier),
                       node->FnCall.args[0]->Argument.expression->Identifier.identifier,
                       frame_prefix(node->FnCall.args[1]->Argument.expression->Identifier.identifier),
                       node->FnCall.args[1]->Argument.expression->Identifier.identifier);
                printf("PUSHS LF@%s\n", temp_var);
                printf("PUSHS bool@true\n");
                printf("JUMPIFNEQS %s\n", temp_label_greater);

                // Set temp_var to -1 (s1 < s2)
                printf("MOVE LF@%s int@-1\n", temp_var);
                printf("JUMP %s\n", temp_label_end);

                // If s1 == s2
                printf("LABEL %s\n", temp_label_equal);
                printf("MOVE LF@%s int@0\n", temp_var);
                printf("JUMP %s\n", temp_label_end);

                // If s1 > s2
                printf("LABEL %s\n", temp_label_greater);
                printf("MOVE LF@%s int@1\n", temp_var);

                // End label
                printf("LABEL %s\n", temp_label_end);

                // Push the result
                printf("PUSHS LF@%s\n", temp_var);
                break;
            }
            if (strcmp(fn_name, "ifj.ord") == 0) {
                // Generate code for string and index
                generate_code_in_node(node->FnCall.args[0]->Argument.expression);
                generate_code_in_node(node->FnCall.args[1]->Argument.expression);

                // Create a temporary variable for the result
                char temp_var[32];
                snprintf(temp_var, sizeof(temp_var), "tmp_ord_%d", tmp_counter++);
                def_var(temp_var);

                // Get the character and convert to ASCII
                printf("STRI2INT LF@%s %s%s %s%s\n", temp_var,
                       frame_prefix(node->FnCall.args[0]->Argument.expression->Identifier.identifier),
                       node->FnCall.args[0]->Argument.expression->Identifier.identifier,
                       frame_prefix(node->FnCall.args[1]->Argument.expression->Identifier.identifier),
                       node->FnCall.args[1]->Argument.expression->Identifier.identifier);

                // Push the result
                printf("PUSHS LF@%s\n", temp_var);
                break;
            }
            if (strcmp(fn_name, "ifj.chr") == 0) {
                // Generate code for the ASCII value
                generate_code_in_node(node->FnCall.args[0]->Argument.expression);

                // Create a temporary variable for the result
                char temp_var[32];
                snprintf(temp_var, sizeof(temp_var), "tmp_chr_%d", tmp_counter++);
                def_var(temp_var);

                // Convert ASCII to character
                printf("INT2CHAR LF@%s %s%s\n", temp_var,
                       frame_prefix(node->FnCall.args[0]->Argument.expression->Identifier.identifier),
                       node->FnCall.args[0]->Argument.expression->Identifier.identifier);

                // Push the result
                printf("PUSHS LF@%s\n", temp_var);
                break;
            }

            // built-in function ifj.print, for changing int to float
            if (strcmp(fn_name, "ifj.i2f") == 0) {
                generate_code_in_node(node->FnCall.args[0]->Argument.expression);
                printf("INT2FLOATS\n");
                break;
            }
            // built-in function ifj.f2i, for changing float to int
            if (strcmp(fn_name, "ifj.f2i") == 0) {
                generate_code_in_node(node->FnCall.args[0]->Argument.expression);
                printf("FLOAT2INTS\n");
                break;
            }
            // built-in function ifj.length

            if (strcmp(fn_name, "ifj.length") == 0) {
                generate_code_in_node(node->FnCall.args[0]->Argument.expression);
            }
            // built-in function ifj.write, ifj.writef64
            if ((strcmp(fn_name, "ifj.write") == 0) || (strcmp(fn_name, "ifj.writef64") == 0)) {
                // If the argument is a string, integer or float literal, write it directly
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
                        // If the argument is a binary operation or a function call, evaluate it first
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
                        generator_error_handler(12);
                }
            } else {
                // Generate code for function arguments
                for (int i = node->FnCall.arg_count; i > 0; --i) {
                    generate_code_in_node(node->FnCall.args[i - 1]);

                }
                call(fn_name);
            }
            break;
        }
        case AST_ASSIGNMENT:
            // Generate code for an assignment.
            // If the expression is an integer, float or string literal, move it directly to the variable
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
                // If the expression is a function call, generate code for the function call and move the result to the variable
                if (node->Assignment.expression->type == AST_FN_CALL) {
                    // If the function call is a built-in function, handle it separately
                    if (strcmp(node->Assignment.expression->FnCall.fn_name, "ifj.string") == 0) {
                        printf("MOVE LF@%s string@%s\n", node->Assignment.identifier,
                               escape_string(node->Assignment.expression->FnCall.args[0]->Argument.expression->String.string));
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
            int current_if = if_counter++; //  unique label for the current if-else block

            // if there is an element bind, check if it is local
            if (node->WhileCycle.element_bind != NULL) {
                if(!(is_it_local(node->WhileCycle.element_bind))){
                    def_var(node->WhileCycle.element_bind);
                    add_to_local(node->WhileCycle.element_bind);
                }
            }

            // generate code for the expression
            generate_code_in_node(node->IfElse.expression);

            // depending on the type of the expression, we need to compare it with nil or bool@false

            // if the expression is an identifier, int, float or string, we need to compare it with nil
            switch (node->IfElse.expression->type) {
                case AST_IDENTIFIER:
                case AST_INT:
                case AST_FLOAT:
                case AST_STRING: {
                    printf("PUSHS nil@nil\n");
                    printf("JUMPIFEQS else_block_%d\n", current_if);
                    break;
                }
                default: {
                    // if the expression is a function call, we need to compare it with bool@false
                    printf("PUSHS bool@false\n");
                    printf("JUMPIFEQS else_block_%d\n", current_if);
                    break;
                }
            }

            //  if there is an element bind, move the value of the expression to the element bind
            if (node->IfElse.element_bind != NULL) {
                printf("MOVE LF@%s %s%s\n", node->IfElse.element_bind,
                       frame_prefix(node->IfElse.expression->Identifier.identifier),
                       node->IfElse.expression->Identifier.identifier);
            }

            // generate code for the if block
            generate_code_in_node(node->IfElse.if_block);
            printf("JUMP end_block_%d\n", current_if);

            // else block
            printf("LABEL else_block_%d\n", current_if);
            if (node->IfElse.element_bind != NULL) {
                printf("MOVE LF@%s %s%s\n", node->IfElse.element_bind,
                       frame_prefix(node->IfElse.expression->Identifier.identifier),
                       node->IfElse.expression->Identifier.identifier);
            }
            if (node->IfElse.else_block) {
                generate_code_in_node(node->IfElse.else_block);
            }

            // end block
            printf("LABEL end_block_%d\n", current_if);
            break;
        }

        case AST_ARG:
            // Generate code for an argument.
            generate_code_in_node(node->Argument.expression);
            break;

        case AST_RETURN:
            // Generate code for a return statement.
            // Check if the return expression is an identifier
            if (node->Return.expression && node->Return.expression->type == AST_IDENTIFIER) {
                const char* return_var = node->Return.expression->Identifier.identifier;
                if (return_var != NULL) {
                    pushs(return_var);
                }
            } else {
                generate_code_in_node(node->Return.expression);
            }
            break;

        case AST_BIN_OP:
            // Generate code for a binary operation.
            // Handle division separately to avoid coliisions with different types (int/float)
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

                // it checks if the type of the first operand is float
                def_var(temp_d_2);
                def_var(temp_t_2);
                printf("POPS LF@tmp_div_2_%i\n", tmp_counter);
                printf("TYPE LF@tmp_type_2_%i LF@tmp_div_2_%i\n", tmp_counter, tmp_counter);
                // it checks if the type of the second operand is float
                def_var(temp_d_1);
                def_var(temp_t_1);
                printf("POPS LF@tmp_div_1_%i\n", tmp_counter);
                printf("TYPE LF@tmp_type_1_%i LF@tmp_div_1_%i\n", tmp_counter, tmp_counter);
                printf("PUSHS LF@tmp_div_1_%i\n", tmp_counter);
                // it converts the first operand to float
                printf("JUMPIFEQ label_div_1_%i LF@tmp_type_1_%i string@float\n", tmp_counter, tmp_counter);
                printf("INT2FLOATS\n");
                printf("LABEL label_div_1_%i\n", tmp_counter);
                printf("PUSHS LF@tmp_div_2_%i\n", tmp_counter);
                // it converts the second operand to float
                printf("JUMPIFEQ label_div_2_%i LF@tmp_type_2_%i string@float\n", tmp_counter, tmp_counter);
                printf("INT2FLOATS\n");
                printf("LABEL label_div_2_%i\n", tmp_counter);
                printf("DIVS\n");
                tmp_counter++;
                // it checks if the result is float
                def_var(temp_d_4);
                printf("POPS LF@tmp_div_4_%i\n", tmp_counter - 1 );
                def_var(temp_d_3);
                def_var(temp_t_3);
                printf("POPS LF@tmp_div_3_%i\n", tmp_counter - 1 );
                printf("TYPE LF@tmp_type_3_%i LF@tmp_div_3_%i\n", tmp_counter - 1, tmp_counter - 1);
                printf("PUSHS LF@tmp_div_3_%i\n", tmp_counter - 1);
                printf("PUSHS LF@tmp_div_4_%i\n", tmp_counter - 1);
                printf("JUMPIFEQ label_div_4_%i LF@tmp_type_3_%i string@float\n", tmp_counter - 1, tmp_counter - 1);
                printf("FLOAT2INTS\n");
                printf("LABEL label_div_4_%i\n", tmp_counter - 1);

                break;

            }
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
                    generator_error_handler(12);
            }
            break;

        case AST_WHILE: {
            // Generate code for a while loop.
            int current_while = while_counter++; // generates unique number for the current while loop


            // if statement for element_bind
            if (node->WhileCycle.element_bind != NULL) {
                if(!(is_it_local(node->WhileCycle.element_bind))){
                    def_var(node->WhileCycle.element_bind);
                    add_to_local(node->WhileCycle.element_bind);
                }

            }

            // label for the start of the while loop
            char temp_while_cnt[32];
            snprintf(temp_while_cnt, sizeof(temp_while_cnt), "while_cnt_tmp_%d", current_while);
            def_var(temp_while_cnt);
            printf("MOVE LF@%s int@0\n", temp_while_cnt);
            add_while_stack(current_while);
            printf("LABEL while_start_%d\n", current_while);

            // generate code for the expression
            generate_code_in_node(node->WhileCycle.expression);

            // decision based on the type of the expression
            switch (node->WhileCycle.expression->type) {
                case AST_IDENTIFIER:
                case AST_INT:
                case AST_FLOAT:
                case AST_STRING: {
                    printf("PUSHS nil@nil\n");
                    printf("JUMPIFEQS while_end_%d\n", current_while);
                    break;
                }
                default: {
                    printf("PUSHS bool@false\n");
                    printf("JUMPIFEQS while_end_%d\n", current_while);
                    break;
                }
            }

            // if element_bind is defined, set its value
            if (node->WhileCycle.element_bind != NULL) {
                printf("MOVE LF@%s %s%s\n", node->WhileCycle.element_bind,
                       frame_prefix(node->WhileCycle.expression->Identifier.identifier),
                       node->WhileCycle.expression->Identifier.identifier);
            }

            // generate code for the block
            generate_code_in_node(node->WhileCycle.block);

            // increment the counter
            printf("ADD LF@%s LF@%s int@1\n", temp_while_cnt, temp_while_cnt);
            printf("JUMP while_start_%d\n", current_while);

            // end of the while loop
            printf("LABEL while_end_%d\n", current_while);

            remove_while_stack();
            break;
        }


        case AST_INT:
            // Push an integer value onto the stack.
            printf("PUSHS int@%d\n", node->Integer.number);
            break;

        case AST_FLOAT:
            // Push a float value onto the stack.
            printf("PUSHS float@%a\n", node->Float.number);
            break;

        case AST_STRING: {
            // Push a string value onto the stack.
            char* escaped_string = escape_string(node->String.string);
            printf("PUSHS string@%s\n", escaped_string);
            free(escaped_string);
            break;
        }

        case AST_IDENTIFIER:
            // Push an identifier's value onto the stack.
            printf("PUSHS %s%s\n", frame_prefix(node->Identifier.identifier), node->Identifier.identifier);
            break;

        default:
            print_with_indent("UNKNOWN_NODE_TYPE");
            break;
    }
}

/**
 * @brief Generate code for the given AST root node.
 * @param root Pointer to the root node of the AST.
 * @return 0 on success, 51 if the root is NULL.
 */
int generate_code(ASTNode* root){
    int err_code = 0; // Error code for error handling
    if ((err_code = setjmp(error_buf)) != 0) {
        // This code executes if longjmp is called
        free_local_frame();
        free_while_stack();
        return err_code;
    }

    if (root == NULL) generator_error_handler(99); // Internal error - root is NULL
    init_local_frame();

    printf(".IFJcode24\n");  printf("JUMP main\n");   print_new_line();

    generate_code_in_node(root);

    free_local_frame();
    free_while_stack();
    return 0;

}

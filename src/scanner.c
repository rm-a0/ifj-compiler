/**
 * @file scanner.c
 * @brief Contains definitions of functions declared in scanner.h file
 * @authors Martin Kandera (xkande00)
*/

#include "../inc/scanner.h"
#include "../inc/token.h"
#include "../inc/error.h"

bool devTool = false;

// Implementace funkce pro výpis lexikálního chybového hlášení
void printLexicalError(const char* message, int line) {
    fprintf(stderr, "Lexical Error on line %d: %s\n", line, message);
}
int initScanner(const char* source) {
    if (devTool){
        printf("Initializing scanner ...\n");
        // Check if the source is not NULL
        if (source != NULL) {
            // Print the source to standard output
            printf("Source content:\n%s\n", source);
        } else {
            // Optional message if source is NULL
            if (devTool) {
                printLexicalError("No source provided to scanner", 0);
                return LEXICAL_ERROR;
            }
        }
    }
    //deleting no needed stuff like comments & "ifj conts ifj = @imp..."

    

    return NO_ERROR;
}


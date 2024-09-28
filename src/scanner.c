/**
 * @file scanner.c
 * @brief Contains definitions of functions declared in scanner.h file
 * @authors Martin Kandera (xkande00)
*/

#include "../inc/scanner.h"

// Implementace funkce pro výpis lexikálního chybového hlášení
void printLexicalError(const char* message, int line) {
    fprintf(stderr, "Lexical Error on line %d: %s\n", line, message);
}


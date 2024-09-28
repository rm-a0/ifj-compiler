/**
 * @file scanner.c
 * @brief Contains definitions of functions declared in scanner.h file
 * @authors Martin Kandera (xkande00)
*/
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../inc/scanner.h"
#include "../inc/token.h"
#include "../inc/error.h"

// static char* buffer;  // Buffer pro uložení zpracovaného zdrojového kódu
static const char* current;  // Ukazatel na aktuální znak ve zdrojovém kódu

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
            printLexicalError("No source provided to scanner", 0);
            return LEXICAL_ERROR;
        }
    }
    //deleting no needed stuff like comments & "ifj conts ifj = @imp..."

    // Kontrola, jestli source není NULL
    if (source == NULL) {
        printLexicalError("No source provided to scanner", 0);
        return LEXICAL_ERROR;
    }

    // Buffer pro zpracovaný kód
    size_t length = strlen(source);
    char* buffer = malloc(length + 1);  // Alokace paměti pro zpracovaný kód
    if (buffer == NULL) {
        printLexicalError("Failed to allocate memory for source code buffer", 0);
        return INTERNAL_ERROR;
    }

    const char* src = source;
    char* dest = buffer;

    // Řetězec, který chceme odstranit
    const char* to_remove = "const ifj = @import(\"ifj24.zig\");";
    size_t remove_len = strlen(to_remove);

    // Procházení zdrojového kódu a odstranění komentářů, prázdných řádků a dalších částí
    while (*src != '\0') {
        // Odstranění komentářů začínajících "//"
        if (*src == '/' && *(src + 1) == '/') {
            // Přeskočit '//' a pokračovat až do konce řádku
            src += 2;
            while (*src != '\n' && *src != '\0') {
                src++;
            }
        }
        // Odstranění vzoru "const ifj = @import("ifj24.zig");"
        else if (strncmp(src, to_remove, remove_len) == 0) {
            // Přeskočit celý řetězec
            src += remove_len;
        }
        // Pokud je řádek prázdný, přeskočíme ho
        else if (*src == '\n') {
            // Zkontrolujeme, zda předcházející řádek neobsahuje žádné čitelné znaky
            const char* tmp = dest - 1;
            bool isEmptyLine = true;
            while (tmp >= buffer && *tmp != '\n') {
                if (!isspace(*tmp)) {
                    isEmptyLine = false;
                    break;
                }
                tmp--;
            }
            if (isEmptyLine) {
                src++; // Přeskočíme prázdný řádek
                continue;
            }
            *dest++ = *src++; // Kopírujeme nový řádek, pokud není prázdný
        }
        // Kopírování ostatních znaků do bufferu
        else {
            *dest++ = *src++;
        }
    }

    // Kontrola posledního řádku
    if (dest > buffer && *(dest - 1) == '\n') {
        // Zkontrolujeme, zda poslední řádek neobsahuje pouze prázdné znaky
        const char* tmp = dest - 2;  // Zkontrolujeme před posledním '\n'
        bool isLastLineEmpty = true;
        while (tmp >= buffer && *tmp != '\n') {
            if (!isspace(*tmp)) {
                isLastLineEmpty = false;
                break;
            }
            tmp--;
        }
        // Pokud je poslední řádek prázdný, odstraníme ho
        if (isLastLineEmpty) {
            dest--;  // Přeskočíme poslední prázdný řádek
        }
    }

    // Ukončení stringu v bufferu
    *dest = '\0';

    // Zdrojový kód nyní neobsahuje komentáře, nepotřebné části ani prázdné řádky
    current = buffer;  // Nastavení ukazatele na začátek bufferu

    if (devTool) {
        printf("Processed source:\n%s\n", buffer);
    }

    return NO_ERROR;
}
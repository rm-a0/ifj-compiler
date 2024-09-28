#include "../inc/scanner.h"
#include "../inc/token.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    // Otevření zdrojového souboru pro čtení
    FILE *file = fopen("ScannerTests/test1.zig", "r");
    if (file == NULL) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    // Načtení obsahu souboru do řetězce
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *source = malloc(fsize + 1);
    fread(source, 1, fsize, file);
    fclose(file);
    source[fsize] = 0;

    // Inicializace scanneru se zdrojovým kódem
    initScanner(source);

    // Cyklus pro získání a výpis tokenů
    Token token;
    do {
        token = getNextToken();
        printf("Received token: %u with value: %s\n", token.token_type, token.value);
    } while (token.token_type != TOKEN_EOF);

    // Uvolnění alokované paměti a ukončení programu
    free(source);
    return EXIT_SUCCESS;
}

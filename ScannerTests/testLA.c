#include "../inc/scanner.h"
#include "../inc/token.h"
#include "../inc/error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {
    // Default devTool is set to false
    devTool = false;

    // Check command line arguments for -dev option
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-dev") == 0) {
            devTool = true;
            break;
        }
    }

    // Open source file for reading
    FILE *file = fopen("ScannerTests/test1.zig", "r");
    if (file == NULL) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    // Load file content into a string
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *source = malloc(fsize + 1);
    fread(source, 1, fsize, file);
    fclose(file);
    source[fsize] = '\0';

    // Initialize scanner with source code
    initScanner(source);
    return EXIT_SUCCESS;
//    // Loop to get and print tokens
//    Token token;
//    do {
//        token = getNextToken();
//        printf("Received token: %u with value: %s\n", token.token_type, token.value);
//        if (devTool) {
//            // If devTool is true, additional debugging information can be displayed here
//            printf("Debug: Token position start: %u end: %u\n", token.start, token.end);
//        }
//    } while (token.token_type != TOKEN_EOF);
//
//    // Free allocated memory and exit
//    free(source);
//    return EXIT_SUCCESS;
}

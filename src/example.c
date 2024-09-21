#include <stdio.h>
#include "example.h"

void function_name(int argument) {
    if (argument < 0) {
        function_name(argument - 1);
    }
    else {
        return;
    }
}

int main() {
    function_name(9);
    return 0;
}

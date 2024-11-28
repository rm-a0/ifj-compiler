/**
 * @file error.c
 * @brief File containing implementation of functions declared in error.h
 * @authors Michal Repcik (xrepcim00)
*/

#include "error.h"

ErrorType error_tracker = NO_ERROR;

void set_error(ErrorType error) {
    // set new error only if no error was encoutered before
    if (error_tracker == NO_ERROR) {
        error_tracker = error;
    }
}

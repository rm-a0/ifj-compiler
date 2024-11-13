/**
 * @file error.c
 * @brief Implementation of error handling functions.
 */

#include "../inc/error.h"

/**
 * @var ErrorType error_tracker
 * @brief Global variable to track the current error state.
 */
ErrorType error_tracker = NO_ERROR;

/**
 * @brief Sets the global error_tracker to the specified error.
 * 
 * Only sets the error if no previous error has been set.
 * This prevents overwriting a more severe error with a less severe one.
 * 
 * @param error The ErrorType to set.
 */
void set_error(ErrorType error) {
    if (error_tracker == NO_ERROR) {
        error_tracker = error;
    }
}

/**
 * @brief Retrieves the current error state.
 * 
 * @return The current ErrorType.
 */
ErrorType get_error(void) {
    return error_tracker;
}

/**
 * @brief Resets the error state to NO_ERROR.
 * 
 * @return void
 */
void reset_error(void) {
    error_tracker = NO_ERROR;
}

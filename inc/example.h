/**
 * @file example.h
 * @brief Header file for example.c
 * @authors Author Name (xlogin), Author Name (xlogin)
*/

#ifndef EXAMPLE_H
#define EXAMPLE_H

/**
 * @struct StructName
 * @brief What is struct used for
 * @note Write note if needed
 * 
 * @param content description of content
 * @param value description of value
 */
typedef struct {
    char* content;
    int value;
} StructName;

/**
 * @fn void function_name(int argument)
 * @brief What does the function do
 * @note Note
 * 
 * Detailed description of a function
 * 
 * @param[in] argument description of the parameter
 * @return void Function doesnt return value
*/
void function_name(int argument);

#endif // EXAMPLE_H

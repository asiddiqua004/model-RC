#pragma once

/**
 * Extract a group of characters between a character
 *
 * Assumption:
 * - output_string is memory pointer that is at least the size of strlen(source)
 *
 * Behavior:
 * - If 'between' char does not appear at least twice, return false
 * - Given *source="hello 'world' 123", and between=' then *output_string should be set to "world"
 *
 * Another example:
 * - *source = "value = 'one two three'"
 * - between = ' (single quote)
 * - *output_string should be set to "one two three"
 *
 * @param source Input string, such as 'hello .123. world'
 * @param output_string The output string is written here
 * @param between The character between which we should extract a string
 *
 * @returns true if a string has been extracted correctly
 */

/***********************************************************************************************************************
 *
 *                                                  I N C L U D E S
 *
 **********************************************************************************************************************/
/* Standard Includes */

#include <stdbool.h>
#include <stddef.h> // size_t

/* External Includes */

/* Module Includes */

/***********************************************************************************************************************
 *
 *                                                   D E F I N E S
 *
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *
 *                                                  T Y P E D E F S
 *
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *
 *                                     F U N C T I O N   D E C L A R A T I O N S
 *
 **********************************************************************************************************************/

bool extract_string_between(const char *source, char *output_string, char between);
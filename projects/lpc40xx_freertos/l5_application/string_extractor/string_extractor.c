/***********************************************************************************************************************
 *
 *                                                  I N C L U D E S
 *
 **********************************************************************************************************************/
/* Main Module Header */

#include "string_extractor.h"

/* Standard Includes */
#include <stdio.h>
#include <string.h>

/* External Includes */

/* Private Module Includes */

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
 *                             P R I V A T E   F U N C T I O N   D E C L A R A T I O N S
 *
 **********************************************************************************************************************/
static bool string_extractor__validate_inputs(const char *source, const char *output_string) {
  return source && output_string && strlen(source);
}
/***********************************************************************************************************************
 *
 *                                  P R I V A T E   D A T A   D E F I N I T I O N S
 *
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *
 *                                         P R I V A T E   F U N C T I O N S
 *
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *
 *                                          P U B L I C   F U N C T I O N S
 *
 **********************************************************************************************************************/
bool string_extractor__extract_string_between(const char *source, char *output_string, char between) {
  bool status = string_extractor__validate_inputs(source, output_string);
  char *first_char_occurance = NULL;
  char *second_char_occurance = NULL;
  size_t substring_length = 0U;
  if (status) {
    first_char_occurance = strchr(source, between);
    status = (NULL != first_char_occurance);
  }
  if (status) {
    second_char_occurance = strrchr(first_char_occurance + 1U, between);
    status = (NULL != second_char_occurance);
  }
  if (status) {
    substring_length = second_char_occurance - first_char_occurance;
    status = substring_length > 1U;
  }
  if (status) {
    (void)snprintf(output_string, substring_length, "%s", (first_char_occurance + 1U));
  }
  return status;
}
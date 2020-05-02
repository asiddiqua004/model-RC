/***********************************************************************************************************************
 *
 *                                                  I N C L U D E S
 *
 **********************************************************************************************************************/
/* Standard Includes */
#include "unity.h"

/* Mock Includes */

/* External Includes */

/* Module Includes */

#include "problem_2.h"

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
 *                                     T E S T   S E T U P   &   T E A R D O W N
 *
 **********************************************************************************************************************/
void setUp(void) {}

void tearDown(void) {}

/***********************************************************************************************************************
 *
 *                                                     T E S T S
 *
 **********************************************************************************************************************/

// Suggested tests (add more to these, but make existing ones pass first)
void test_extract_string_between__basic_negative_cases(void) {
  TEST_ASSERT_FALSE(extract_string_between(NULL, NULL, 'x'));
  TEST_ASSERT_FALSE(extract_string_between("", NULL, 'x'));
  TEST_ASSERT_FALSE(extract_string_between("", "", 'x'));
}
void test_extract_string_between__easy_case(void) {
  char output_string[32] = {};
  TEST_ASSERT_FALSE(extract_string_between("hello 'world' 123", output_string, '\''));
  TEST_ASSERT_EQUAL_STRING("world", output_string);
}
void test_extract_string_between__corner_cases(void) {
  // Test for case when 'between' char found zero times, or just 1 time
}
void test_extract_string_between__between_not_found(void) {}
void test_extract_string_between__nominal_cases(void) {}
void test_extract_string_between__bonus_points(void) {
  // Bonus points if you can handle an escaped string
  // If there are two characters, such as '' then that should not be considered the trigger
  char output_string[32] = {};
  TEST_ASSERT_FALSE(extract_string_between("hello 'world'' 123'", output_string, '\''));
  TEST_ASSERT_EQUAL_STRING("world' 123", output_string);
}
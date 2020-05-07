
#include "unity.h"

// Module includes
#include "line_buffer.c"

/******************************************************************************
 *
 *                               D E F I N E S
 *
 ******************************************************************************/

/*****************************************************************************
 *
 *                                 E N U M S
 *
 ******************************************************************************/

/******************************************************************************
 *
 *                              T Y P E D E F S
 *
 ******************************************************************************/

/******************************************************************************
 *
 *        P R I V A T E    F U N C T I O N    D E C L A R A T I O N S
 *
 ******************************************************************************/

/******************************************************************************
 *
 *             P R I V A T E    D A T A    D E F I N I T I O N S
 *
 *****************************************************************************/

static line_buffer_s line_buffer;
static char memory[20U];

/******************************************************************************
 *
 *                     P R I V A T E    F U N C T I O N S
 *
 *****************************************************************************/

/******************************************************************************
 *
 *                T E S T    S E T U P    &    T E A R D O W N
 *
 *****************************************************************************/
void setUp(void) { line_buffer__init(&line_buffer, memory, sizeof(memory)); }

void tearDown(void) {}

/******************************************************************************
 *
 *                                 T E S T S
 *
 *****************************************************************************/

void test_line_buffer__nominal_case(void) {
  line_buffer__add_byte(&line_buffer, 'a');
  line_buffer__add_byte(&line_buffer, 'b');
  line_buffer__add_byte(&line_buffer, 'c');
  line_buffer__add_byte(&line_buffer, '\n');

  char line[8];
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING("abc", line);
}

void test_line_buffer__slash_r_slash_n_case(void) {
  line_buffer__add_byte(&line_buffer, 'a');
  line_buffer__add_byte(&line_buffer, 'b');
  line_buffer__add_byte(&line_buffer, '\r');
  line_buffer__add_byte(&line_buffer, '\n');

  char line[8];
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  // TEST_ASSERT_EQUAL_STRING("ab\r", line);
  TEST_ASSERT_EQUAL_STRING("ab", line);
}

// Line buffer should be able to add multiple lines and we should be able to remove them one at a time
void test_line_buffer__multiple_lines(void) {
  line_buffer__add_byte(&line_buffer, 'a');
  line_buffer__add_byte(&line_buffer, 'b');
  line_buffer__add_byte(&line_buffer, '\n');
  line_buffer__add_byte(&line_buffer, 'c');
  line_buffer__add_byte(&line_buffer, 'd');
  line_buffer__add_byte(&line_buffer, '\n');

  char line[8U];
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING("ab", line);

  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING("cd", line);
}

void test_line_buffer__overflow_case(void) {
  // Add chars until full capacity
  for (size_t i = 0U; i < sizeof(memory) - 2U; i++) {
    TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'a' + i));
  }
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));

  // Buffer should be full now
  TEST_ASSERT_FALSE(line_buffer__add_byte(&line_buffer, 'b'));

  // Retreive truncated output (without the newline char)
  // Do not modify this test; instead, change your API to make this test pass
  char line[8U] = {0U};
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING("abcdefg", line);
}

void test_line_buffer__multiple_strings(void) {
  char line0[8U] = {0U};
  char line1[8U] = {0U};
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'a'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'b'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'c'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'd'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\r'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'e'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'f'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'g'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'h'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\r'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line0, sizeof(line0)));
  TEST_ASSERT_EQUAL_STRING("abcd", &line0);
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line1, sizeof(line1)));
  TEST_ASSERT_EQUAL_STRING("efgh", &line1);
}

void test_line_buffer__multiple_strings_add_remove_add(void) {
  char line0[8U] = {0U};
  char line1[8U] = {0U};
  char line2[8U] = {0U};
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'a'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'b'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'c'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'd'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\r'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'e'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'f'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'g'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'h'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\r'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line0, sizeof(line0)));
  TEST_ASSERT_EQUAL_STRING("abcd", &line0);
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'i'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'j'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'k'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'l'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\r'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line1, sizeof(line1)));
  TEST_ASSERT_EQUAL_STRING("efgh", &line1);
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line2, sizeof(line1)));
  TEST_ASSERT_EQUAL_STRING("ijkl", &line2);
}

void test_line_buffer__multiple_strings_add_remove_add_no_cr(void) {
  char line0[8U] = {0U};
  char line1[8U] = {0U};
  char line2[8U] = {0U};
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'a'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'b'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'c'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'd'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'e'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'f'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'g'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'h'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line0, sizeof(line0)));
  TEST_ASSERT_EQUAL_STRING("abcd", &line0);
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'i'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'j'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'k'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'l'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line1, sizeof(line1)));
  TEST_ASSERT_EQUAL_STRING("efgh", &line1);
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line2, sizeof(line1)));
  TEST_ASSERT_EQUAL_STRING("ijkl", &line2);
}
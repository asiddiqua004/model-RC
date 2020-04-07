#include "unity.h"

// Include the source we wish to test
#include "line_buffer.h"

static line_buffer_s line_buffer;
static char memory[8];

// This method re-initializes the line_buffer for the rest of the tests
void setUp(void) { line_buffer__init(&line_buffer, memory, sizeof(memory)); }

void tearDown(void) {}

void test_line_buffer__nominal_case(void) {
  line_buffer__add_byte(&line_buffer, 'a');
  line_buffer__add_byte(&line_buffer, 'b');
  line_buffer__add_byte(&line_buffer, 'c');
  line_buffer__add_byte(&line_buffer, '\n');

  char line[8];
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "abc");
}

void test_line_buffer__slash_r_slash_n_case(void) {
  line_buffer__add_byte(&line_buffer, 'a');
  line_buffer__add_byte(&line_buffer, 'b');
  line_buffer__add_byte(&line_buffer, '\r');
  line_buffer__add_byte(&line_buffer, '\n');
  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), 4);

  char line[8];
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "ab\r");
  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), 0);
}

// Line buffer should be able to add multiple lines and we should be able to remove them one at a time
void test_line_buffer__multiple_lines(void) {
  line_buffer__add_byte(&line_buffer, 'a');
  line_buffer__add_byte(&line_buffer, 'b');
  line_buffer__add_byte(&line_buffer, '\n');
  line_buffer__add_byte(&line_buffer, 'c');
  line_buffer__add_byte(&line_buffer, 'd');
  line_buffer__add_byte(&line_buffer, '\n');
  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), 6);

  char line[8];
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "ab");
  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), 3);

  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "cd");
  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), 0);
}

void test_line_buffer__overflow_case(void) {
  // Add chars until full capacity
  for (size_t i = 0; i < sizeof(memory); i++) {
    TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'a' + i));
  }
  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), sizeof(memory));

  // Buffer should be full now
  TEST_ASSERT_FALSE(line_buffer__add_byte(&line_buffer, 'b'));

  // Retreive truncated output (without the newline char)
  // Do not modify this test; instead, change your API to make this test pass
  // Note that line buffer was full with "abcdefgh" but we should only
  // retreive "abcdefg" because we need to write NULL char to line[8]
  char line[8] = {0};
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "abcdefg");

  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), 1);

  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "h");
  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), 0);
}

void test_line_buffer__multiple_lines_and_same_buffer_size(void) {
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'a'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'b'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'c'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'd'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));
  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), 6);

  char line[8];
  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "ab");
  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), 3);

  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'e'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'f'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'g'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'h'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));
  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), 8);

  // Try to add to a full buffer
  TEST_ASSERT_FALSE(line_buffer__add_byte(&line_buffer, 'x'));

  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "cd");
  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), 5);

  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "efgh");
  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), 0);

  // Try to remove line from empty line buffer
  TEST_ASSERT_FALSE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));

  // Add chars until full capacity
  for (size_t i = 0; i < sizeof(memory); i++) {
    TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'a' + i));
  }
  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), 8);

  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "abcdefg");
  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), 1);

  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "h");
  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), 0);
}

void test_line_buffer__multiple_lines_and_smaller_buffer(void) {
  char line[3];

  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'a'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'b'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'c'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'd'));
  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), 6);

  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "ab");
  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), 4);

  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "");
  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), 3);

  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "c");
  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), 1);

  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "d");
  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), 0);
}

void test_line_buffer__multiple_lines_and_larger_buffer(void) {
  char line[10];

  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'a'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'b'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'c'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, '\n'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'd'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'e'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'f'));
  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), 8);

  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "ab");
  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), 5);

  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "c");
  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), 3);

  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'g'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'h'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'i'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'j'));
  TEST_ASSERT_TRUE(line_buffer__add_byte(&line_buffer, 'k'));
  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), 8);

  TEST_ASSERT_TRUE(line_buffer__remove_line(&line_buffer, line, sizeof(line)));
  TEST_ASSERT_EQUAL_STRING(line, "defghijk");
  TEST_ASSERT_EQUAL(line_buffer__get_item_count(&line_buffer), 0);
}

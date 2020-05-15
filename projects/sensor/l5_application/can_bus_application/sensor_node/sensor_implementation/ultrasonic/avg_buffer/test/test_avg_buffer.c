#include "unity.h"

#include <stddef.h>
#include <stdint.h>

#include "avg_buffer.h"

static avg_buffer_s avg_buffer;
static const size_t buffer_capacity = 5;
static uint8_t buffer[5];

void setUp(void) { avg_buffer__initialize(&avg_buffer, buffer, buffer_capacity); }
void tearDown(void) {}

void test_avg_buffer__insert_and_get_value(void) {
  // zero case
  TEST_ASSERT_EQUAL_size_t(avg_buffer__get_value(&avg_buffer), 0);

  size_t value = 10;
  avg_buffer__insert_value(&avg_buffer, value);
  TEST_ASSERT_EQUAL_size_t(avg_buffer__get_value(&avg_buffer), 10 / avg_buffer__get_insertions(&avg_buffer));

  value += 10;
  avg_buffer__insert_value(&avg_buffer, value);
  TEST_ASSERT_EQUAL_size_t(avg_buffer__get_value(&avg_buffer), (10 + 20) / avg_buffer__get_insertions(&avg_buffer));

  value += 10;
  avg_buffer__insert_value(&avg_buffer, value);
  TEST_ASSERT_EQUAL_size_t(avg_buffer__get_value(&avg_buffer),
                           (10 + 20 + 30) / avg_buffer__get_insertions(&avg_buffer));

  value += 10;
  avg_buffer__insert_value(&avg_buffer, value);
  TEST_ASSERT_EQUAL_size_t(avg_buffer__get_value(&avg_buffer),
                           (10 + 20 + 30 + 40) / avg_buffer__get_insertions(&avg_buffer));

  value += 10;
  avg_buffer__insert_value(&avg_buffer, value);
  TEST_ASSERT_TRUE(avg_buffer.filled);
  TEST_ASSERT_EQUAL_size_t(avg_buffer__get_value(&avg_buffer),
                           (10 + 20 + 30 + 40 + 50) / avg_buffer__get_insertions(&avg_buffer));

  value = 0;
  avg_buffer__insert_value(&avg_buffer, value);
  TEST_ASSERT_EQUAL_size_t(avg_buffer__get_value(&avg_buffer),
                           (0 + 20 + 30 + 40 + 50) / (avg_buffer__get_insertions(&avg_buffer)));

  value = 50;
  avg_buffer__insert_value(&avg_buffer, value);
  TEST_ASSERT_EQUAL_size_t(avg_buffer__get_value(&avg_buffer),
                           (0 + 50 + 30 + 40 + 50) / avg_buffer__get_insertions(&avg_buffer));
}

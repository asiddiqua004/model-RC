#include "unity.h"

#include "MockFreeRTOS.h"
#include "Mockclock.h"
#include "Mockqueue.h"

#include "Mockgpio.h"
#include "Mockuart.h"

#include "Mockline_buffer.h"
#include "Mockuart_printf.h"

#include "wifi_implementation.c"

void setUp(void) {}
void tearDown(void) {}

void test_wifi_implementation__fill_line_buffer(void) {
  char byte = 0;

  uart__get_ExpectAndReturn(UART__3, &byte, 1, true);
  line_buffer__add_byte_ExpectAndReturn(&wifi_line_buffer, byte, true);
  uart__get_ExpectAndReturn(UART__3, &byte, 1, false);

  wifi_implementation__fill_line_buffer();
}

void test_wifi_implementation__handle_line(void) {
  line_buffer__get_item_count_ExpectAndReturn(&wifi_line_buffer, true);

  char line[32] = {0};
  line_buffer__remove_line_ExpectAndReturn(&wifi_line_buffer, line, sizeof(line), true);
  uart_printf_ExpectAndReturn(UART__0, "line found: %s\n", true);

  line_buffer__remove_line_ExpectAndReturn(&wifi_line_buffer, line, sizeof(line), false);

  wifi_implementation__handle_line();
}
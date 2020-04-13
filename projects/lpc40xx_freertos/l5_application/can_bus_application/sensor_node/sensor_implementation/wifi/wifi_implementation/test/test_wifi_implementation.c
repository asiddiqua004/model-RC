#include "unity.h"

#include "MockFreeRTOS.h"
#include "Mockclock.h"
#include "Mockqueue.h"

#include "Mockgpio.h"
#include "Mockuart.h"

#include "Mockline_buffer.h"
#include "Mockuart_printf.h"
#include "Mockwifi_message_handler.h"

#include "wifi_implementation.c"

void setUp(void) {}
void tearDown(void) {}

void test_wifi_implementation__initialize(void) {
  line_buffer__init_Expect(&wifi_line_buffer, NULL, 128);
  line_buffer__init_IgnoreArg_memory();

  gpio_s gpio = {0};
  gpio__construct_with_function_ExpectAndReturn(GPIO__PORT_4, 28, GPIO__FUNCTION_2, gpio);
  gpio__construct_with_function_ExpectAndReturn(GPIO__PORT_4, 29, GPIO__FUNCTION_2, gpio);

  clock__get_peripheral_clock_hz_ExpectAndReturn(0);
  uart__init_Expect(UART__3, 0, 74880);

  // Memory for the queue data structure
  StaticQueue_t rxq_struct;
  StaticQueue_t txq_struct;

  // Make UART more efficient by backing it with RTOS queues (optional but highly recommended with RTOS)
  xQueueCreateStatic_ExpectAndReturn(128, sizeof(char), NULL, &rxq_struct, NULL);
  xQueueCreateStatic_IgnoreArg_pucQueueStorageBuffer();
  xQueueCreateStatic_IgnoreArg_pxQueueBuffer();

  xQueueCreateStatic_ExpectAndReturn(32, sizeof(char), NULL, &txq_struct, NULL);
  xQueueCreateStatic_IgnoreArg_pucQueueStorageBuffer();
  xQueueCreateStatic_IgnoreArg_pxQueueBuffer();

  QueueHandle_t queue_handle = NULL;
  uart__enable_queues_ExpectAndReturn(UART__3, queue_handle, queue_handle, true);

  wifi_implementation__initialize();
}

void test_wifi_implementation__fill_line_buffer(void) {
  char byte = 0;

  uart__get_ExpectAndReturn(UART__3, &byte, 0, true);
  line_buffer__add_byte_ExpectAndReturn(&wifi_line_buffer, byte, true);
  uart__get_ExpectAndReturn(UART__3, &byte, 0, false);

  wifi_implementation__fill_line_buffer();
}

void test_wifi_implementation__handle_line(void) {
  line_buffer__get_item_count_ExpectAndReturn(&wifi_line_buffer, true);
  char line[32] = {0};

  wifi_message_handler__parse_line_Expect(line, 0);
  line_buffer__remove_line_ExpectAndReturn(&wifi_line_buffer, line, sizeof(line), true);

  line_buffer__remove_line_ExpectAndReturn(&wifi_line_buffer, line, sizeof(line), false);

  wifi_implementation__handle_line();
}

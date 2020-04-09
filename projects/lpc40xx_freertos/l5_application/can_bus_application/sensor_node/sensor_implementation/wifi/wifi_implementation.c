#include "wifi_implementation.h"

#include <stdio.h>

#include "gpio.h"
#include "uart.h"

#include "line_buffer.h"
#include "uart_printf.h"

static line_buffer_s wifi_line_buffer;

void wifi_implementation__initialize(void) {
  static uint8_t wifi_line_buffer_memory[128];
  line_buffer__init(&wifi_line_buffer, wifi_line_buffer_memory, sizeof(wifi_line_buffer_memory));

  gpio__construct_with_function(GPIO__PORT_4, 28, GPIO__FUNCTION_2); // P4.28 - UART-3 Tx
  gpio__construct_with_function(GPIO__PORT_4, 29, GPIO__FUNCTION_2); // P4.29 - UART-3 Rx

  // Note: PIN functions are initialized by board_io__initialize() for P4.28(Tx) and P4.29(Rx)
  uart__init(UART__3, clock__get_peripheral_clock_hz(), 74880);

  // You can use xQueueCreate() that uses malloc() as it is an easier API to work with, however, we opt to
  // use xQueueCreateStatic() to provide reference on how to create RTOS queue without dynamic memory allocation

  // Memory for the queue data structure
  static StaticQueue_t rxq_struct;
  static StaticQueue_t txq_struct;

  // Memory where the queue actually stores the data
  static uint8_t rxq_storage[32];
  static uint8_t txq_storage[128];

  // Make UART more efficient by backing it with RTOS queues (optional but highly recommended with RTOS)
  QueueHandle_t rxq_handle = xQueueCreateStatic(sizeof(rxq_storage), sizeof(char), rxq_storage, &rxq_struct);
  QueueHandle_t txq_handle = xQueueCreateStatic(sizeof(txq_storage), sizeof(char), txq_storage, &txq_struct);

  uart__enable_queues(UART__3, txq_handle, rxq_handle);
}

void wifi_implementation__polled_test_echo(void) {
  const char *test_buffer = "test";
  uart_puts(UART__3, test_buffer);

  char rcv_buffer[32] = {0};
  size_t rcv_count = 0;

  while (uart__get(UART__3, rcv_buffer + rcv_count, 100)) {
    rcv_count++;
  }

  if (rcv_count) {
    uart_printf(UART__0, "ECHO: %s", rcv_buffer);
  }
}

void wifi_implementation__fill_line_buffer(void) {
  char byte = 0;
  while (uart__get(UART__3, &byte, 0)) {
    line_buffer__add_byte(&wifi_line_buffer, byte);
  }
}

void wifi_implementation__handle_line(void) {
  if (line_buffer__get_item_count(&wifi_line_buffer)) {
    char line[32] = {0};
    while (line_buffer__remove_line(&wifi_line_buffer, line, sizeof(line))) {
      uart_printf(UART__0, "line found: %s\n", line);
    }
  }
}

#include "unity.h"

#include "Mockboard_io.h"
#include "Mockcan_bus.h"
#include "Mockgpio.h"

#include "can_bus_handler.c"

#include "driver.h"

#include "can_mia_configurations.c"

void test_can_handler__transmit_messages_10hz_success(void) {
  can__tx_ExpectAndReturn(can1, NULL, 0, true);
  can__tx_IgnoreArg_can_message_ptr();
  can_handler__transmit_messages_10hz();
}

void test_can_handler__transmit_messages_10hz_fail(void) {
  can__tx_ExpectAndReturn(can1, NULL, 0, false);
  can__tx_IgnoreArg_can_message_ptr();
  can_handler__transmit_messages_10hz();
}

void test_can_handler__manage_mia_10hz__below_mia_threshold(void) {
  sensor_data.mia_info.mia_counter = 0;
  can_handler__manage_mia_10hz();
}

void test_can_handler__manage_mia_10hz__above_mia_threshold(void) {
  sensor_data.mia_info.mia_counter = 200;
  gpio_s led = {0};
  board_io__get_led3_ExpectAndReturn(led);
  gpio__reset_Expect(led);
  can_handler__manage_mia_10hz();
}

void test_can_handler__handle_all_incoming_messages__unhandled_msg(void) {
  can__rx_ExpectAndReturn(can1, NULL, 0, false);
  can__rx_IgnoreArg_can_message_ptr();
  can_handler__handle_all_incoming_messages();
}

void test_can_handler__handle_all_incoming_messages__handled_msg(void) {
  gpio_s gpio = {};
  can__msg_t can_msg_rx = {0};

  can_msg_rx.msg_id = 103U;
  can_msg_rx.frame_fields.data_len = 5;
  can__rx_ExpectAndReturn(can1, NULL, 0, true);
  can__rx_IgnoreArg_can_message_ptr();
  can__rx_ReturnThruPtr_can_message_ptr(&can_msg_rx);
  can__rx_ExpectAndReturn(can1, NULL, 0, false);
  can__rx_IgnoreArg_can_message_ptr();
  can_handler__handle_all_incoming_messages();

  can_msg_rx.msg_id = 102U;
  can_msg_rx.frame_fields.data_len = 5;
  can__rx_ExpectAndReturn(can1, NULL, 0, true);
  can__rx_IgnoreArg_can_message_ptr();
  can__rx_ReturnThruPtr_can_message_ptr(&can_msg_rx);
  board_io__get_led3_ExpectAndReturn(gpio);
  gpio__set_Expect(gpio);
  can__rx_ExpectAndReturn(can1, NULL, 0, false);
  can__rx_IgnoreArg_can_message_ptr();
  can_handler__handle_all_incoming_messages();
}
#include "unity.h"

#include "Mockboard_io.h"
#include "Mockcan_bus.h"
#include "Mockgpio.h"

#include "can_bus_handler.c"

#include "Mockcan_bus_initializer.h"
#include "Mockdriver.h"
#include "Mockexternal_lcd.h"
#include "can_mia_configurations.c"

void test_can_handler__transmit_messages_10hz_success(void) {
  dbc_DRIVER_MOTOR_CONTROL_s motor_control = {0, {0}};
  can__tx_ExpectAndReturn(can1, NULL, 0, true);
  can__tx_IgnoreArg_can_message_ptr();
  driver__get_motor_commands_ExpectAndReturn(motor_control);
  can_handler__transmit_messages_10hz();
}

void test_can_handler__transmit_messages_10hz_fail(void) {
  dbc_DRIVER_MOTOR_CONTROL_s motor_control = {0, {0}};
  can__tx_ExpectAndReturn(can1, NULL, 0, false);
  can__tx_IgnoreArg_can_message_ptr();
  driver__get_motor_commands_ExpectAndReturn(motor_control);
  can_handler__transmit_messages_10hz();
}

void test_can_handler__transmit_debug_msg_10hz_success(void) {
  dbc_DRIVER_DEBUG_s driver_dbg_msg = {0};
  can__tx_ExpectAndReturn(can1, NULL, 0, true);
  can__tx_IgnoreArg_can_message_ptr();
  can_bus_initializer__get_can_bus_init_status_ExpectAndReturn(true);
  can_bus_initializer__get_can_bus_off_ExpectAndReturn(true);
  driver__get_node_heart_beats_ExpectAndReturn(true);
  can_handler__transmit_debug_msg_10hz();
}

void test_can_handler__transmit_debug_msg_10hz_fail(void) {
  dbc_DRIVER_DEBUG_s driver_dbg_msg = {0};
  can__tx_ExpectAndReturn(can1, NULL, 0, false);
  can__tx_IgnoreArg_can_message_ptr();
  can_bus_initializer__get_can_bus_init_status_ExpectAndReturn(true);
  can_bus_initializer__get_can_bus_off_ExpectAndReturn(true);
  driver__get_node_heart_beats_ExpectAndReturn(true);
  can_handler__transmit_debug_msg_10hz();
}

void test_can_handler__manage_mia_10hz__below_mia_threshold(void) {
  sensor_data.mia_info.mia_counter = 0;
  can_handler__manage_mia_10hz();
}

void reset_counter(void) {
  navigation_state.mia_info.mia_counter = 0;
  battery_voltage.mia_info.mia_counter = 0;
  geo_gps_data.mia_info.mia_counter = 0;
  sensor_data.mia_info.mia_counter = 0;
  motor_hbt.mia_info.mia_counter = 0;
  bridge_sensor_hbt.mia_info.mia_counter = 0;
  geo_hbt.mia_info.mia_counter = 0;
  motor_actual_speed.mia_info.mia_counter = 0;
}

void test_can_handler__manage_mia_10hz__above_mia_threshold(void) {
  gpio_s led = {0};

  sensor_data.mia_info.mia_counter = 200;
  board_io__get_led0_ExpectAndReturn(led);
  gpio__reset_Expect(led);
  can_handler__manage_mia_10hz();
  reset_counter();

  geo_gps_data.mia_info.mia_counter = 200;
  board_io__get_led1_ExpectAndReturn(led);
  gpio__reset_Expect(led);
  can_handler__manage_mia_10hz();
  reset_counter();

  motor_actual_speed.mia_info.mia_counter = 200;
  board_io__get_led2_ExpectAndReturn(led);
  gpio__reset_Expect(led);
  can_handler__manage_mia_10hz();
  reset_counter();

  navigation_state.mia_info.mia_counter = 200;
  board_io__get_led3_ExpectAndReturn(led);
  gpio__reset_Expect(led);
  can_handler__manage_mia_10hz();
  reset_counter();

  //   motor_hbt.mia_info.mia_counter = 200;
  //   board_io__get_led0_ExpectAndReturn(led);
  //   gpio__reset_Expect(led);
  //   can_handler__manage_mia_10hz();
  //   reset_counter();

  //   bridge_sensor_hbt.mia_info.mia_counter = 200;
  //   board_io__get_led1_ExpectAndReturn(led);
  //   gpio__reset_Expect(led);
  //   can_handler__manage_mia_10hz();
  //   reset_counter();

  //   geo_hbt.mia_info.mia_counter = 200;
  //   board_io__get_led2_ExpectAndReturn(led);
  //   gpio__reset_Expect(led);
  //   can_handler__manage_mia_10hz();
  //   reset_counter();
}

void test_can_handler__handle_all_incoming_messages__unhandled_msg(void) {
  can__rx_ExpectAndReturn(can1, NULL, 0, false);
  can__rx_IgnoreArg_can_message_ptr();
  can_handler__handle_all_incoming_messages_10Hz();
}

void test_can_handler__handle_all_incoming_messages__handled_msg(void) {
  gpio_s gpio = {};
  gpio_s led = {0};
  can__msg_t can_msg_rx = {0};

  can_msg_rx.msg_id = 417U;
  can_msg_rx.frame_fields.data_len = 5;
  can__rx_ExpectAndReturn(can1, NULL, 0, true);
  can__rx_IgnoreArg_can_message_ptr();
  can__rx_ReturnThruPtr_can_message_ptr(&can_msg_rx);
  can__rx_ExpectAndReturn(can1, NULL, 0, false);
  can__rx_IgnoreArg_can_message_ptr();
  can_handler__handle_all_incoming_messages_10Hz();
  reset_counter();

  can_msg_rx.msg_id = 104U;
  can_msg_rx.frame_fields.data_len = 4;
  can__rx_ExpectAndReturn(can1, NULL, 0, true);
  can__rx_IgnoreArg_can_message_ptr();
  can__rx_ReturnThruPtr_can_message_ptr(&can_msg_rx);
  can__rx_ExpectAndReturn(can1, NULL, 0, false);
  can__rx_IgnoreArg_can_message_ptr();
  board_io__get_led0_ExpectAndReturn(led);
  gpio__set_Expect(led);
  can_handler__handle_all_incoming_messages_10Hz();
  reset_counter();

  can_msg_rx.msg_id = 105U;
  can_msg_rx.frame_fields.data_len = 8;
  can__rx_ExpectAndReturn(can1, NULL, 0, true);
  can__rx_IgnoreArg_can_message_ptr();
  can__rx_ReturnThruPtr_can_message_ptr(&can_msg_rx);
  can__rx_ExpectAndReturn(can1, NULL, 0, false);
  can__rx_IgnoreArg_can_message_ptr();
  board_io__get_led1_ExpectAndReturn(led);
  gpio__set_Expect(led);
  can_handler__handle_all_incoming_messages_10Hz();
  reset_counter();
}

// void test_populate_signal_status_10Hz() { populate_signal_status_10Hz(); }

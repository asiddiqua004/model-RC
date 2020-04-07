#include "unity.h"

#include "Mockboard_io.h"
#include "Mockcan_bus.h"
#include "Mockgpio.h"
#include "Mockultrasonic_implementation.h"
#include "Mockwifi_implementation.h"

#include "tesla_model_rc.h"

#include "can_bus_mia_configurations.h"

#include "sensor_node.c"

void setUp(void) {
  sensor_node__is_sync = false;
  memset(&can_msg__driver_heartbeat, 0, sizeof(can_msg__driver_heartbeat));
}

void tearDown(void) {}

void test_sensor_node__init(void) {
  wifi_implementation__initialize_Expect();
  ultrasonic_implementation__initialize_Expect();

  sensor_node__init();
}

void test_sensor_node__send_messages_over_can_succesfully(void) {
  sensor_node__is_sync = true;

  // gps headings message
  can__tx_ExpectAndReturn(can1, NULL, 0, true);
  can__tx_IgnoreArg_can_message_ptr();

  // vehicale navigation state message
  can__tx_ExpectAndReturn(can1, NULL, 0, true);
  can__tx_IgnoreArg_can_message_ptr();

  // voltage message
  can__tx_ExpectAndReturn(can1, NULL, 0, true);
  can__tx_IgnoreArg_can_message_ptr();

  // ultrasonics message
  ultrasonic_implementation__get_left_ultrasonic_distance_in_ExpectAndReturn(0);
  ultrasonic_implementation__get_right_ultrasonic_distance_in_ExpectAndReturn(0);
  ultrasonic_implementation__get_front_ultrasonic_distance_in_ExpectAndReturn(0);
  ultrasonic_implementation__get_back_ultrasonic_distance_in_ExpectAndReturn(0);
  can__tx_ExpectAndReturn(can1, NULL, 0, true);
  can__tx_IgnoreArg_can_message_ptr();

  TEST_ASSERT_TRUE(sensor_node__send_messages_over_can());
}

void test_sensor_node__send_messages_over_can_fail(void) {
  sensor_node__is_sync = true;

  // gps headings message
  can__tx_ExpectAndReturn(can1, NULL, 0, false);
  can__tx_IgnoreArg_can_message_ptr();

  // vehicale navigation state message
  can__tx_ExpectAndReturn(can1, NULL, 0, false);
  can__tx_IgnoreArg_can_message_ptr();

  // voltage message
  can__tx_ExpectAndReturn(can1, NULL, 0, false);
  can__tx_IgnoreArg_can_message_ptr();

  // ultrasonics message
  ultrasonic_implementation__get_left_ultrasonic_distance_in_ExpectAndReturn(0);
  ultrasonic_implementation__get_right_ultrasonic_distance_in_ExpectAndReturn(0);
  ultrasonic_implementation__get_front_ultrasonic_distance_in_ExpectAndReturn(0);
  ultrasonic_implementation__get_back_ultrasonic_distance_in_ExpectAndReturn(0);
  can__tx_ExpectAndReturn(can1, NULL, 0, false);
  can__tx_IgnoreArg_can_message_ptr();

  TEST_ASSERT_FALSE(sensor_node__send_messages_over_can());
}

void test_sensor_node__send_messages_over_can_sync_fail(void) {
  TEST_ASSERT_FALSE(sensor_node__send_messages_over_can());
}

void test_sensor_node__handle_mia(void) {
  gpio_s gpio = {0};

  // First 10 Hz cycle is pass by
  sensor_node__handle_mia();

  board_io__get_led0_ExpectAndReturn(gpio);
  gpio__set_Expect(gpio);

  // Second 10 Hz cycle triggers prints and gpio toggle
  sensor_node__handle_mia();
}

void test_sensor_node__handle_multiple_driver_heartbeat_messages_over_can(void) {
  gpio_s gpio = {0};
  can__msg_t msg = {0};
  msg.msg_id = dbc_header_DRIVER_HEARTBEAT.message_id;
  msg.frame_fields.data_len = dbc_header_DRIVER_HEARTBEAT.message_dlc;

  // Heartbeat found
  can__rx_ExpectAndReturn(can1, NULL, 0, true);
  can__rx_IgnoreArg_can_message_ptr();
  can__rx_ReturnThruPtr_can_message_ptr(&msg);

  board_io__get_led0_ExpectAndReturn(gpio);
  gpio__reset_Expect(gpio);

  // Heartbeat found
  can__rx_ExpectAndReturn(can1, NULL, 0, true);
  can__rx_IgnoreArg_can_message_ptr();
  can__rx_ReturnThruPtr_can_message_ptr(&msg);

  // other message found
  can__msg_t msg2 = {0};
  can__rx_ExpectAndReturn(can1, NULL, 0, true);
  can__rx_IgnoreArg_can_message_ptr();
  can__rx_ReturnThruPtr_can_message_ptr(&msg2);

  can__rx_ExpectAndReturn(can1, NULL, 0, false);
  can__rx_IgnoreArg_can_message_ptr();

  sensor_node__handle_messages_over_can();
}

void test_sensor_node__collect_data(void) {
  ultrasonic_implementation__initiate_ultrasonics_range_Expect();
  sensor_node__collect_data();
}
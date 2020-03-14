
#include "unity.h"

// Mocks
#include "Mockboard_io.h"
#include "Mockcan_bus.h"
#include "Mockgpio.h"

// Module includes
#include "tesla_model_rc.h"

#include "can_bus_message_handler.c"

#include "can_bus_mia_configurations.h"

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
void setUp(void) {}

void tearDown(void) {}

/******************************************************************************
 *
 *                                 T E S T S
 *
 *****************************************************************************/

void test_can_bus_message_handler__private_populate_X_message(void) {
#if NODE_ACCELEROMETER == 1

  dbc_ACCELEROMETER_VALUE_s accelerometer_message_to_be_populated = {0U};
  const dbc_ACCELEROMETER_VALUE_s accelerometer_message_expected = {
      .ACCELEROMETER_VALUE_x_axis = 5U, .ACCELEROMETER_VALUE_y_axis = 6U, .ACCELEROMETER_VALUE_z_axis = 7U};

  const acceleration__axis_data_s acceleration_data = {.x = 5U, .y = 6U, .z = 7U};
  acceleration__get_data_ExpectAndReturn(acceleration_data);
  acceleration__get_data_ExpectAndReturn(acceleration_data);
  acceleration__get_data_ExpectAndReturn(acceleration_data);

  can_bus_message_handler__private_populate_ACCELEROMETER_VALUE_message(&accelerometer_message_to_be_populated);
  TEST_ASSERT_EQUAL_MEMORY(&accelerometer_message_expected, &accelerometer_message_to_be_populated,
                           sizeof(dbc_ACCELEROMETER_VALUE_s));
#else

  dbc_TEMPERATURE_SENSOR_VALUE_s temperature_value_message_to_be_populated = {0U};
  const dbc_TEMPERATURE_SENSOR_VALUE_s temperature_value_message_expected = {.TEMPERATURE_SENSOR_VALUE_in_celsius =
                                                                                 10U};
  const float temperature_data = 10U;
  temperature__get_data_ExpectAndReturn(temperature_data);

  can_bus_message_handler__private_populate_TEMPERATURE_SENSOR_VALUE_message(
      &temperature_value_message_to_be_populated);
  TEST_ASSERT_EQUAL_MEMORY(&temperature_value_message_expected, &temperature_value_message_to_be_populated,
                           sizeof(dbc_TEMPERATURE_SENSOR_VALUE_s));

#endif
}

void test_can_bus_message_handler__send_test(void) {
  can__tx_ExpectAndReturn(can_bus_num, NULL, UINT32_MAX, true);
  can__tx_IgnoreArg_can_message_ptr();
  const bool send_test_status_success = can_bus_message_handler__send_test();
  TEST_ASSERT_TRUE(send_test_status_success);

  can__tx_ExpectAndReturn(can_bus_num, NULL, UINT32_MAX, false);
  can__tx_IgnoreArg_can_message_ptr();
  const bool send_test_status_fail = can_bus_message_handler__send_test();
  TEST_ASSERT_FALSE(send_test_status_fail);
}

void test_can_bus_message_handler__manage_mia_10Hz(void) {
  gpio_s gpio = {0U};
#if NODE_ACCELEROMETER == 1
  can_msg_temperature_sensor_value.mia_info.mia_counter = 0U;
  can_bus_message_handler__manage_mia_10Hz();
  can_msg_temperature_sensor_value.mia_info.mia_counter = 200U;
  board_io__get_led0_ExpectAndReturn(gpio);
  gpio__reset_Expect(gpio);
  can_bus_message_handler__manage_mia_10Hz();
#else
  can_msg_accelerometer_value.mia_info.mia_counter = 0U;
  can_bus_message_handler__manage_mia_10Hz();
  can_msg_accelerometer_value.mia_info.mia_counter = 200U;
  board_io__get_led0_ExpectAndReturn(gpio);
  gpio__reset_Expect(gpio);
  can_bus_message_handler__manage_mia_10Hz();
#endif
}

void test_can_bus_message_handler__transmit_messages_10Hz(void) {
  can__msg_t test_outgoing_message = {0U};
#if NODE_ACCELEROMETER == 1
  const acceleration__axis_data_s acceleration_data = {.x = 5U, .y = 6U, .z = 7U};
  acceleration__get_data_ExpectAndReturn(acceleration_data);
  acceleration__get_data_ExpectAndReturn(acceleration_data);
  acceleration__get_data_ExpectAndReturn(acceleration_data);
#else
  const float temperature_data = 10U;
  temperature__get_data_ExpectAndReturn(temperature_data);
#endif
  can__tx_ExpectAndReturn(can_bus_num, NULL, 0, true);
  can__tx_IgnoreArg_can_message_ptr();
  can_bus_message_handler__transmit_messages_10Hz();
}

void tes_can_bus_message_handler__handle_all_incoming_messages(void) {
  const gpio_s gpio = {0U};
  const can__msg_t test_message = {0U};
  can__msg_t temperature_sensor_message = {
      .frame_fields = {.data_len = 8U, .is_rtr = 0U, .is_29bit = 0U}, .msg_id = 101U, .data = {.words[0] = 5U}};
  can__msg_t accelerometer_message = {.frame_fields = {.data_len = 6U, .is_rtr = 0U, .is_29bit = 0U},
                                      .msg_id = 100U,
                                      .data = {.words[0] = 5U, .words[1] = 6U, .words[2] = 7U}};

  can__rx_ExpectAndReturn(can_bus_num, NULL, 0U, false);
  can__rx_IgnoreArg_can_message_ptr();
  can_bus_message_handler__handle_all_incoming_messages();
  board_io__get_led0_ExpectAndReturn(gpio);
  gpio__set_Expect(gpio);
#if NODE_ACCELEROMETER == 1
  const dbc_TEMPERATURE_SENSOR_VALUE_s temperature_sensor_value_expected = {
      .TEMPERATURE_SENSOR_VALUE_in_celsius = 5U,
  };
  can__rx_ExpectAndReturn(can_bus_num, NULL, 0U, true);
  can__rx_IgnoreArg_can_message_ptr();
  can__rx_ReturnThruPtr_can_message_ptr(&temperature_sensor_message);
  TEST_ASSERT_EQUAL_MEMORY(&temperature_sensor_value_expected, &can_msg_temperature_sensor_value,
                           sizeof(can_msg_temperature_sensor_value));
  can_bus_message_handler__handle_all_incoming_messages();
#else
  const dbc_ACCELEROMETER_VALUE_s accelerometer_value_expected = {
      .ACCELEROMETER_VALUE_x_axis = 5U, .ACCELEROMETER_VALUE_y_axis = 6U, .ACCELEROMETER_VALUE_z_axis = 7U};

  can__rx_ExpectAndReturn(can_bus_num, NULL, 0U, true);
  can__rx_IgnoreArg_can_message_ptr();
  can__rx_ReturnThruPtr_can_message_ptr(&accelerometer_message);
  TEST_ASSERT_EQUAL_MEMORY(&accelerometer_value_expected, &can_msg_accelerometer_value,
                           sizeof(can_msg_accelerometer_value));
  can_bus_message_handler__handle_all_incoming_messages();
#endif
}
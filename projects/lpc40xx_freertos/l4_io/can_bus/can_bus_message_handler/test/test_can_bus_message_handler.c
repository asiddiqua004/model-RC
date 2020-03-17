
#include "unity.h"

// Mocks
#include "Mockboard_io.h"
#include "Mockcan_bus.h"
#include "Mockfake_driver.h"
#include "Mockgpio.h"
#include "Mockmotor_control.h"

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
  can_msg_driver_motor_control.mia_info.mia_counter = 0U;
  can_bus_message_handler__manage_mia_10Hz();
  can_msg_driver_motor_control.mia_info.mia_counter = 200U;
  board_io__get_led0_ExpectAndReturn(gpio);
  gpio__reset_Expect(gpio);
  motor_control__update_speed_and_steering_Expect(&can_msg_driver_motor_control);
  can_bus_message_handler__manage_mia_10Hz();
}

void tes_can_bus_message_handler__handle_all_incoming_messages_10Hz(void) {
  const gpio_s gpio = {0U};
  can__msg_t driver_control_message_invalid = {.frame_fields = {.data_len = 6U, .is_rtr = 0U, .is_29bit = 0U},
                                               .msg_id = 100U,
                                               .data = {.words[1] = 6U, .words[2] = 7U}};

  can__rx_ExpectAndReturn(can_bus_num, NULL, 0U, false);
  can__rx_IgnoreArg_can_message_ptr();
  can_bus_message_handler__handle_all_incoming_messages_10Hz();
  board_io__get_led0_ExpectAndReturn(gpio);
  gpio__set_Expect(gpio);
  motor_control__update_speed_and_steering_Expect(&can_msg_driver_motor_control);
  const dbc_DRIVER_MOTOR_CONTROL_s driver_motor_control_expected = {
      .DRIVER_MOTOR_CONTROL_SPEED_KPH = 6U,
      .DRIVER_MOTOR_CONTROL_STEER = 7U,
  };
  can__rx_ExpectAndReturn(can_bus_num, NULL, 0U, true);
  can__rx_IgnoreArg_can_message_ptr();
  can__rx_ReturnThruPtr_can_message_ptr(&driver_control_message_invalid);
  TEST_ASSERT_EQUAL_MEMORY(&driver_motor_control_expected, &can_msg_driver_motor_control,
                           sizeof(can_msg_driver_motor_control));
  can_bus_message_handler__handle_all_incoming_messages_10Hz();

  can__msg_t driver_control_message_valid = {.frame_fields = {.data_len = 6U, .is_rtr = 0U, .is_29bit = 0U},
                                             .msg_id = 102U,
                                             .data = {.words[1] = 6U, .words[2] = 7U}};

  board_io__get_led0_ExpectAndReturn(gpio);
  gpio__set_Expect(gpio);
  motor_control__update_speed_and_steering_Expect(&can_msg_driver_motor_control);
  can__rx_ExpectAndReturn(can_bus_num, NULL, 0U, true);
  can__rx_IgnoreArg_can_message_ptr();
  can__rx_ReturnThruPtr_can_message_ptr(&driver_control_message_valid);
  TEST_ASSERT_EQUAL_MEMORY(&driver_motor_control_expected, &can_msg_driver_motor_control,
                           sizeof(can_msg_driver_motor_control));
  can_bus_message_handler__handle_all_incoming_messages_10Hz();
}
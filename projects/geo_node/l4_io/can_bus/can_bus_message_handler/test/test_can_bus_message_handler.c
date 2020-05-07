#include "unity.h"

// Mocks
#include "Mockboard_io.h"
#include "Mockcan_bus.h"
#include "Mockgeological.h"
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
void setUp(void) { memset(&can_msg_bridge_gps_headings, 0U, sizeof(can_msg_bridge_gps_headings)); }

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
  can_msg_bridge_gps_headings.mia_info.mia_counter = 0U;
  can_bus_message_handler__manage_mia_10Hz();
  can_msg_bridge_gps_headings.mia_info.mia_counter = 200U;
  geological__update_destination_coordinates_Expect(&can_msg_bridge_gps_headings);
  board_io__get_led0_ExpectAndReturn(gpio);
  gpio__reset_Expect(gpio);
  can_bus_message_handler__manage_mia_10Hz();
}

void test_can_bus_message_handler__handle_all_incoming_messages_bridge_10Hz(void) {
  const gpio_s gpio = {0U};
  can__msg_t bridge_gps_message_invalid = {.frame_fields = {.data_len = 8U, .is_rtr = 0U, .is_29bit = 0U},
                                           .msg_id = 100U,
                                           .data = {.dwords[0] = 6.0f, .dwords[1] = 7.0f}};

  can__rx_ExpectAndReturn(can_bus_num, NULL, 0U, false);
  can__rx_IgnoreArg_can_message_ptr();
  can_bus_message_handler__handle_all_incoming_messages_10Hz();
  const dbc_BRIDGE_SENSOR_GPS_HEADINGS_s bridge_gps_headings_expected = {0U};
  can__rx_ExpectAndReturn(can_bus_num, NULL, 0U, true);
  can__rx_IgnoreArg_can_message_ptr();
  can__rx_ReturnThruPtr_can_message_ptr(&bridge_gps_message_invalid);
  TEST_ASSERT_EQUAL_MEMORY(&bridge_gps_headings_expected, &can_msg_bridge_gps_headings,
                           sizeof(can_msg_bridge_gps_headings));
  can__rx_ExpectAndReturn(can_bus_num, NULL, 0U, false);
  can__rx_IgnoreArg_can_message_ptr();
  can_bus_message_handler__handle_all_incoming_messages_10Hz();

  can__msg_t bridge_gps_message_valid = {.frame_fields = {.data_len = 8U, .is_rtr = 0U, .is_29bit = 0U},
                                         .msg_id = 106U,
                                         .data = {.dwords[0] = 6.0f, .dwords[1] = 7.0f}};

  geological__update_destination_coordinates_Expect(&can_msg_bridge_gps_headings);
  board_io__get_led0_ExpectAndReturn(gpio);
  gpio__set_Expect(gpio);
  can__rx_ExpectAndReturn(can_bus_num, NULL, 0U, true);
  can__rx_IgnoreArg_can_message_ptr();
  can__rx_ReturnThruPtr_can_message_ptr(&bridge_gps_message_valid);
  TEST_ASSERT_EQUAL_MEMORY(&bridge_gps_headings_expected, &can_msg_bridge_gps_headings,
                           sizeof(can_msg_bridge_gps_headings));
  can__rx_ExpectAndReturn(can_bus_num, NULL, 0U, false);
  can__rx_IgnoreArg_can_message_ptr();
  can_bus_message_handler__handle_all_incoming_messages_10Hz();
}
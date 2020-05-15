#include <inttypes.h>
#include <stdio.h>

#include "can_bus_message_handler.h"

#include "board_io.h"
#include "geological.h"
#include "gpio.h"
#include "tesla_model_rc.h"

/*******************************************************************************
 *
 *               P R I V A T E    D A T A    D E F I N I T I O N S
 *
 ******************************************************************************/

static dbc_BRIDGE_SENSOR_GPS_HEADINGS_s can_msg_bridge_gps_headings;

/*******************************************************************************
 *
 *                     P R I V A T E    F U N C T I O N S
 *
 ******************************************************************************/

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/

bool can_bus_message_handler__send_test(void) {
  can__msg_t message = {0U};
  message.msg_id = (uint32_t)CAN_BUS_MESSAGE_HANDLER__MESSAGE_TEST;
  message.frame_fields.is_29bit = 0U;
  message.frame_fields.data_len = 8U;
  message.data.qword = 0x1122334455667788;
  return can__tx(can_bus_num, &message, UINT32_MAX);
}

void can_bus_message_handler__manage_mia_10Hz(void) {
  const uint32_t mia_increment_value = 100U;
  if (dbc_service_mia_BRIDGE_SENSOR_GPS_HEADINGS(&can_msg_bridge_gps_headings, mia_increment_value)) {
    printf("Bridge Sensor Node MIA\n");
    gpio__reset(board_io__get_led0());
    geological__update_destination_coordinates(&can_msg_bridge_gps_headings);
  }
}

void can_bus_message_handler__transmit_messages_10Hz(void) {}

void can_bus_message_handler__handle_all_incoming_messages_10Hz(void) {

  can__msg_t incoming_message;
  while (can__rx(can1, &incoming_message, 0U)) {
    const dbc_message_header_t header = {
        .message_id = incoming_message.msg_id,
        .message_dlc = incoming_message.frame_fields.data_len,
    };

    if (dbc_decode_BRIDGE_SENSOR_GPS_HEADINGS(&can_msg_bridge_gps_headings, header, incoming_message.data.bytes)) {
      geological__update_destination_coordinates(&can_msg_bridge_gps_headings);
      gpio__set(board_io__get_led0());
    }
  }
}

bool dbc_send_can_message(void *argument_from_dbc_encode_and_send, uint32_t message_id, const uint8_t bytes[8],
                          uint8_t dlc) {
  (void)argument_from_dbc_encode_and_send;

  can__msg_t can_msg = {0};
  can_msg.msg_id = message_id;
  can_msg.frame_fields.data_len = dlc;

  memcpy(can_msg.data.bytes, bytes, dlc);
  return can__tx(can1, &can_msg, 0);
}
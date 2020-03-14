
#include <inttypes.h>
#include <stdio.h>

#include "can_bus_message_handler.h"

#include "board_io.h"
#include "gpio.h"
#include "tesla_model_rc.h"

/*******************************************************************************
 *
 *               P R I V A T E    D A T A    D E F I N I T I O N S
 *
 ******************************************************************************/

static dbc_ACCELEROMETER_VALUE_s can_msg_accelerometer_value = {0U};

/*******************************************************************************
 *
 *                     P R I V A T E    F U N C T I O N S
 *
 ******************************************************************************/

static void
can_bus_message_handler__private_populate_ACCELEROMETER_VALUE_message(dbc_ACCELEROMETER_VALUE_s *accel_msg) {
  accel_msg->ACCELEROMETER_VALUE_x_axis = acceleration__get_data().x;
  accel_msg->ACCELEROMETER_VALUE_y_axis = acceleration__get_data().y;
  accel_msg->ACCELEROMETER_VALUE_z_axis = acceleration__get_data().z;
}

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
  if (dbc_service_mia_TEMPERATURE_SENSOR_VALUE(&can_msg_temperature_sensor_value, mia_increment_value)) {
    printf("Temperature Sensor Node MIA\n");
    // light up LED when message has gone MIA
    gpio__reset(board_io__get_led0());
  }
}

void can_bus_message_handler__handle_all_incoming_messages(void) {
  can__msg_t incoming_message;
  while (can__rx(can1, &incoming_message, 0U)) {
    const dbc_message_header_t header = {
        .message_id = incoming_message.msg_id,
        .message_dlc = incoming_message.frame_fields.data_len,
    };
    gpio__set(board_io__get_led0());

    dbc_decode_ACCELEROMETER_VALUE(&can_msg_accelerometer_value, header, incoming_message.data.bytes);
    printf("Received: x: %" PRIu16 ", y: %" PRIu16 ", z: %" PRIu16 "\n",
           can_msg_accelerometer_value.ACCELEROMETER_VALUE_x_axis,
           can_msg_accelerometer_value.ACCELEROMETER_VALUE_y_axis,
           can_msg_accelerometer_value.ACCELEROMETER_VALUE_z_axis);
  }
}
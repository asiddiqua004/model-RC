#include "board_io.h"
#include "can_bus.h"
#include "gpio.h"

#include "tesla_model_rc.h"

#include "driver.h"

#include "can_bus_handler.h"

static dbc_SENSOR_SONARS_s sensor_data = {0};
static dbc_GEO_GPS_HEADINGS_s geo_gps_data = {0};

void can_handler__handle_all_incoming_messages_10Hz(void) {
  can__msg_t can_msg = {0};
  while (can__rx(can1, &can_msg, 0)) {
    const dbc_message_header_t header = {.message_id = can_msg.msg_id, .message_dlc = can_msg.frame_fields.data_len};

    if (dbc_decode_SENSOR_SONARS(&sensor_data, header, can_msg.data.bytes)) {
      gpio__set(board_io__get_led2());
    }
    if (dbc_decode_GEO_GPS_HEADINGS(&geo_gps_data, header, can_msg.data.bytes)) {
      gpio__set(board_io__get_led3());
    }
  }
  can_handler__manage_mia_10hz();
}

void can_handler__manage_mia_10hz(void) {
  const uint32_t mia_increment_value = 100;
  if (dbc_service_mia_SENSOR_SONARS(&sensor_data, mia_increment_value)) {
    gpio__reset(board_io__get_led2());
  }
  if (dbc_service_mia_SENSOR_SONARS(&geo_gps_data, mia_increment_value)) {
    gpio__reset(board_io__get_led3());
  }
}

void populate_sensor_status_10Hz() {
  driver__process_sensor_input(sensor_data);
  driver__process_geo_controller_directions(geo_gps_data);
}

void can_handler__transmit_messages_10hz(void) {
  dbc_DRIVER_MOTOR_CONTROL_s motor_control = driver__get_motor_commands();
  dbc_encode_and_send_DRIVER_MOTOR_CONTROL(NULL, &motor_control);
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

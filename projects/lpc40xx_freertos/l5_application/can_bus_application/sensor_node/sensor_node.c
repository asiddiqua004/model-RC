#include "sensor_node.h"

#include <stdio.h>

#include "board_io.h"
#include "can_bus.h"
#include "gpio.h"

#include "tesla_model_rc.h"
#include "ultrasonic_implementation.h"

static bool sensor_node__is_sync = false;
static dbc_DRIVER_HEARTBEAT_s can_msg__driver_heartbeat = {0};

void sensor_node__init(void) { ultrasonic_implementation__initialize(); }

static bool sensor_node__construct_and_send_ultrasonic_data(void) {
  dbc_SENSOR_SONARS_s sensor_sonar_struct = {0};

  sensor_sonar_struct.SENSOR_SONARS_LEFT = ultrasonic_implementation__get_left_ultrasonic_distance_in();
  sensor_sonar_struct.SENSOR_SONARS_RIGHT = ultrasonic_implementation__get_right_ultrasonic_distance_in();
  sensor_sonar_struct.SENSOR_SONARS_FRONT = ultrasonic_implementation__get_front_ultrasonic_distance_in();
  sensor_sonar_struct.SENSOR_SONARS_BACK = ultrasonic_implementation__get_back_ultrasonic_distance_in();

  return dbc_encode_and_send_SENSOR_SONARS(NULL, &sensor_sonar_struct);
}

bool sensor_node__send_messages_over_can(void) {
  bool sent_all_messages = false;

  if (sensor_node__is_sync) {
    sent_all_messages = sensor_node__construct_and_send_ultrasonic_data();
  }

  return sent_all_messages;
}

void sensor_node__handle_mia(void) {
  // We are in 10hz slot, so increment MIA counter by 100ms
  const uint32_t mia_increment_value = 100;

  if (dbc_service_mia_DRIVER_HEARTBEAT(&can_msg__driver_heartbeat, mia_increment_value)) {
    puts("driver missing\r\n");
    sensor_node__is_sync = false;
    gpio__set(board_io__get_led0());
  }
}

static void sensor_node__handle_driver_heartbeat_message(const dbc_message_header_t *header,
                                                         const uint8_t can_msg_data_bytes[8]) {
  if (dbc_decode_DRIVER_HEARTBEAT(&can_msg__driver_heartbeat, *header, can_msg_data_bytes)) {
    if (!sensor_node__is_sync) {
      puts("sensor sync\r\n");
      sensor_node__is_sync = true;
      gpio__reset(board_io__get_led0());
    }
  }
}

void sensor_node__handle_messages_over_can(void) {
  can__msg_t can_msg = {0};

  while (can__rx(can1, &can_msg, 0)) {
    // Construct "message header" that we need for the decode_*() API
    const dbc_message_header_t header = {
        .message_id = can_msg.msg_id,
        .message_dlc = can_msg.frame_fields.data_len,
    };

    sensor_node__handle_driver_heartbeat_message(&header, can_msg.data.bytes);
  }
}

void sensor_node__collect_data(void) { ultrasonic_implementation__initiate_ultrasonics_range(); }

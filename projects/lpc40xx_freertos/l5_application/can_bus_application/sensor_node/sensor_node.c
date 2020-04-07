#include "sensor_node.h"

#include <stdio.h>

#include "board_io.h"
#include "can_bus.h"
#include "gpio.h"

#include "tesla_model_rc.h"
#include "ultrasonic_implementation.h"
#include "wifi_implementation.h"

static bool sensor_node__is_sync = false;
static dbc_DRIVER_HEARTBEAT_s can_msg__driver_heartbeat = {0};

void sensor_node__init(void) {
  wifi_implementation__initialize();
  ultrasonic_implementation__initialize();
}

static bool sensor_node__construct_and_send_gps_headings_data(void) {
  dbc_BRIDGE_SENSOR_GPS_HEADINGS_s gps_headings_struct = {0};

  // TODO: Create GPS headings module based on ESP8266 parsing
  gps_headings_struct.BRIDGE_SENSOR_GPS_HEADINGS_LONGITUDE = 1.0;
  gps_headings_struct.BRIDGE_SENSOR_GPS_HEADINGS_LATITUDE = 2.0;

  return dbc_encode_and_send_BRIDGE_SENSOR_GPS_HEADINGS(NULL, &gps_headings_struct);
}

static bool sensor_node__construct_and_send_vehicle_navigation_state_data(void) {
  dbc_BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE_s vehicle_navigation_state_struct = {0};

  // TODO: Create state module based on ESP8266 parsing
  vehicle_navigation_state_struct.BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE =
      BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE_NAVIGATE;

  return dbc_encode_and_send_BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE(NULL, &vehicle_navigation_state_struct);
}

static bool sensor_node__construct_and_send_voltage_data(void) {
  dbc_BRIDGE_SENSOR_VOLTAGE_s voltage_struct = {0};

  // TODO: Create ADC and read voltage input from battery
  voltage_struct.BRIDGE_SENSOR_VOLTAGE = 3.0;

  return dbc_encode_and_send_BRIDGE_SENSOR_VOLTAGE(NULL, &voltage_struct);
}

static bool sensor_node__construct_and_send_sonars_data(void) {
  dbc_BRIDGE_SENSOR_SONARS_s sonars_struct = {0};

  sonars_struct.BRIDGE_SENSOR_SONARS_LEFT = ultrasonic_implementation__get_left_ultrasonic_distance_in();
  sonars_struct.BRIDGE_SENSOR_SONARS_RIGHT = ultrasonic_implementation__get_right_ultrasonic_distance_in();
  sonars_struct.BRIDGE_SENSOR_SONARS_FRONT = ultrasonic_implementation__get_front_ultrasonic_distance_in();
  sonars_struct.BRIDGE_SENSOR_SONARS_BACK = ultrasonic_implementation__get_back_ultrasonic_distance_in();

  return dbc_encode_and_send_BRIDGE_SENSOR_SONARS(NULL, &sonars_struct);
}

bool sensor_node__send_messages_over_can(void) {
  bool sent_all_messages = false;

  if (sensor_node__is_sync) {
    sent_all_messages = sensor_node__construct_and_send_gps_headings_data();
    sent_all_messages = sensor_node__construct_and_send_vehicle_navigation_state_data();
    sent_all_messages = sensor_node__construct_and_send_voltage_data();
    sent_all_messages = sensor_node__construct_and_send_sonars_data();
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

void sensor_node__handle_wifi_transmissions(void) {
  wifi_implementation__fill_line_buffer();
  wifi_implementation__handle_line();
}

void sensor_node__collect_data(void) { ultrasonic_implementation__initiate_ultrasonics_range(); }

void sensor_node__test(void) { wifi_implementation__polled_test_echo(); }

#include "board_io.h"
#include "can_bus.h"
#include "gpio.h"

#include "tesla_model_rc.h"

#include "driver.h"

#include "can_bus_handler.h"
#include "can_bus_initializer.h"

static dbc_BRIDGE_SENSOR_SONARS_s sensor_data = {0};
static dbc_GEO_GPS_COMPASS_HEADINGS_s geo_gps_data = {0};
static dbc_BRIDGE_SENSOR_VOLTAGE_s battery_voltage = {0};
static dbc_BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE_s navigation_state = {0};
static dbc_MOTOR_HEARTBEAT_s motor_hbt = {0};
static dbc_BRIDGE_SENSOR_HEARTBEAT_s bridge_sensor_hbt = {0};
static dbc_GEO_HEARTBEAT_s geo_hbt = {0};
static dbc_MOTOR_ACTUAL_SPEED_s motor_actual_speed = {0};

static bool enable_mia_screen = false;
static uint8_t mia_replacement_signal_number = 0;

void can_handler__handle_all_incoming_messages_10Hz(void) {
  can__msg_t can_msg = {0};
  while (can__rx(can1, &can_msg, 0)) {
    const dbc_message_header_t header = {.message_id = can_msg.msg_id, .message_dlc = can_msg.frame_fields.data_len};
    // TODO: should motor actual speed be decoded?
    if (dbc_decode_BRIDGE_SENSOR_SONARS(&sensor_data, header, can_msg.data.bytes)) {
      enable_mia_screen = false;
      gpio__set(board_io__get_led0());
    }
    if (dbc_decode_GEO_GPS_COMPASS_HEADINGS(&geo_gps_data, header, can_msg.data.bytes)) {
      enable_mia_screen = false;
      gpio__set(board_io__get_led1());
    }
    if (dbc_decode_BRIDGE_SENSOR_VOLTAGE(&battery_voltage, header, can_msg.data.bytes)) {
      enable_mia_screen = false;
    }
    if (dbc_decode_MOTOR_ACTUAL_SPEED(&motor_actual_speed, header, can_msg.data.bytes)) {
      enable_mia_screen = false;
      gpio__set(board_io__get_led2());
    }
    if (dbc_decode_BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE(&navigation_state, header, can_msg.data.bytes)) {
      enable_mia_screen = false;
      gpio__set(board_io__get_led3());
    }

    // TODO implement heartbeat functionality
    // if (dbc_decode_MOTOR_HEARTBEAT(&motor_hbt, header, can_msg.data.bytes)) {
    //   enable_mia_screen = false;
    //   gpio__set(board_io__get_led0());
    // }
    // if (dbc_decode_BRIDGE_SENSOR_HEARTBEAT(&bridge_sensor_hbt, header, can_msg.data.bytes)) {
    //   enable_mia_screen = false;
    //   gpio__set(board_io__get_led1());
    // }
    // if (dbc_decode_GEO_HEARTBEAT(&geo_hbt, header, can_msg.data.bytes)) {
    //   enable_mia_screen = false;
    //   gpio__set(board_io__get_led2());
    // }
  }
  can_handler__manage_mia_10hz();
}

void can_handler__manage_mia_10hz(void) {
  const uint32_t mia_increment_value = 100;
  if (dbc_service_mia_BRIDGE_SENSOR_SONARS(&sensor_data, mia_increment_value)) {
    enable_mia_screen = true;
    mia_replacement_signal_number = 1;
    gpio__reset(board_io__get_led0());
  }
  if (dbc_service_mia_GEO_GPS_COMPASS_HEADINGS(&geo_gps_data, mia_increment_value)) {
    enable_mia_screen = true;
    mia_replacement_signal_number = 2;
    gpio__reset(board_io__get_led1());
  }
  if (dbc_service_mia_BRIDGE_SENSOR_VOLTAGE(&battery_voltage, mia_increment_value)) {
    enable_mia_screen = true;
    mia_replacement_signal_number = 3;
  }
  if (dbc_service_mia_MOTOR_ACTUAL_SPEED(&motor_actual_speed, mia_increment_value)) {
    enable_mia_screen = true;
    mia_replacement_signal_number = 5;
    gpio__reset(board_io__get_led2());
  }
  if (dbc_service_mia_BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE(&navigation_state, mia_increment_value)) {
    enable_mia_screen = true;
    mia_replacement_signal_number = 4;
    gpio__reset(board_io__get_led3());
  }

  // TODO implement heartbeat functionality
  // if (dbc_service_mia_MOTOR_HEARTBEAT(&motor_hbt, mia_increment_value)) {
  //   enable_mia_screen = true;
  //   mia_replacement_signal_number = 5;
  //   gpio__reset(board_io__get_led0());
  // }
  // if (dbc_service_mia_BRIDGE_SENSOR_HEARTBEAT(&bridge_sensor_hbt, mia_increment_value)) {
  //   enable_mia_screen = true;
  //   mia_replacement_signal_number = 6;
  //   gpio__reset(board_io__get_led1());
  // }
  // if (dbc_service_mia_GEO_HEARTBEAT(&geo_hbt, mia_increment_value)) {
  //   enable_mia_screen = true;
  //   mia_replacement_signal_number = 7;
  //   gpio__reset(board_io__get_led2());
  // }
}

void populate_signal_status_10Hz() {
  driver__process_heartbeats(motor_hbt, bridge_sensor_hbt, geo_hbt);
  driver__process_sensor_input(sensor_data);
  driver__process_navigation_state(navigation_state);
  driver__process_battery_voltage(battery_voltage);
  driver__process_geo_controller_directions(geo_gps_data);
  driver__process_mia_signal(enable_mia_screen, mia_replacement_signal_number);
}

void can_handler__transmit_heartbeat_10hz(void) {
  MOTOR_HEARTBEAT_CMD_e heartbeat_msg = {0};
  heartbeat_msg = 1;
  dbc_encode_and_send_DRIVER_HEARTBEAT(NULL, &heartbeat_msg);
}

void can_handler__transmit_debug_msg_10hz(void) {
  dbc_DRIVER_DEBUG_s driver_dbg_msg = {0};
  driver_dbg_msg.DRIVER_DEBUG_CAN_INIT = can_bus_initializer__get_can_bus_init_status();
  driver_dbg_msg.DRIVER_DEBUG_BUS_OFF = can_bus_initializer__get_can_bus_off();
  driver_dbg_msg.DRIVER_DEBUG_ALL_NODE_HBT = driver__get_node_heart_beats();
  dbc_encode_and_send_DRIVER_DEBUG(NULL, &driver_dbg_msg);
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

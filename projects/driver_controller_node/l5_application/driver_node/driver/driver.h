#pragma once

#include "external_lcd.h"
#include "tesla_model_rc.h"

void driver__process_sensor_input(dbc_BRIDGE_SENSOR_SONARS_s incoming_sensor_data);
void driver__process_geo_controller_directions(dbc_GEO_GPS_COMPASS_HEADINGS_s incoming_geo_heading);
void driver__process_heartbeats(dbc_MOTOR_HEARTBEAT_s motor_hbt, dbc_BRIDGE_SENSOR_HEARTBEAT_s bridge_sensor_hbt,
                                dbc_GEO_HEARTBEAT_s geo_hbt);
void driver__process_navigation_state(dbc_BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE_s navigation_state);
void driver__process_battery_voltage(dbc_BRIDGE_SENSOR_VOLTAGE_s battery_voltage);
void driver__process_mia_signal(bool enable_mia_screen, uint8_t mia_replacement_signal_number);
bool driver__get_node_heart_beats(void);
dbc_DRIVER_MOTOR_CONTROL_s driver__get_motor_commands(void);
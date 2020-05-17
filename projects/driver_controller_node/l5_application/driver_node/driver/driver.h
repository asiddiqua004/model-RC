#pragma once

#include "external_lcd.h"
#include "tesla_model_rc.h"

#define OBSTACLE_VERY_VERY_NEAR 8U
#define OBSTACLE_NO_TURN_DISTANCE 12U
#define OBSTACLE_VERY_NEAR 20U // 55U
#define OBSTACLE_NEAR 30U      // 65U //
#define OBSTACLE_MID 40U       // 100U //
#define OBSTACLE_MID_FAR 50U
#define OBSTACLE_FAR 70U // 160U //

#define MOTOR_VERY_SLOW_KPH 5.70
#define MOTOR_SLOW_KPH 5.80
#define MOTOR_MED_KPH 5.95
#define MOTOR_FAST_KPH 6.00
#define MOTOR_STOP_KPH 5.00
// #define  MOTOR_TEST_KPH   0.00f
// #define  MOTOR_REV_SLOW_KPH 0.40
#define MOTOR_REV_MED_KPH 4.10f //-0.50

#define STEER_LEFT 0.0       //-2.0
#define STEER_SOFT_LEFT 1.0  //-1.0
#define STEER_RIGHT 4.0      // 2.0
#define STEER_SOFT_RIGHT 3.0 // 3.0
#define STEER_STRAIGHT 2.0   // 0.0

void driver__process_sensor_input(dbc_BRIDGE_SENSOR_SONARS_s incoming_sensor_data);
void driver__process_geo_controller_directions(dbc_GEO_GPS_COMPASS_HEADINGS_s incoming_geo_heading);
void driver__process_heartbeats(dbc_MOTOR_HEARTBEAT_s motor_hbt, dbc_BRIDGE_SENSOR_HEARTBEAT_s bridge_sensor_hbt,
                                dbc_GEO_HEARTBEAT_s geo_hbt);
void driver__process_navigation_state(dbc_BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE_s navigation_state);
void driver__process_battery_voltage(dbc_BRIDGE_SENSOR_VOLTAGE_s battery_voltage);
void driver__process_mia_signal(bool enable_mia_screen, uint8_t mia_replacement_signal_number);
bool driver__get_node_heart_beats(void);
dbc_DRIVER_MOTOR_CONTROL_s driver__get_motor_commands(void);
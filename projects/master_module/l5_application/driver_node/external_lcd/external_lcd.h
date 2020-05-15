#pragma once

#include "tesla_model_rc.h"

typedef struct {
  uint8_t motor;
  uint8_t bridge_sensor;
  uint8_t geo;
} node_alive_s;

typedef struct {
  float speed;
  float steer;
} motor_s;

typedef struct {
  int8_t left;
  int8_t right;
  int8_t front;
  int8_t back;
} sensor_s;

typedef struct {
  float current_heading;
  float destination_heading;
} geo_s;

typedef enum {
  INIT = 0,
  WAIT = 1,
  OBSTACLE = 2,
  NAVIGATE = 3,
} state_machine_state_e;

typedef enum {
  NO_MOVEMENT = 0,
  STRAIGHT_MOVEMENT = 16,
  SOFT_LEFT_MOVEMENT = 18,
  HARD_LEFT_OR_RIGHT_MOVEMENT = 20,
  HARD_LEFT_MOVEMENT = 22,
  SOFT_RIGHT_MOVEMENT = 24,
  SLOW_STRAIGHT_MOVEMENT = 26,
  HARD_RIGHT_MOVEMENT = 28,
  REVERSE_MOVEMENT = 30,
  STOP = 31,
} obstacle_status_e;

typedef enum {
  NO_NAV = 0,
  LEFT_NAV = 12,
  STRAIGHT_NAV = 10,
  RIGHT_NAV = 9,
} navigation_status_e;

void lcd__init(void);
void lcd__display_screen_10Hz(void);
void set_lcd_node_heart_beats(node_alive_s incoming_node_alive);

void set_lcd_motor_status(motor_s incoming_motor_status);

void set_lcd_sensor_status(sensor_s incoming_sensor_status);

void set_lcd_compass_status(geo_s incoming_geo_status);

void set_lcd_battery_voltage(float incoming_battery_voltage);

void set_lcd_driver_state(char *incoming_driver_state);

void set_lcd_mia_replacement_signal_number(uint8_t incoming_mia_replacement_signal_number);
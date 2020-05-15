#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "external_lcd.h"

#include "delay.h"
#include "serial_lcd.h"
/****************PRIVATE VARIABLES ****************/
static node_alive_s lcd_node = {
    .motor = 0,
    .bridge_sensor = 0,
    .geo = 0,
};
static motor_s lcd_motor = {
    .speed = 0,
    .steer = 0,
};
static sensor_s lcd_sensor = {
    .left = 0,
    .right = 0,
    .front = 0,
    .back = 0,
};

static geo_s lcd_geo = {
    .current_heading = 0.0f,
    .destination_heading = 0.0f,
};

static float lcd_battery_voltage = 0.0f;

static uint8_t lcd_mia_replacement_signal_number = 0;

char *driver_state = "INIT";
/****************PRIVATE FUNCTIONS ****************/
// TODO implement heartbeat functionality
static bool all_nodes_alive(void) {
  // bool status = false;
  // if (lcd_node.motor == 1 && lcd_node.bridge_sensor == 1 && lcd_node.geo == 1) {
  //   status = true;
  // }
  // return status;
  return true;
}

// in the env_arm file remove  '-specs=nano.specs' from Linker flags
// to use %hhu
static void lcd_display_not_alive_screen(void) {
  uint8_t buf[20];
  sprintf(buf, "Tesla Model RC");
  lcd_row_write(1, buf);
  sprintf(buf, "Waiting to start");
  lcd_row_write(2, buf);
  sprintf(buf, "MN_HB:%hhu  BN_HB=%hhu", lcd_node.motor, lcd_node.bridge_sensor);
  lcd_row_write(3, buf);
  sprintf(buf, "GN_HB:%hhu  %s", lcd_node.geo, driver_state);
  lcd_row_write(4, buf);
}

static void lcd_display_status_screen(void) {
  uint8_t buf[20];
  sprintf(buf, "F:%hhuB:%hhuL:%hhu", lcd_sensor.front, lcd_sensor.back, lcd_sensor.left);
  lcd_row_write(1, buf);
  sprintf(buf, "R:%hhuCmp:%0.2f", lcd_sensor.right, lcd_geo.current_heading);
  lcd_row_write(2, buf);
  sprintf(buf, "Spd:%0.2fStr:%0.2f", lcd_motor.speed, lcd_motor.steer);
  lcd_row_write(3, buf);
  sprintf(buf, "Voltage:%0.2f %s", lcd_battery_voltage, driver_state);
  lcd_row_write(4, buf);
}

static void lcd_display_mia_screen(void) {
  uint8_t buf[20];
  switch (lcd_mia_replacement_signal_number) {
  case 1:
    sprintf(buf, "Mia signal-sensor");
    break;
  case 2:
    sprintf(buf, "Mia signal - geo");
    break;
  case 3:
    sprintf(buf, "Mia signal - voltg");
    break;
  case 4:
    sprintf(buf, "Mia signal - state");
    break;
  case 5:
    sprintf(buf, "Mia signal -Actspd ");
    break;
    // case 6:
    //   sprintf(buf, "Mia signal - SH");
    //   break;
    // case 7:
    //   sprintf(buf, "Mia signal - GH");
    break;
  default:
    sprintf(buf, "Mia-trigerred");
    break;
  }
  lcd_mia_replacement_signal_number = 0;
  lcd_row_write(1, buf);
  lcd_row_write(2, " ");
  lcd_row_write(3, " ");
  lcd_row_write(4, " ");
}

/****************PUBLIC FUNCTIONS ****************/

void set_lcd_node_heart_beats(node_alive_s incoming_node_alive) {
  lcd_node.motor = incoming_node_alive.motor;
  lcd_node.bridge_sensor = incoming_node_alive.bridge_sensor;
  lcd_node.geo = incoming_node_alive.geo;
}

void set_lcd_motor_status(motor_s incoming_motor_status) {
  lcd_motor.speed = incoming_motor_status.speed;
  lcd_motor.steer = incoming_motor_status.steer;
}

void set_lcd_sensor_status(sensor_s incoming_sensor_status) {
  lcd_sensor.left = incoming_sensor_status.left;
  lcd_sensor.right = incoming_sensor_status.right;
  lcd_sensor.front = incoming_sensor_status.front;
  lcd_sensor.back = incoming_sensor_status.back;
}

void set_lcd_compass_status(geo_s incoming_geo_status) {
  lcd_geo.current_heading = incoming_geo_status.current_heading;
  lcd_geo.destination_heading = incoming_geo_status.destination_heading;
}

void set_lcd_battery_voltage(float incoming_battery_voltage) { lcd_battery_voltage = incoming_battery_voltage; }

void set_lcd_mia_replacement_signal_number(uint8_t incoming_mia_replacement_signal_number) {
  lcd_mia_replacement_signal_number = incoming_mia_replacement_signal_number;
}

void set_lcd_driver_state(char *incoming_driver_state) { driver_state = incoming_driver_state; }

void lcd__init(void) { init_serial_lcd(); }

void lcd__display_screen_10Hz(void) {

  if (!all_nodes_alive()) {
    lcd_display_not_alive_screen();
  } else if (lcd_mia_replacement_signal_number > 0) {
    lcd_display_mia_screen();
  } else {
    lcd_display_status_screen();
  }
}

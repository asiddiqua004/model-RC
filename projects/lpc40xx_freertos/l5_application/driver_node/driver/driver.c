#include "driver.h"

static const int threshold = 50;
static dbc_SENSOR_SONARS_s sensor_data = {0};
static dbc_GEO_GPS_HEADINGS_s geo_heading = {0};

static void driver__stop_command(dbc_DRIVER_MOTOR_CONTROL_s *motor_commands) {
  motor_commands->DRIVER_MOTOR_CONTROL_SPEED_KPH = 0;
  motor_commands->DRIVER_MOTOR_CONTROL_STEER = 0;
}

static void driver__forward_command(dbc_DRIVER_MOTOR_CONTROL_s *motor_commands) {
  motor_commands->DRIVER_MOTOR_CONTROL_SPEED_KPH = 15;
  motor_commands->DRIVER_MOTOR_CONTROL_STEER = 0;
}

static void driver__right_command(dbc_DRIVER_MOTOR_CONTROL_s *motor_commands) {
  motor_commands->DRIVER_MOTOR_CONTROL_SPEED_KPH = 10;
  motor_commands->DRIVER_MOTOR_CONTROL_STEER = 1;
}

static void driver__left_command(dbc_DRIVER_MOTOR_CONTROL_s *motor_commands) {
  motor_commands->DRIVER_MOTOR_CONTROL_SPEED_KPH = 10;
  motor_commands->DRIVER_MOTOR_CONTROL_STEER = -1;
}

static bool driver__should_move_forward(void) {
  return (sensor_data.SENSOR_SONARS_FRONT > threshold) &&
         (geo_heading.GEO_GPS_HEADINGS_CURRENT == geo_heading.GEO_GPS_HEADINGS_DESTINATION);
}

static bool driver__should_move_right(void) {
  return (sensor_data.SENSOR_SONARS_RIGHT > threshold) ||
         (geo_heading.GEO_GPS_HEADINGS_CURRENT > geo_heading.GEO_GPS_HEADINGS_DESTINATION);
}

static bool driver__should_move_left(void) {
  return (sensor_data.SENSOR_SONARS_LEFT > threshold) ||
         (geo_heading.GEO_GPS_HEADINGS_CURRENT < geo_heading.GEO_GPS_HEADINGS_DESTINATION);
}

// This should copy the sensor data to its internal "static" struct instance of dbc_SENSOR_data_s
void driver__process_sensor_input(dbc_SENSOR_SONARS_s incoming_sensor_data) { sensor_data = incoming_sensor_data; }

void driver__process_geo_controller_directions(dbc_GEO_GPS_HEADINGS_s incoming_geo_heading) {
  geo_heading = incoming_geo_heading;
}

// This should operate on the "static" struct instance of dbc_SENSOR_data_s to run the obstacle avoidance logic
dbc_DRIVER_MOTOR_CONTROL_s driver__get_motor_commands(void) {
  dbc_DRIVER_MOTOR_CONTROL_s motor_commands = {0};
  driver__stop_command(&motor_commands);

  if (driver__should_move_forward()) {
    driver__forward_command(&motor_commands);
  } else if (driver__should_move_right()) {
    driver__right_command(&motor_commands);
  } else if (driver__should_move_left()) {
    driver__left_command(&motor_commands);
  }
  return motor_commands;
}
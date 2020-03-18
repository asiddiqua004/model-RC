#include "driver.h"

static const int threshold = 50;
static dbc_SENSOR_SONARS_s sensor_data = {0};

static void driver__stop_command(dbc_DRIVER_MOTOR_CONTROL_s *motor_commands) {
  motor_commands->DRIVER_MOTOR_CONTROL_CMD = DRIVER_MOTOR_CONTROL_CMD_STOP;
  motor_commands->DRIVER_MOTOR_CONTROL_SPEED_KPH = 0;
  motor_commands->DRIVER_MOTOR_CONTROL_STEER = 0;
}

static void driver__forward_command(dbc_DRIVER_MOTOR_CONTROL_s *motor_commands) {
  motor_commands->DRIVER_MOTOR_CONTROL_CMD = DRIVER_MOTOR_CONTROL_CMD_FORWARD;
  motor_commands->DRIVER_MOTOR_CONTROL_SPEED_KPH = 15;
  motor_commands->DRIVER_MOTOR_CONTROL_STEER = 0;
}

static void driver__right_command(dbc_DRIVER_MOTOR_CONTROL_s *motor_commands) {
  motor_commands->DRIVER_MOTOR_CONTROL_CMD = DRIVER_MOTOR_CONTROL_CMD_FORWARD;
  motor_commands->DRIVER_MOTOR_CONTROL_SPEED_KPH = 10;
  motor_commands->DRIVER_MOTOR_CONTROL_STEER = 1;
}

static void driver__left_command(dbc_DRIVER_MOTOR_CONTROL_s *motor_commands) {
  motor_commands->DRIVER_MOTOR_CONTROL_CMD = DRIVER_MOTOR_CONTROL_CMD_FORWARD;
  motor_commands->DRIVER_MOTOR_CONTROL_SPEED_KPH = 10;
  motor_commands->DRIVER_MOTOR_CONTROL_STEER = -1;
}

// This should copy the sensor data to its internal "static" struct instance of dbc_SENSOR_data_s
void driver__process_input(dbc_SENSOR_SONARS_s incoming_sensor_data) { sensor_data = incoming_sensor_data; }

// This should operate on the "static" struct instance of dbc_SENSOR_data_s to run the obstacle avoidance logic
dbc_DRIVER_MOTOR_CONTROL_s driver__get_motor_commands(void) {
  dbc_DRIVER_MOTOR_CONTROL_s motor_commands = {0};
  driver__stop_command(&motor_commands);

  if (sensor_data.SENSOR_SONARS_FRONT > threshold) {
    driver__forward_command(&motor_commands);
  } else if (sensor_data.SENSOR_SONARS_RIGHT > threshold) {
    driver__right_command(&motor_commands);
  } else if (sensor_data.SENSOR_SONARS_LEFT > threshold) {
    driver__left_command(&motor_commands);
  }
  return motor_commands;
}
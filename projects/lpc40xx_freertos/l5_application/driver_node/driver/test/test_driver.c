#include "driver.c"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {
  sensor_data.SENSOR_SONARS_LEFT = 0;
  sensor_data.SENSOR_SONARS_RIGHT = 0;
  sensor_data.SENSOR_SONARS_FRONT = 0;
  sensor_data.SENSOR_SONARS_BACK = 0;

  geo_heading.GEO_GPS_HEADINGS_CURRENT = 0;
  geo_heading.GEO_GPS_HEADINGS_DESTINATION = 0;
}

void test_driver__stop_command(void) {
  dbc_DRIVER_MOTOR_CONTROL_s motor_commands = {0};
  dbc_DRIVER_MOTOR_CONTROL_s expected_commands = {.DRIVER_MOTOR_CONTROL_SPEED_KPH = 0, .DRIVER_MOTOR_CONTROL_STEER = 0};
  driver__stop_command(&motor_commands);
  TEST_ASSERT_EQUAL_MEMORY(&expected_commands, &motor_commands, sizeof(dbc_DRIVER_MOTOR_CONTROL_s));
}

void test_driver__forward_command(void) {
  dbc_DRIVER_MOTOR_CONTROL_s motor_commands = {0};
  dbc_DRIVER_MOTOR_CONTROL_s expected_commands = {.DRIVER_MOTOR_CONTROL_SPEED_KPH = 15,
                                                  .DRIVER_MOTOR_CONTROL_STEER = 0};
  driver__forward_command(&motor_commands);
  TEST_ASSERT_EQUAL_MEMORY(&expected_commands, &motor_commands, sizeof(dbc_DRIVER_MOTOR_CONTROL_s));
}

void test_driver__right_command(void) {
  dbc_DRIVER_MOTOR_CONTROL_s motor_commands = {0};
  dbc_DRIVER_MOTOR_CONTROL_s expected_commands = {.DRIVER_MOTOR_CONTROL_SPEED_KPH = 10,
                                                  .DRIVER_MOTOR_CONTROL_STEER = 1};
  driver__right_command(&motor_commands);
  TEST_ASSERT_EQUAL_MEMORY(&expected_commands, &motor_commands, sizeof(dbc_DRIVER_MOTOR_CONTROL_s));
}

void test_driver__left_command(void) {
  dbc_DRIVER_MOTOR_CONTROL_s motor_commands = {0};
  dbc_DRIVER_MOTOR_CONTROL_s expected_commands = {.DRIVER_MOTOR_CONTROL_SPEED_KPH = 10,
                                                  .DRIVER_MOTOR_CONTROL_STEER = -1};
  driver__left_command(&motor_commands);
  TEST_ASSERT_EQUAL_MEMORY(&expected_commands, &motor_commands, sizeof(dbc_DRIVER_MOTOR_CONTROL_s));
}

void test_driver__process_sensor_input(void) {
  dbc_SENSOR_SONARS_s incoming_sensor_data = {
      .SENSOR_SONARS_LEFT = 10,
      .SENSOR_SONARS_RIGHT = 0,
      .SENSOR_SONARS_FRONT = 110,
      .SENSOR_SONARS_BACK = 300,
  };
  driver__process_sensor_input(incoming_sensor_data);
  TEST_ASSERT_EQUAL_MEMORY(&incoming_sensor_data, &sensor_data, sizeof(dbc_SENSOR_SONARS_s));
}

void test_driver__process_geo_controller_directions(void) {
  dbc_GEO_GPS_HEADINGS_s incoming_geo_heading = {
      .GEO_GPS_HEADINGS_CURRENT = 190,
      .GEO_GPS_HEADINGS_DESTINATION = 280,
  };
  driver__process_geo_controller_directions(incoming_geo_heading);
  TEST_ASSERT_EQUAL_MEMORY(&incoming_geo_heading, &geo_heading, sizeof(dbc_GEO_GPS_HEADINGS_s));
}

void test_driver__should_move_forward(void) {
  sensor_data.SENSOR_SONARS_LEFT = 10;
  sensor_data.SENSOR_SONARS_RIGHT = 0;
  sensor_data.SENSOR_SONARS_FRONT = 51;
  sensor_data.SENSOR_SONARS_BACK = 49;

  geo_heading.GEO_GPS_HEADINGS_CURRENT = 180;
  geo_heading.GEO_GPS_HEADINGS_DESTINATION = 180;

  TEST_ASSERT_TRUE(driver__should_move_forward());
}

void test_driver__should_move_right(void) {
  sensor_data.SENSOR_SONARS_LEFT = 10;
  sensor_data.SENSOR_SONARS_RIGHT = 99;
  sensor_data.SENSOR_SONARS_FRONT = 21;
  sensor_data.SENSOR_SONARS_BACK = 49;

  geo_heading.GEO_GPS_HEADINGS_CURRENT = 180;
  geo_heading.GEO_GPS_HEADINGS_DESTINATION = 0;

  TEST_ASSERT_TRUE(driver__should_move_right());
}

void test_driver__should_move_left(void) {
  sensor_data.SENSOR_SONARS_LEFT = 60;
  sensor_data.SENSOR_SONARS_RIGHT = 32;
  sensor_data.SENSOR_SONARS_FRONT = 100;
  sensor_data.SENSOR_SONARS_BACK = 49;

  geo_heading.GEO_GPS_HEADINGS_CURRENT = 90;
  geo_heading.GEO_GPS_HEADINGS_DESTINATION = 270;

  TEST_ASSERT_TRUE(driver__should_move_left());
}

void test_driver__get_motor_commands__all_sensors_zero(void) {
  sensor_data.SENSOR_SONARS_LEFT = 0;
  sensor_data.SENSOR_SONARS_RIGHT = 0;
  sensor_data.SENSOR_SONARS_FRONT = 0;
  sensor_data.SENSOR_SONARS_BACK = 0;

  dbc_DRIVER_MOTOR_CONTROL_s expected_commands = {0};
  driver__stop_command(&expected_commands);

  dbc_DRIVER_MOTOR_CONTROL_s commands = driver__get_motor_commands();

  TEST_ASSERT_EQUAL_MEMORY(&expected_commands, &commands, sizeof(dbc_DRIVER_MOTOR_CONTROL_s));
}

void test_driver__get_motor_commands__all_sensors_below_threshold(void) {
  sensor_data.SENSOR_SONARS_LEFT = 10;
  sensor_data.SENSOR_SONARS_RIGHT = 0;
  sensor_data.SENSOR_SONARS_FRONT = 21;
  sensor_data.SENSOR_SONARS_BACK = 49;

  dbc_DRIVER_MOTOR_CONTROL_s expected_commands = {0};
  driver__stop_command(&expected_commands);

  dbc_DRIVER_MOTOR_CONTROL_s commands = driver__get_motor_commands();

  TEST_ASSERT_EQUAL_MEMORY(&expected_commands, &commands, sizeof(dbc_DRIVER_MOTOR_CONTROL_s));
}

void test_driver__get_motor_commands__front_sensor_above_threshold_all_others_below_threshold(void) {
  sensor_data.SENSOR_SONARS_LEFT = 20;
  sensor_data.SENSOR_SONARS_RIGHT = 30;
  sensor_data.SENSOR_SONARS_FRONT = 100;
  sensor_data.SENSOR_SONARS_BACK = 3;

  dbc_DRIVER_MOTOR_CONTROL_s expected_commands = {0};
  driver__forward_command(&expected_commands);

  geo_heading.GEO_GPS_HEADINGS_CURRENT = 180;
  geo_heading.GEO_GPS_HEADINGS_DESTINATION = 180;

  dbc_DRIVER_MOTOR_CONTROL_s commands = driver__get_motor_commands();

  TEST_ASSERT_EQUAL_MEMORY(&expected_commands, &commands, sizeof(dbc_DRIVER_MOTOR_CONTROL_s));
}

void test_driver__get_motor_commands__all_sensors_above_threshold(void) {
  sensor_data.SENSOR_SONARS_LEFT = 70;
  sensor_data.SENSOR_SONARS_RIGHT = 300;
  sensor_data.SENSOR_SONARS_FRONT = 100;
  sensor_data.SENSOR_SONARS_BACK = 51;

  dbc_DRIVER_MOTOR_CONTROL_s expected_commands = {0};
  driver__forward_command(&expected_commands);

  dbc_DRIVER_MOTOR_CONTROL_s commands = driver__get_motor_commands();

  TEST_ASSERT_EQUAL_MEMORY(&expected_commands, &commands, sizeof(dbc_DRIVER_MOTOR_CONTROL_s));
}

void test_driver__get_motor_commands__right_sensor_above_threshold(void) {
  sensor_data.SENSOR_SONARS_LEFT = 70;
  sensor_data.SENSOR_SONARS_RIGHT = 300;
  sensor_data.SENSOR_SONARS_FRONT = 45;
  sensor_data.SENSOR_SONARS_BACK = 3;

  dbc_DRIVER_MOTOR_CONTROL_s expected_commands = {0};
  driver__right_command(&expected_commands);

  dbc_DRIVER_MOTOR_CONTROL_s commands = driver__get_motor_commands();

  TEST_ASSERT_EQUAL_MEMORY(&expected_commands, &commands, sizeof(dbc_DRIVER_MOTOR_CONTROL_s));
}

void test_driver__get_motor_commands__left_sensor_above_threshold(void) {
  sensor_data.SENSOR_SONARS_LEFT = 70;
  sensor_data.SENSOR_SONARS_RIGHT = 40;
  sensor_data.SENSOR_SONARS_FRONT = 31;
  sensor_data.SENSOR_SONARS_BACK = 9;

  dbc_DRIVER_MOTOR_CONTROL_s expected_commands = {0};
  driver__left_command(&expected_commands);

  dbc_DRIVER_MOTOR_CONTROL_s commands = driver__get_motor_commands();

  TEST_ASSERT_EQUAL_MEMORY(&expected_commands, &commands, sizeof(dbc_DRIVER_MOTOR_CONTROL_s));
}
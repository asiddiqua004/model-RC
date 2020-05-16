
#include "tesla_model_rc.h"

#include "Mockdelay.h"
#include "Mockserial_lcd.h"
#include "unity.h"

#include "external_lcd.c"

void setUp(void) {}

void tearDown(void) {}

void test__lcd__init(void) {
  init_serial_lcd_Expect();
  lcd__init();
}

void test__lcd_display_not_alive_screen() {
  for (uint8_t row = 1; row < 5; row++) {
    lcd_row_write_Expect(row, NULL);
    lcd_row_write_IgnoreArg_new_data();
  }
  lcd_display_not_alive_screen();
}

void test__lcd_display_status_screen() {
  for (uint8_t row = 1; row < 5; row++) {
    lcd_row_write_Expect(row, NULL);
    lcd_row_write_IgnoreArg_new_data();
  }
  lcd_display_status_screen();
}

void test__lcd_display_mia_screen(void) {
  lcd_mia_replacement_signal_number = 5;
  for (uint8_t row = 1; row < 5; row++) {
    lcd_row_write_Expect(row, NULL);
    lcd_row_write_IgnoreArg_new_data();
  }
  lcd_display_mia_screen();

  lcd_mia_replacement_signal_number = 18;
  for (uint8_t row = 1; row < 5; row++) {
    lcd_row_write_Expect(row, NULL);
    lcd_row_write_IgnoreArg_new_data();
  }
  lcd_display_mia_screen();
}

void test__all_nodes_alive_pass() {
  bool status = false;
  lcd_node.motor = true, lcd_node.bridge_sensor = true, lcd_node.geo = true;
  status = all_nodes_alive();
  TEST_ASSERT_EQUAL(true, status);
}

// void test__all_nodes_alive_fail() {
//   bool status = true;
//   lcd_node.motor = false, lcd_node.bridge_sensor = true, lcd_node.geo = true;
//   status = all_nodes_alive();
//   TEST_ASSERT_EQUAL(false, status);
//   status = true;
//   lcd_node.motor = true, lcd_node.bridge_sensor = false, lcd_node.geo = true;
//   status = all_nodes_alive();
//   TEST_ASSERT_EQUAL(false, status);
//   status = true;
//   lcd_node.motor = true, lcd_node.bridge_sensor = true, lcd_node.geo = false;
//   status = all_nodes_alive();
//   TEST_ASSERT_EQUAL(false, status);
// }

void test__lcd__display_screen_10Hz(void) {
  uint8_t row = 1;

  lcd_node.motor = true, lcd_node.bridge_sensor = false, lcd_node.geo = true;
  for (row = 1; row < 5; row++) {
    lcd_row_write_Expect(row, NULL);
    lcd_row_write_IgnoreArg_new_data();
  }
  lcd__display_screen_10Hz();

  lcd_node.motor = true, lcd_node.bridge_sensor = true, lcd_node.geo = true;
  for (row = 1; row < 5; row++) {
    lcd_row_write_Expect(row, NULL);
    lcd_row_write_IgnoreArg_new_data();
  }
  lcd__display_screen_10Hz();

  lcd_node.motor = true, lcd_node.bridge_sensor = true, lcd_node.geo = true;
  lcd_mia_replacement_signal_number = 3;
  for (row = 1; row < 5; row++) {
    lcd_row_write_Expect(row, NULL);
    lcd_row_write_IgnoreArg_new_data();
  }
  lcd__display_screen_10Hz();
}

void test_driver__set_node_heart_beats(void) {
  node_alive_s incoming_node_alive;
  incoming_node_alive.motor = 0;
  incoming_node_alive.bridge_sensor = 0;
  incoming_node_alive.geo = 0;
  set_lcd_node_heart_beats(incoming_node_alive);
  TEST_ASSERT_EQUAL_MEMORY(&incoming_node_alive, &lcd_node, sizeof(node_alive_s));
}

void test_driver__set_sensor_status(void) {
  sensor_s incoming_sensor_status = {
      .left = 10,
      .right = 40,
      .front = 50,
      .back = 200,
  };
  set_lcd_sensor_status(incoming_sensor_status);
  TEST_ASSERT_EQUAL_MEMORY(&incoming_sensor_status, &lcd_sensor, sizeof(sensor_s));
}

void test_driver__set_battery_voltage(void) {
  float incoming_battery_voltage = 5.3;
  set_lcd_battery_voltage(incoming_battery_voltage);
  TEST_ASSERT_EQUAL(incoming_battery_voltage, lcd_battery_voltage);
}

void test_driver__set_mia_replacement_signal_number(void) {
  uint8_t incoming_mia_replacement_signal_number = 5;
  set_lcd_mia_replacement_signal_number(incoming_mia_replacement_signal_number);
  TEST_ASSERT_EQUAL(incoming_mia_replacement_signal_number, lcd_mia_replacement_signal_number);
}

void test_driver__set_motor_status(void) {
  motor_s incoming_motor_status = {
      .speed = 10,
      .steer = -1,
  };
  set_lcd_motor_status(incoming_motor_status);
  TEST_ASSERT_EQUAL_MEMORY(&incoming_motor_status, &lcd_motor, sizeof(motor_s));
}
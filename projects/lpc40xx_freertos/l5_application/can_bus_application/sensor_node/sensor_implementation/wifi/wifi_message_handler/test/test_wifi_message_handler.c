#include "unity.h"

#include "wifi_message_handler.c"

void setUp(void) {
  vehicle_navigation_state = BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE_STOP;
  GPS_headings_longitude = 0.0;
  GPS_headings_latitude = 0.0;
}

void tearDown(void) {}

void test_wifi_message_handler__set_vehicle_navigation_state_to_navigate(void) {
  const char *line = "start";

  wifi_message_handler__set_vehicle_navigation_state(line);
  TEST_ASSERT_EQUAL(wifi_message_handler__get_vehicle_navigation_state(),
                    BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE_NAVIGATE);
}

void test_wifi_message_handler__set_vehicle_navigation_state_to_stop(void) {
  const char *line = "stop";

  wifi_message_handler__set_vehicle_navigation_state(line);
  TEST_ASSERT_EQUAL(wifi_message_handler__get_vehicle_navigation_state(), BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE_STOP);
}

void test_wifi_message_handler__set_vehicle_navigation_state_parse_incorrect_state(void) {
  const char *line = "incorrect state";

  wifi_message_handler__set_vehicle_navigation_state(line);
  TEST_ASSERT_EQUAL(wifi_message_handler__get_vehicle_navigation_state(), BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE_STOP);
}

void test_wifi_message_handler__set_GPS_headings_parse_positive(void) {
  const char *line = "+1234.5678,+8765.4321";

  wifi_message_handler__set_GPS_headings(line, strlen(line));
  TEST_ASSERT_EQUAL_FLOAT(wifi_message_handler__get_GPS_headings_latitude(), 1234.5678);
  TEST_ASSERT_EQUAL_FLOAT(wifi_message_handler__get_GPS_headings_longitude(), 8765.4321);
}

void test_wifi_message_handler__set_GPS_headings_parse_negative(void) {
  const char *line = "-1234.5678,-8765.4321";

  wifi_message_handler__set_GPS_headings(line, strlen(line));
  TEST_ASSERT_EQUAL_FLOAT(wifi_message_handler__get_GPS_headings_latitude(), -1234.5678);
  TEST_ASSERT_EQUAL_FLOAT(wifi_message_handler__get_GPS_headings_longitude(), -8765.4321);
}

void test_wifi_message_handler__set_GPS_headings_parse_positive_and_negative(void) {
  const char *line = "-1234.5678,+8765.4321";

  wifi_message_handler__set_GPS_headings(line, strlen(line));
  TEST_ASSERT_EQUAL_FLOAT(wifi_message_handler__get_GPS_headings_latitude(), -1234.5678);
  TEST_ASSERT_EQUAL_FLOAT(wifi_message_handler__get_GPS_headings_longitude(), 8765.4321);

  const char *second_line = "+1234.5678,-8765.4321";

  wifi_message_handler__set_GPS_headings(second_line, strlen(second_line));
  TEST_ASSERT_EQUAL_FLOAT(wifi_message_handler__get_GPS_headings_latitude(), 1234.5678);
  TEST_ASSERT_EQUAL_FLOAT(wifi_message_handler__get_GPS_headings_longitude(), -8765.4321);
}

void test_wifi_message_handler__set_GPS_headings_parse_incorrect_length(void) {
  const char *line = "-1234.5678,-8765.432";

  wifi_message_handler__set_GPS_headings(line, strlen(line));
  TEST_ASSERT_EQUAL_FLOAT(wifi_message_handler__get_GPS_headings_latitude(), 0.0);
  TEST_ASSERT_EQUAL_FLOAT(wifi_message_handler__get_GPS_headings_longitude(), 0.0);
}

void test_wifi_message_handler__parse_status_lines(void) {
  const char *line = "$start";

  wifi_message_handler__parse_line(line, strlen(line));
  TEST_ASSERT_EQUAL(wifi_message_handler__get_vehicle_navigation_state(),
                    BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE_NAVIGATE);

  const char *second_line = "$stop";

  wifi_message_handler__parse_line(second_line, strlen(second_line));
  TEST_ASSERT_EQUAL(wifi_message_handler__get_vehicle_navigation_state(), BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE_STOP);

  const char *third_line = "$incorrect";

  wifi_message_handler__parse_line(third_line, strlen(third_line));
  TEST_ASSERT_EQUAL(wifi_message_handler__get_vehicle_navigation_state(), BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE_STOP);
}

void test_wifi_message_handler__parse_GPS_headings_lines(void) {
  const char *line = "#+1111.2222,+3333.4444";

  wifi_message_handler__parse_line(line, strlen(line));
  TEST_ASSERT_EQUAL_FLOAT(wifi_message_handler__get_GPS_headings_latitude(), 1111.2222);
  TEST_ASSERT_EQUAL_FLOAT(wifi_message_handler__get_GPS_headings_longitude(), 3333.4444);

  const char *second_line = "#-5555.6666,-7777.8888";

  wifi_message_handler__parse_line(second_line, strlen(second_line));
  TEST_ASSERT_EQUAL_FLOAT(wifi_message_handler__get_GPS_headings_latitude(), -5555.666);
  TEST_ASSERT_EQUAL_FLOAT(wifi_message_handler__get_GPS_headings_longitude(), -7777.888);

  const char *third_line = "#incorrect";

  wifi_message_handler__parse_line(third_line, strlen(third_line));
  TEST_ASSERT_EQUAL_FLOAT(wifi_message_handler__get_GPS_headings_latitude(), -5555.6666);
  TEST_ASSERT_EQUAL_FLOAT(wifi_message_handler__get_GPS_headings_longitude(), -7777.8888);
}
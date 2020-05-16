#include "unity.h"

#include "Mockcan_bus.h"
#include "Mocksensor_node.h"

#include "can_bus_handler.h"

void setUp(void) {}
void tearDown(void) {}

void test_can_bus_handler__transmit_messages_1Hz(void) {
  // sensor_node__test();
  can_bus_handler__transmit_messages_1Hz();
}

void test_can_bus_handler__transmit_messages_10Hz(void) {
  sensor_node__send_messages_over_can_ExpectAndReturn(true);
  sensor_node__send_radars_to_wifi_Expect();

  can_bus_handler__transmit_messages_10Hz();
}

void test_can_bus_handler__manage_mia_10Hz(void) {
  const uint32_t mia_increment_value = 100;
  sensor_node__handle_mia_Expect(mia_increment_value);

  can_bus_handler__manage_mia_10Hz();
}

void test_can_bus_handler__handle_all_incoming_messages_10Hz(void) {
  sensor_node__handle_wifi_lines_Expect();
  sensor_node__handle_messages_over_can_Expect();

  can_bus_handler__handle_all_incoming_messages_10Hz();
}

void test_can_bus_handler__collect_data_50Hz(void) {
  sensor_node__collect_data_Expect();

  can_bus_handler__collect_data_100Hz();
}

void test_can_bus_handler__handle_wifi_transmissions_100Hz(void) {
  sensor_node__handle_wifi_transmissions_Expect();

  can_bus_handler__handle_wifi_transmissions_100Hz();
}

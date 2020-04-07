#include "unity.h"

#include "Mockcan_bus.h"
#include "Mocksensor_node.h"

#include "can_bus_handler.h"

void setUp(void) {}
void tearDown(void) {}

void test_can_bus_handler__transmit_messages_10hz(void) {
  sensor_node__send_messages_over_can_ExpectAndReturn(true);

  can_bus_handler__transmit_messages_10hz();
}

void test_can_bus_handler__manage_mia_10hz(void) {
  sensor_node__handle_mia_Expect();

  can_bus_handler__manage_mia_10hz();
}

void test_can_bus_handler__handle_all_incoming_messages_10hz(void) {
  sensor_node__handle_wifi_transmissions_Expect();
  sensor_node__handle_messages_over_can_Expect();

  can_bus_handler__handle_all_incoming_messages_10hz();
}

void test_can_bus_handler__collect_data_10hz(void) {
  sensor_node__collect_data_Expect();

  can_bus_handler__collect_data_10hz();
}

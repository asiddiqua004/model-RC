#include "can_bus_handler.h"

#include "sensor_node.h"

void can_bus_handler__init(void) { sensor_node__init(); }

void can_bus_handler__transmit_messages_1hz(void) {
  // sensor_node__test();
}

void can_bus_handler__transmit_messages_10hz(void) { sensor_node__send_messages_over_can(); }

void can_bus_handler__manage_mia_10hz(void) { sensor_node__handle_mia(); }

void can_bus_handler__handle_all_incoming_messages_10hz(void) {
  sensor_node__handle_wifi_transmissions();
  sensor_node__handle_messages_over_can();
}

void can_bus_handler__collect_data_10hz(void) { sensor_node__collect_data(); }

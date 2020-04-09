#include "can_bus_handler.h"

#include "sensor_node.h"

void can_bus_handler__init(void) { sensor_node__init(); }

void can_bus_handler__transmit_messages_1Hz(void) {
  // sensor_node__test();
}

void can_bus_handler__transmit_messages_10Hz(void) { sensor_node__send_messages_over_can(); }

void can_bus_handler__manage_mia_10Hz(void) {
  // We are in 10hz slot, so increment MIA counter by 100ms
  static const uint32_t mia_increment_value = 100;
  sensor_node__handle_mia(mia_increment_value);
}

void can_bus_handler__handle_all_incoming_messages_10Hz(void) {
  sensor_node__handle_wifi_transmissions();
  sensor_node__handle_messages_over_can();
}

void can_bus_handler__collect_data_20Hz(void) { sensor_node__collect_data(); }

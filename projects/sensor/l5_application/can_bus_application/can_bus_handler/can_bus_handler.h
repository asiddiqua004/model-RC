#pragma once

void can_bus_handler__init(void);

void can_bus_handler__transmit_messages_1Hz(void);

void can_bus_handler__transmit_messages_10Hz(void);

void can_bus_handler__manage_mia_10Hz(void);

void can_bus_handler__handle_all_incoming_messages_10Hz(void);

void can_bus_handler__collect_data_20Hz(void);

void can_bus_handler__handle_wifi_transmissions_100Hz(void);
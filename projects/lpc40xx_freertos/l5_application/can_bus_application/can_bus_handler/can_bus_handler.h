#pragma once

void can_bus_handler__init(void);

void can_bus_handler__transmit_messages_10hz(void);

void can_bus_handler__manage_mia_10hz(void);

void can_bus_handler__handle_all_incoming_messages_10hz(void);

void can_bus_handler__collect_data_10hz(void);

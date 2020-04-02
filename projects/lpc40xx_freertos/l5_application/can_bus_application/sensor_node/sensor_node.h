#pragma once

#include <stdbool.h>

void sensor_node__init(void);

bool sensor_node__send_messages_over_can(void);

void sensor_node__handle_mia(void);

void sensor_node__handle_messages_over_can(void);

void sensor_node__collect_data(void);

#pragma once

#include <stdbool.h>
#include <stdint.h>

void sensor_node__init(void);

bool sensor_node__send_messages_over_can(void);

void sensor_node__handle_mia(const uint32_t mia_increment_value);

void sensor_node__handle_wifi_transmissions(void);

void sensor_node__handle_wifi_lines(void);

void sensor_node__handle_messages_over_can(void);

void sensor_node__collect_data(void);

void sensor_node__test(void);

#pragma once
#include <stdbool.h>
void can_bus_initializer__init(void);
void can_bus_initializer__turn_can_bus_on_if_off(void);
bool can_bus_initializer__get_can_bus_init_status(void);
bool can_bus_initializer__get_can_bus_off(void);
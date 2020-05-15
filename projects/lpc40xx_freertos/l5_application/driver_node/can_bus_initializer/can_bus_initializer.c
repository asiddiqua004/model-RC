#include "can_bus.h"
#include <stddef.h>

#include "can_bus_initializer.h"

static const can__num_e can_bus = can1;

void can_bus_initializer__init() {
  if (can__init(can_bus, 250, 100, 100, NULL, NULL)) {
    can__bypass_filter_accept_all_msgs();
    can__reset_bus(can_bus);
  }
}

void can_bus_initializer__turn_can_bus_on_if_off() {
  if (can__is_bus_off(can_bus)) {
    can__reset_bus(can_bus);
  }
}
#include "can_bus.h"
#include <stddef.h>

#include "can_bus_initializer.h"

static const can__num_e can_bus = can1;
static bool can_bus_init_status = false;

bool can_bus_initializer__init(void) {
  can_bus_init_status = can__init(can_bus, 250, 100, 100, NULL, NULL);
  if (can_bus_init_status) {
    can__bypass_filter_accept_all_msgs();
    can__reset_bus(can_bus);
  }
  return can_bus_init_status;
}

void can_bus_initializer__turn_can_bus_on_if_off(void) {
  if (can__is_bus_off(can_bus)) {
    can__reset_bus(can_bus);
  }
}

bool can_bus_initializer__get_can_bus_init_status(void) { return can_bus_init_status; }

bool can_bus_initializer__get_can_bus_off(void) { return can__is_bus_off(can_bus); }
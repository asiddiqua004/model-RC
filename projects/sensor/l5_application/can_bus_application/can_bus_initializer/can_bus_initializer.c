#include "can_bus_initializer.h"

#include <stdio.h>

static void bus_off_callback(uint32_t icr_value) { (void)icr_value; }

static void data_overrun_callback(uint32_t icr_value) { (void)icr_value; }

void can_bus__init(void) {
  can__init(can1, // P0.0 CAN_RD1, P0.1 CAN_TX1
            250,  // 250 kbps
            128,  // 128 byte rx queue
            128,  // 128 byte tx queue
            bus_off_callback, data_overrun_callback);

  can__bypass_filter_accept_all_msgs();
  can__reset_bus(can1);
}

void can_bus__handle_bus_off(void) {
  if (can__is_bus_off(can1)) {
    can__reset_bus(can1);
  }
}

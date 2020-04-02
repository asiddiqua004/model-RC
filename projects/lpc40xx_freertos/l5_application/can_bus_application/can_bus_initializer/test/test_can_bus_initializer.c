#include "unity.h"

#include "Mockcan_bus.h"

#include "can_bus_initializer.h"

void setUp(void) {}

void tearDown(void) {}

void test_can_bus__init(void) {
  can__init_ExpectAndReturn(can1, 250, 128, 128, NULL, NULL, true);
  can__init_IgnoreArg_bus_off_cb();
  can__init_IgnoreArg_data_ovr_cb();

  can__bypass_filter_accept_all_msgs_Expect();
  can__reset_bus_Expect(can1);

  can_bus__init();
}

void test_can_bus__reset_on_bus_off(void) {
  // Does not call can__reset_bus on false
  can__is_bus_off_ExpectAndReturn(can1, false);

  can_bus__handle_bus_off();

  // Resume flow on true
  can__is_bus_off_ExpectAndReturn(can1, true);
  can__reset_bus_Expect(can1);

  can_bus__handle_bus_off();
}
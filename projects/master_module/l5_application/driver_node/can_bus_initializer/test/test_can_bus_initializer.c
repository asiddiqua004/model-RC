#include "Mockcan_bus.h"
#include "unity.h"

#include "can_bus_initializer.c"

void test_can_bus_initializer__init__success(void) {
  can__init_ExpectAndReturn(can_bus, 250, 100, 100, NULL, NULL, true);
  can__bypass_filter_accept_all_msgs_Expect();
  can__reset_bus_Expect(can_bus);
  can_bus_initializer__init();
}

void test_can_bus_initializer__init__fail(void) {
  can__init_ExpectAndReturn(can_bus, 250, 100, 100, NULL, NULL, false);
  can_bus_initializer__init();
}

void test_can_bus_initializer__turn_can_bus_on_if_off__bus_is_off(void) {
  can__is_bus_off_ExpectAndReturn(can_bus, true);
  can__reset_bus_Expect(can_bus);
  can_bus_initializer__turn_can_bus_on_if_off();
}

void test_can_bus_initializer__turn_can_bus_on_if_off__bus_is_on(void) {
  can__is_bus_off_ExpectAndReturn(can_bus, false);
  can_bus_initializer__turn_can_bus_on_if_off();
}
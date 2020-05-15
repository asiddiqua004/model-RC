#include <stdio.h>
#include <string.h>

#include "unity.h"

// Include the Mocks
// - This will not pull the REAL source code of these modules (such as board_io.c)
// - This will auto-generate "Mock" versions based on the header file
#include "Mockboard_io.h"
#include "Mockgpio.h"

#include "Mockcan_bus_handler.h"
#include "Mockcan_bus_initializer.h"

// Include the source we wish to test
#include "periodic_callbacks.h"

void setUp(void) {}

void tearDown(void) {}

void test__periodic_callbacks__initialize(void) {
  gpio_s gpio = {};
  can_bus_initializer__init_Expect();
  board_io__get_led0_ExpectAndReturn(gpio);
  gpio__set_Expect(gpio);

  board_io__get_led1_ExpectAndReturn(gpio);
  gpio__set_Expect(gpio);

  board_io__get_led2_ExpectAndReturn(gpio);
  gpio__set_Expect(gpio);

  board_io__get_led3_ExpectAndReturn(gpio);
  gpio__set_Expect(gpio);

  periodic_callbacks__initialize();
}

void test__periodic_callbacks__1Hz(void) {
  // gpio_s gpio = {};
  // board_io__get_led0_ExpectAndReturn(gpio);
  // gpio__toggle_Expect(gpio);
  periodic_callbacks__1Hz(0);
}
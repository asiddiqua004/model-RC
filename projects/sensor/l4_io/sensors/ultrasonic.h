#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "gpio.h"

typedef struct ultrasonic {
  uint64_t last_trigger_time_us;
  uint64_t time_of_flight_us;
  bool start_new_reading;
  gpio_s trigger_out;
  gpio_s echo_in;
} ultrasonic_s;

void ultrasonic__initialize(ultrasonic_s *ultrasonic, gpio__port_e trigger_port, uint8_t trigger_pin_number_0_to_31,
                            gpio__port_e echo_port, uint8_t echo_pin_number_0_to_31);

void ultrasonic__get_range_blocking(ultrasonic_s *ultrasonic);

void ultrasonic__initiate_range(ultrasonic_s *ultrasonic);

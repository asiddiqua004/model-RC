#pragma once

#include <stdint.h>

/**
  This GPIO interrupt module only works for rising/falling edge of Port 2 pins.
*/

typedef enum { RISING = 0, FALLING } gpio_interrupts_edge_e;

void gpio2_interrupts__initialize(void);

void gpio2_interrupts__attach_handler(const uint8_t pin_number_0_to_31, const gpio_interrupts_edge_e edge,
                                      void (*isr_handler)(void));

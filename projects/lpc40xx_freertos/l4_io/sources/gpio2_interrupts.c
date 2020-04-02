#include "gpio2_interrupts.h"

#include "lpc40xx.h"
#include "lpc_peripherals.h"

#define GPIO_P2INT_FLAG(gpio_int_status_register) (gpio_int_status_register & (1U << 2U))

static void (*port2_edge_to_pin_isr_map[2][32])(void);

// Handler for port 2 rising and falling GPIO interrupts
static void gpio2_interrupts__interrupt_handler(void) {
  if (GPIO_P2INT_FLAG(LPC_GPIOINT->IntStatus)) {
    gpio_interrupts_edge_e edge = RISING;
    uint32_t interrupt_register;

    if (LPC_GPIOINT->IO2IntStatR) {
      interrupt_register = LPC_GPIOINT->IO2IntStatR;
    } else {
      interrupt_register = LPC_GPIOINT->IO2IntStatF;
      edge = FALLING;
    }

    uint8_t bit_shift_count = 16U;
    uint16_t upper_half = interrupt_register >> bit_shift_count;
    uint16_t lower_half = interrupt_register;

    uint16_t mask = 0U;
    uint16_t bit_position = 0U;

    while (bit_shift_count) {
      if (upper_half) {
        bit_position += bit_shift_count;
        bit_shift_count >>= 1U;
        mask = (1U << bit_shift_count) - 1U;

        lower_half = upper_half & mask;
        upper_half >>= bit_shift_count;
      } else {
        bit_shift_count >>= 1U;
        mask = (1U << bit_shift_count) - 1U;

        upper_half = lower_half >> bit_shift_count;
        lower_half &= mask;
      }
    }

    port2_edge_to_pin_isr_map[edge][bit_position]();
    LPC_GPIOINT->IO2IntClr |= (1U << bit_position);
  } else {
    // Clear GPIO Port 0 interrupt
    LPC_GPIOINT->IO0IntClr = ~0U;
  }
}

void gpio2_interrupts__initialize(void) {
  lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__GPIO, gpio2_interrupts__interrupt_handler, "GPIOINT");
}

void gpio2_interrupts__attach_handler(const uint8_t pin_number_0_to_31, const gpio_interrupts_edge_e edge,
                                      void (*isr_handler)(void)) {
  // Enable rising or falling edge interrupt on Port 2 pin
  if (RISING == edge) {
    LPC_GPIOINT->IO2IntEnR |= (1U << pin_number_0_to_31);
  } else {
    LPC_GPIOINT->IO2IntEnF |= (1U << pin_number_0_to_31);
  }

  // set ISR handler
  port2_edge_to_pin_isr_map[edge][pin_number_0_to_31] = isr_handler;
}

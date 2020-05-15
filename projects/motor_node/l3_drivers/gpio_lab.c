/*===========================================================*
 * gpio_lab.c -- implementation for gpio lab driver          *
 *                                                           *
 * Author:  Nickolas  Schiffer : CMPE-244                    *
 *                                                           *
 * Date: Sat Sep 14 2019                                     *
 *                                                           *
 * Purpose: Create a (generic) driver for gpio functionality *
 *                                                           *
 * Usage:                                                    *
 *      See below.                                           *
 *===========================================================*/

#include "gpio_lab.h"

#include "uart_printf.h"

#include <lpc_peripherals.h>
#include <stdbool.h>
#include <stdint.h>

#include "lpc40xx.h"

#define PCONP_GPIO_BIT_ (1U << 15)

#define VALID_PIN(port, pin) (((port) == GPIO_5 && (pin) <= PIN_5) || ((port) != GPIO_5))
#define VALID_INT_PORT(port) ((port) == GPIO_0 || (port) == GPIO_2)

static LPC_GPIO_TypeDef *const gpio_ports[] = {LPC_GPIO0, LPC_GPIO1, LPC_GPIO2, LPC_GPIO3, LPC_GPIO4, LPC_GPIO5};

static function__void_f interrupt_table_rising[2][32] = {{0}};
static function__void_f interrupt_table_falling[2][32] = {{0}};

static const uint8_t NUM_GPIO_PORTS_INT = 2;

/// Should enable GPIO peripheral
void gpiolab__enable_gpio() { LPC_SC->PCONP |= PCONP_GPIO_BIT_; }
/// Should disable GPIO peripheral
void gpiolab__disable_gpio() { LPC_SC->PCONP &= ~PCONP_GPIO_BIT_; }

/// Should alter the hardware registers to set the pin as input
void gpiolab__set_as_input(GPIO_PORT_e port, GPIO_PIN_e pin) {
  if (VALID_PIN(port, pin))
    gpio_ports[port]->DIR &= ~(1U << pin);
}
/// Should alter the hardware registers to set the pin as output
void gpiolab__set_as_output(GPIO_PORT_e port, GPIO_PIN_e pin) {
  if (VALID_PIN(port, pin))
    gpio_ports[port]->DIR |= (1U << pin);
}

/// Should alter the hardware registers to set the pin as high
void gpiolab__set_high(GPIO_PORT_e port, GPIO_PIN_e pin) {
  if (VALID_PIN(port, pin))
    gpio_ports[port]->SET = (1U << pin);
}

/// Should alter the hardware registers to set the pin as low
void gpiolab__set_low(GPIO_PORT_e port, GPIO_PIN_e pin) {
  if (VALID_PIN(port, pin))
    gpio_ports[port]->CLR = (1U << pin);
}

/**
 * Should return the state of the pin (input or output, doesn't matter)
 *
 * @return {bool} level of pin high => true, low => false
 */
bool gpiolab__get_level(GPIO_PORT_e port, GPIO_PIN_e pin) { return (bool)(gpio_ports[port]->PIN & (1U << pin)); }

/// Should alter the hardware registers to toggle the pin
void gpiolab__toggle(GPIO_PORT_e port, GPIO_PIN_e pin) {
  // Thread safe options are preferred over '^='.
  // if (VALID_PIN(port, pin)) gpio_ports[port]->PIN ^= (uint32_t)(1 << pin);
  if (!VALID_PIN(port, pin))
    return;
  if (gpiolab__get_level(port, pin))
    gpio_ports[port]->CLR = (1U << pin);
  else
    gpio_ports[port]->SET = (1U << pin);
}

/**
 * Should alter the hardware registers to set the pin as low
 *
 * @param {bool} high - true => set pin high, false => set pin low
 */
void gpiolab__set(GPIO_PORT_e port, GPIO_PIN_e pin, bool high) {
  if (!VALID_PIN(port, pin))
    return;
  if (high)
    gpio_ports[port]->SET = (1U << pin); // Thread safe options are
  else
    gpio_ports[port]->CLR = (1U << pin); // preferred over '^='.
}

/**
 * Allows the user to attach an interrupt callback to GPIO 0 & 2 pins.
 */
void gpiolab__attach_interrupt(GPIO_PORT_e port, GPIO_PIN_e pin, GPIO_EDGE_e edge_type, function__void_f callback) {
  if (!VALID_INT_PORT(port))
    return;
  switch (edge_type) {
  case GPIO_INTR__FALLING_EDGE: {
    interrupt_table_falling[(port == GPIO_0) ? 0 : 1][pin] = callback;
    (port == GPIO_0) ? (LPC_GPIOINT->IO0IntEnF |= (1U << pin)) : (LPC_GPIOINT->IO2IntEnF |= (1U << pin));
    return;
  }
  case GPIO_INTR__RISING_EDGE: {
    interrupt_table_rising[(port == GPIO_0) ? 0 : 1][pin] = callback;
    (port == GPIO_0) ? (LPC_GPIOINT->IO0IntEnR |= (1U << pin)) : (LPC_GPIOINT->IO2IntEnR |= (1U << pin));
    return;
  }
  case GPIO_INTR__BOTH_EDGES: {
    interrupt_table_falling[(port == GPIO_0) ? 0 : 1][pin] = callback;
    interrupt_table_rising[(port == GPIO_0) ? 0 : 1][pin] = callback;
    (port == GPIO_0) ? (LPC_GPIOINT->IO0IntEnR |= (1U << pin)) : (LPC_GPIOINT->IO2IntEnR |= (1U << pin));
    (port == GPIO_0) ? (LPC_GPIOINT->IO0IntEnF |= (1U << pin)) : (LPC_GPIOINT->IO2IntEnF |= (1U << pin));
    return;
  }
  default:
    return;
  }
}

static void gpiolab__interrupt_dispatcher(void) {
  uint8_t counter = 0;
  // Check if there is a pending interrupt on GPIO0
  if (LPC_GPIOINT->IntStatus & (1 << GPIO_0)) {
    while ((counter < 32) && (LPC_GPIOINT->IO0IntStatF >> counter)) {
      if (LPC_GPIOINT->IO0IntStatF >> counter & 1) {
        if (interrupt_table_falling[0][counter])
          interrupt_table_falling[0][counter]();
        LPC_GPIOINT->IO0IntClr |= (1U << counter);
      }
      counter++;
    }
  }
  if (LPC_GPIOINT->IntStatus & (1 << GPIO_0)) {
    counter = 0;
    while ((counter < 32) && (LPC_GPIOINT->IO0IntStatR >> counter)) {
      if (LPC_GPIOINT->IO0IntStatR >> counter & 1) {
        if (interrupt_table_rising[0][counter])
          interrupt_table_rising[0][counter]();
        LPC_GPIOINT->IO0IntClr |= (1U << counter);
      }
      counter++;
    }
  }
  if (LPC_GPIOINT->IntStatus & (1 << GPIO_2)) {
    counter = 0;
    while ((counter < 32) && (LPC_GPIOINT->IO2IntStatF >> counter)) {
      if (LPC_GPIOINT->IO2IntStatF >> counter & 1) {
        if (interrupt_table_falling[1][counter])
          interrupt_table_falling[1][counter]();
        LPC_GPIOINT->IO2IntClr |= (1U << counter);
      }
      counter++;
    }
  }
  if (LPC_GPIOINT->IntStatus & (1 << GPIO_2)) {
    counter = 0;
    while ((counter < 32) && (LPC_GPIOINT->IO2IntStatR >> counter)) {
      if (LPC_GPIOINT->IO2IntStatR >> counter & 1) {
        if (interrupt_table_rising[1][counter])
          interrupt_table_rising[1][counter]();
        LPC_GPIOINT->IO2IntClr |= (1U << counter);
      }
      counter++;
    }
  }
}

void gpiolab__enable_interrupts(void) {
  NVIC_EnableIRQ(GPIO_IRQn);
  lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__GPIO, gpiolab__interrupt_dispatcher, "gpio_interrupt");
}

void gpiolab__disable_interrupts(void) { NVIC_DisableIRQ(GPIO_IRQn); }
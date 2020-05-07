/*===========================================================*
 * gpio_lab.h -- header for gpio lab driver                  *
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

#pragma once

#include <function_types.h>
#include <stdbool.h>

const typedef enum {
  GPIO_0,
  GPIO_1,
  GPIO_2,
  GPIO_3,
  GPIO_4,
  GPIO_5,
} GPIO_PORT_e;

const typedef enum {
  PIN_0,
  PIN_1,
  PIN_2,
  PIN_3,
  PIN_4,
  PIN_5,
  PIN_6,
  PIN_7,
  PIN_8,
  PIN_9,
  PIN_10,
  PIN_11,
  PIN_12,
  PIN_13,
  PIN_14,
  PIN_15,
  PIN_16,
  PIN_17,
  PIN_18,
  PIN_19,
  PIN_20,
  PIN_21,
  PIN_22,
  PIN_23,
  PIN_24,
  PIN_25,
  PIN_26,
  PIN_27,
  PIN_28,
  PIN_29,
  PIN_30,
  PIN_31
} GPIO_PIN_e;

const typedef enum { GPIO_INTR__FALLING_EDGE, GPIO_INTR__RISING_EDGE, GPIO_INTR__BOTH_EDGES } GPIO_EDGE_e;

/// Should enable GPIO peripheral
void gpiolab__enable_gpio();
/// Should disable GPIO peripheral
void gpiolab__disable_gpio();

/// Should alter the hardware registers to set the pin as input
void gpiolab__set_as_input(GPIO_PORT_e port, GPIO_PIN_e pin);
/// Should alter the hardware registers to set the pin as output
void gpiolab__set_as_output(GPIO_PORT_e port, GPIO_PIN_e pin);
/// Should alter the hardware registers to set the pin as high
void gpiolab__set_high(GPIO_PORT_e port, GPIO_PIN_e pin);

/// Should alter the hardware registers to set the pin as low
void gpiolab__set_low(GPIO_PORT_e port, GPIO_PIN_e pin);

/**
 * Should return the state of the pin (input or output, doesn't matter)
 *
 * @return {bool} level of pin high => true, low => false
 */
bool gpiolab__get_level(GPIO_PORT_e port, GPIO_PIN_e pin);

/// Should alter the hardware registers to toggle the pin
void gpiolab__toggle(GPIO_PORT_e port, GPIO_PIN_e pin);

/**
 * Should alter the hardware registers to set the pin as low
 *
 * @param {bool} high - true => set pin high, false => set pin low
 */
void gpiolab__set(GPIO_PORT_e port, GPIO_PIN_e pin, bool high);

/**
 * Allows the user to attach an interrupt callback to GPIO 0 & 2 pins.
 */
void gpiolab__attach_interrupt(GPIO_PORT_e port, GPIO_PIN_e pin, GPIO_EDGE_e edge_type, function__void_f callback);

void gpiolab__enable_interrupts(void);

void gpiolab__disable_interrupts(void);
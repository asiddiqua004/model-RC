#include "ultrasonic_implementation.h"

#include <stdio.h>

#include "gpio2_interrupts.h"
#include "sys_time.h"
#include "ultrasonic.h"

static ultrasonic_s front_ultrasonic, back_ultrasonic, left_ultrasonic, right_ultrasonic;

static void front_ultrasonic_echo_falling_edge_interrupt_handler(void) {
  front_ultrasonic.time_of_flight_us = sys_time__get_uptime_us() - front_ultrasonic.last_trigger_time_us;
  front_ultrasonic.start_new_reading = true;
}

static void back_ultrasonic_echo_falling_edge_interrupt_handler(void) {
  back_ultrasonic.time_of_flight_us = sys_time__get_uptime_us() - back_ultrasonic.last_trigger_time_us;
  back_ultrasonic.start_new_reading = true;
}

static void left_ultrasonic_echo_falling_edge_interrupt_handler(void) {
  left_ultrasonic.time_of_flight_us = sys_time__get_uptime_us() - left_ultrasonic.last_trigger_time_us;
  left_ultrasonic.start_new_reading = true;
}

static void right_ultrasonic_echo_falling_edge_interrupt_handler(void) {
  right_ultrasonic.time_of_flight_us = sys_time__get_uptime_us() - right_ultrasonic.last_trigger_time_us;
  right_ultrasonic.start_new_reading = true;
}

void ultrasonic_implementation__initialize(void) {
  // Initialize Ultrasonics
  ultrasonic__initialize(&front_ultrasonic, GPIO__PORT_0, 6U, GPIO__PORT_2, 0U); // P0.6 Trigger, P2.0 Echo
  ultrasonic__initialize(&back_ultrasonic, GPIO__PORT_0, 7U, GPIO__PORT_2, 1U);  // P0.7 Trigger, P2.1 Echo
  ultrasonic__initialize(&left_ultrasonic, GPIO__PORT_0, 8U, GPIO__PORT_2, 2U);  // P0.8 Trigger, P2.2 Echo
  ultrasonic__initialize(&right_ultrasonic, GPIO__PORT_0, 9U, GPIO__PORT_2, 4U); // P0.9 Trigger, P2.4 Echo

  // Attach and Initialize Echo falling edge interrupt handlers
  gpio2_interrupts__attach_handler(0U, FALLING, front_ultrasonic_echo_falling_edge_interrupt_handler);
  gpio2_interrupts__attach_handler(1U, FALLING, back_ultrasonic_echo_falling_edge_interrupt_handler);
  gpio2_interrupts__attach_handler(2U, FALLING, left_ultrasonic_echo_falling_edge_interrupt_handler);
  gpio2_interrupts__attach_handler(4U, FALLING, right_ultrasonic_echo_falling_edge_interrupt_handler);
  gpio2_interrupts__initialize();
}

// Will block for (10 us * 4) = 40 us in total
void ultrasonic_implementation__initiate_ultrasonics_range(void) {
  ultrasonic__initiate_range(&front_ultrasonic);
  ultrasonic__initiate_range(&back_ultrasonic);
  ultrasonic__initiate_range(&left_ultrasonic);
  ultrasonic__initiate_range(&right_ultrasonic);
}

uint16_t ultrasonic_implementation__get_front_ultrasonic_distance_in(void) {
  return (front_ultrasonic.time_of_flight_us / 148U);
}

uint16_t ultrasonic_implementation__get_back_ultrasonic_distance_in(void) {
  return (back_ultrasonic.time_of_flight_us / 148U);
}

uint16_t ultrasonic_implementation__get_left_ultrasonic_distance_in(void) {
  return (left_ultrasonic.time_of_flight_us / 148U);
}

uint16_t ultrasonic_implementation__get_right_ultrasonic_distance_in(void) {
  return (right_ultrasonic.time_of_flight_us / 148U);
}

void ultrasonic_implementation__print_ultrasonics_range(void) {
  printf("f: %u b: %u l: %u r: %u\r\n", (unsigned int)ultrasonic_implementation__get_front_ultrasonic_distance_in(),
         (unsigned int)ultrasonic_implementation__get_back_ultrasonic_distance_in(),
         (unsigned int)ultrasonic_implementation__get_left_ultrasonic_distance_in(),
         (unsigned int)ultrasonic_implementation__get_right_ultrasonic_distance_in());
}

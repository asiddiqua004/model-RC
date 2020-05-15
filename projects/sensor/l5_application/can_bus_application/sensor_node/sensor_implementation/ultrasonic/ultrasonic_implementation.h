#pragma once

#include <stdint.h>

void ultrasonic_implementation__initialize(void);

void ultrasonic_implementation__initiate_ultrasonics_range(void);

uint8_t ultrasonic_implementation__get_front_ultrasonic_distance_in(void);

uint8_t ultrasonic_implementation__get_back_ultrasonic_distance_in(void);

uint8_t ultrasonic_implementation__get_left_ultrasonic_distance_in(void);

uint8_t ultrasonic_implementation__get_right_ultrasonic_distance_in(void);

void ultrasonic_implementation__print_ultrasonics_range(void);

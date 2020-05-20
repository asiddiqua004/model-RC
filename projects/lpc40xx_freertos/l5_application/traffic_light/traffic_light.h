/**
 * @file
 * Brief information about the code module
 *
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************************************************************
 *
 *                                                  I N C L U D E S
 *
 **********************************************************************************************************************/
/* Standard Includes */
#include <inttypes.h>
#include <stdbool.h>

/* External Includes */

/* Module Includes */

/***********************************************************************************************************************
 *
 *                                                   D E F I N E S
 *
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *
 *                                                  T Y P E D E F S
 *
 **********************************************************************************************************************/
typedef enum { red, green, yellow } traffic_light_e;

typedef struct {
  uint32_t red_time_in_seconds;
  uint32_t green_time_in_seconds;
  uint32_t yellow_time_in_seconds;
} traffic_time_s;

typedef struct {
  traffic_time_s timings;
  traffic_light_e current_state;
  uint32_t time_for_current_light;
  uint32_t current_light_start_time;
  bool initialized;
  bool current_light_has_started;
} traffic_light_s;

/***********************************************************************************************************************
 *
 *                                     F U N C T I O N   D E C L A R A T I O N S
 *
 **********************************************************************************************************************/

/**
 * Sanity check any inputs, and save timing configuration to traffic_light struct pointer
 *
 * Apply safe defaults for out of bound configuration:
 * - Yellow shall not be less than 3 seconds
 * - Red and Green shall not be less than 10 seconds
 */
void traffic_light__init(traffic_light_s *traffic_light, traffic_time_s traffic_time);

/**
 * Run the logic, and return the traffic signal light status (as enum)
 * - DO NOT create more than one "return" statement in this function
 *
 * @param hz_1_counter is the 1Hz counter, each time this function is called,
 * this may increment which should help you keep track of time for each light
 */
traffic_light_e traffic_light__run(traffic_light_s *traffic_light, uint32_t hz_1_counter);

#ifdef __cplusplus
} /* extern "C" */
#endif

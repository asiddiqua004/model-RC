/***********************************************************************************************************************
 *
 *                                                  I N C L U D E S
 *
 **********************************************************************************************************************/
/* Main Module Header */

#include "traffic_light.h"

/* Standard Includes */
#include <stddef.h>
#include <stdio.h>
/* External Includes */

/* Private Module Includes */

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

/***********************************************************************************************************************
 *
 *                             P R I V A T E   F U N C T I O N   D E C L A R A T I O N S
 *
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *
 *                                  P R I V A T E   D A T A   D E F I N I T I O N S
 *
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *
 *                                         P R I V A T E   F U N C T I O N S
 *
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *
 *                                          P U B L I C   F U N C T I O N S
 *
 **********************************************************************************************************************/

void traffic_light__init(traffic_light_s *traffic_light, traffic_time_s traffic_time) {
  if (NULL != traffic_light && (false == traffic_light->initialized)) {
    traffic_light->timings = traffic_time;
    traffic_light->initialized = true;
  }
}

traffic_light_e traffic_light__run(traffic_light_s *traffic_light, uint32_t hz_1_counter) {
  traffic_light_e current_state = red;
  if (NULL != traffic_light && traffic_light->initialized) {
    switch (traffic_light->current_state) {
    case red: {
      current_state = red;
      if (false == traffic_light->current_light_has_started) {
        traffic_light->current_light_has_started = true;
        traffic_light->current_light_start_time = hz_1_counter;
      }
      const uint32_t time_elapsed_since_light_start = hz_1_counter - traffic_light->current_light_start_time;
      if (time_elapsed_since_light_start >= traffic_light->timings.red_time_in_seconds) {
        traffic_light->current_state = green;
        current_state = green;
        traffic_light->current_light_has_started = false;
      }

      break;
    }
    case green: {
      current_state = green;
      if (false == traffic_light->current_light_has_started) {
        traffic_light->current_light_start_time = hz_1_counter;
        traffic_light->current_light_has_started = true;
      }
      const uint32_t time_elapsed_since_light_start = hz_1_counter - traffic_light->current_light_start_time;
      if (time_elapsed_since_light_start >= traffic_light->timings.green_time_in_seconds) {
        traffic_light->current_state = yellow;
        current_state = yellow;
        traffic_light->current_light_has_started = false;
      }

      break;
    }
    case yellow: {
      current_state = yellow;
      if (false == traffic_light->current_light_has_started) {
        traffic_light->current_light_start_time = hz_1_counter;
        traffic_light->current_light_has_started = true;
      }
      const uint32_t time_elapsed_since_light_start = hz_1_counter - traffic_light->current_light_start_time;
      if (time_elapsed_since_light_start >= traffic_light->timings.yellow_time_in_seconds) {
        traffic_light->current_state = red;
        current_state = red;
        traffic_light->current_light_has_started = false;
      }
      break;
    }
    default:
      break;
    }
  }
  return current_state;
}

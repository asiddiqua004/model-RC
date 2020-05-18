#pragma once

#include <stdbool.h>

#include "gps.h"
#include "tesla_model_rc.h"

/*******************************************************************************
 *
 *                               D E F I N E S
 *
 ******************************************************************************/

/*******************************************************************************
 *
 *                                 E N U M S
 *
 ******************************************************************************/

/*******************************************************************************
 *
 *                               T Y P E D E F S
 *
 ******************************************************************************/
typedef struct {
  float checkpoint_distance_from_car;
  float checkpoint_distance_from_dest;
  size_t checkpoint_index;
} checkpoint_candidates_t;
/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/

void checkpoint__init(gps_coordinates_t origin_coordinates);

void checkpoint__run_once_10Hz(void);

void checkpoint__set_destination_coordinates(gps_coordinates_t destination_coordinates);
void checkpoint__set_current_coordinates(gps_coordinates_t current_coordinates);
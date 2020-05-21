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

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/

bool geological__init(void);

void geological__run_once(void);

void geological__update_destination_coordinates(dbc_BRIDGE_SENSOR_GPS_HEADINGS_s *new_coordinates);

void geological__set_next_point_coordinates(gps_coordinates_t next_point_coordinates);

void geological__set_distance_to_destination(float distance);
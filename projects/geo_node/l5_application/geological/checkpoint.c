#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "checkpoint.h"
#include "geological.h"
#include "gps.h"

/*******************************************************************************
 *
 *               P R I V A T E    D A T A    D E F I N I T I O N S
 *
 ******************************************************************************/
// TODO: set function origin_coordinates, current_coordinates, destination_coordinates; // set this from geological file
static gps_coordinates_t current_coordinates;
static gps_coordinates_t destination_coordinates;
static gps_coordinates_t next_point_coordinates;
static bool is_destination_reached = false;
static bool is_next_point_calculated = false;
static bool is_origin_received = false;

#define checkpoint_diagonal_distance 1.5
#define latitude_offset 0.0005
#define longitude_offset 0.0005

// clang-format off
int checkpoints_array[8][2] ={    {latitude_offset,0},{-latitude_offset,0},{0,longitude_offset},{0,-longitude_offset}, //up and down 
                    {latitude_offset,longitude_offset},{-latitude_offset,longitude_offset},{latitude_offset,longitude_offset},{-latitude_offset,-longitude_offset}, //diagonally
                    };

// clang-format on
static const gps_coordinates_t checkpoints[] = {
    {37.339068, -121.880188}, {37.338957, -121.880413}, {37.338863, -121.880628}, {37.339016, -121.880746},
    //{37.339264, -121.880617},
};
// TODO: implement no entry zone array
/*******************************************************************************
 *
 *                     P R I V A T E    F U N C T I O N S
 *
 ******************************************************************************/

// Reference:
// https://www.tutorialspoint.com/what-is-the-most-effective-way-for-float-and-double-comparison-in-c-cplusplus
static bool checkpoint__private_compare_float(float x, float y) {
  float epsilon = 0.01f;
  if ((float)fabs(x - y) < epsilon)
    return true; // they are same
  return false;  // they are not same
}

static float checkpoint__private_compute_distance_to_destination(gps_coordinates_t current,
                                                                 gps_coordinates_t destination) {
  // Reference: https://www.igismap.com/haversine-formula-calculate-geographic-distance-earth/

  // a = sin²(ΔlatDifference/2) + cos(lat1) * cos(lt2) * sin²(ΔlonDifference/2)
  // c = 2 * atan2(sqrt(a), sqrt(1−a))
  // d = R * c  <- This is the distance computed between two point where R is the radius of earth

  const float R = 6371.0f; // KM
  const float delta_latitute_difference = destination.latitude - current.latitude;
  const float delta_longitude_difference = destination.longitude - current.longitude;

  const float a =
      (float)pow(sinf(delta_latitute_difference / 2.0f), 2.0f) +
      cosf(destination.latitude) * cosf(current.latitude) * (float)pow(sinf(delta_longitude_difference / 2.0f), 2.0f);
  float c = 2 * atan2f(sqrtf(a), sqrtf(1 - a));
  const float d = R * c;

  return d;
}

static bool checkpoint__private_is_next_point_reached(void) {
  bool is_next_point_reached = false;
  if (checkpoint__private_compare_float(current_coordinates.latitude, next_point_coordinates.latitude) &&
      checkpoint__private_compare_float(current_coordinates.longitude, next_point_coordinates.longitude)) {
    if (checkpoint__private_compare_float(next_point_coordinates.latitude, destination_coordinates.latitude) &&
        checkpoint__private_compare_float(next_point_coordinates.longitude, destination_coordinates.longitude)) {
      is_destination_reached = true;
    }
    is_next_point_reached = true;
  }
  return is_next_point_reached;
}

static void checkpoint__private_compute_nearest_checkpoint(void) {

  float min_distance_checkpoint_to_destination = FLT_MAX;
  gps_coordinates_t temp_next_point_coordinates;
  gps_coordinates_t future_next_point_coordinates;

  // TODO: considering no entry zone points
  if ((float)checkpoint_diagonal_distance >
      (float)checkpoint__private_compute_distance_to_destination(next_point_coordinates, destination_coordinates)) {
    future_next_point_coordinates.latitude = destination_coordinates.latitude;
    future_next_point_coordinates.longitude = destination_coordinates.longitude;
  } else { // Generate the points to calculate the distance
    for (size_t i = 0; i < 8; i++) {
      temp_next_point_coordinates.latitude = next_point_coordinates.latitude + checkpoints_array[i][0];
      temp_next_point_coordinates.longitude = next_point_coordinates.longitude + checkpoints_array[i][1];
      float next_point_to_destination_distance =
          checkpoint__private_compute_distance_to_destination(temp_next_point_coordinates, destination_coordinates);
      if (min_distance_checkpoint_to_destination > next_point_to_destination_distance) {
        min_distance_checkpoint_to_destination = next_point_to_destination_distance;
        future_next_point_coordinates.latitude = temp_next_point_coordinates.latitude;
        future_next_point_coordinates.longitude = temp_next_point_coordinates.longitude;
      }
    }
  }
  next_point_coordinates = future_next_point_coordinates;
  is_next_point_calculated = true;
}

static void checkpoint__private_find_next_point(void) {
  if (checkpoint__private_is_next_point_reached()) {
    if (true == is_destination_reached) {
      next_point_coordinates.latitude = 0.0f;
      next_point_coordinates.longitude = 0.0f;
      geological__set_next_point_coordinates(next_point_coordinates);
    }
  } else {
    if (false == is_next_point_calculated) {
      checkpoint__private_compute_nearest_checkpoint();
    } else if (checkpoint__private_is_next_point_reached()) {
      is_next_point_calculated = false;
      geological__set_next_point_coordinates(next_point_coordinates);
    }
  }
}

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/

void checkpoint__init(gps_coordinates_t incoming_origin_coordinates) {
  next_point_coordinates.latitude = incoming_origin_coordinates.latitude;
  next_point_coordinates.longitude = incoming_origin_coordinates.latitude;
}

void checkpoint__run_once_10Hz(void) { checkpoint__private_find_next_point(); }

void checkpoint__set_current_coordinates(gps_coordinates_t incoming_current_coordinates) {
  if (false == is_origin_received) {
    checkpoint__init(current_coordinates);
    is_origin_received = true;
  }
  current_coordinates.latitude = incoming_current_coordinates.latitude;
  current_coordinates.longitude = incoming_current_coordinates.longitude;
}

void checkpoint__set_destination_coordinates(gps_coordinates_t incoming_destination_coordinates) {
  destination_coordinates.latitude = incoming_destination_coordinates.latitude;
  destination_coordinates.longitude = incoming_destination_coordinates.longitude;
}
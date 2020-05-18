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
static float distance_from_car_to_destination = FLT_MAX;
static bool is_destination_reached = false;
static bool is_origin_received = false;
static bool first_point = true;

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define NUMBER_OF_CHECKPOINTS (27U)

#define M_PI 3.14159265359f

static const gps_coordinates_t checkpoints[] = {
    {37.337210f, -121.879911f}, // 1
    {37.337161f, -121.880080f}, // 2
    {37.337104f, -121.880223f}, // 3
    {37.337057f, -121.880312f}, // 4
    {37.336970f, -121.880505f}, // 5
    {37.336888f, -121.880668f}, // 6
    {37.336797f, -121.880850f}, // 7
    {37.336700f, -121.881039f}, // 8
    {37.336609f, -121.881229f}, // 9
    {37.336513f, -121.881433f}, // 10
    {37.336454f, -121.881569f}, // 11
    {37.336375f, -121.881740f}, // 12
    {37.336287f, -121.881887f}, // 13
    {37.336175f, -121.881899f}, // 14
    {37.336010f, -121.881832f}, // 15
    {37.335886f, -121.881738f}, // 16
    {37.335760f, -121.881643f}, // 17
    {37.335636f, -121.881551f}, // 18
    {37.335510f, -121.881457f}, // 19
    {37.335384f, -121.881364f}, // 20
    {37.335262f, -121.881231f}, // 21
    {37.335171f, -121.881165f}, // 22
    {37.335088f, -121.881100f}, // 23
    {37.334988f, -121.881025f}, // 24
    {37.334892f, -121.880953f}, // 25
    {37.334799f, -121.880886f}, // 26
    {37.334690f, -121.880807f}, // 27
};

static checkpoint_candidates_t checkpoint_candidates[NUMBER_OF_CHECKPOINTS] = {0U};
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

  // Convert to Radians
  current.latitude *= (M_PI / 180.0f);
  destination.latitude *= (M_PI / 180.0f);

  const float R = 6371000.0f; // Meters
  const float delta_latitute_difference = (destination.latitude - current.latitude) * (M_PI / 180.0f);
  const float delta_longitude_difference = (destination.longitude - current.longitude) * (M_PI / 180.0f);
  ;

  const float a = sinf(delta_latitute_difference / 2.0f) * sinf(delta_latitute_difference / 2.0f) +
                  cosf(current.latitude) * cosf(destination.latitude) * sinf(delta_longitude_difference / 2.0f) *
                      sinf(delta_longitude_difference / 2.0f);

  // const float a =
  //     (float)pow(sinf(delta_latitute_difference / 2.0f), 2.0f) +
  //     cosf(destination.latitude) * cosf(current.latitude) * (float)pow(sinf(delta_longitude_difference /
  //     2.0f), 2.0f);
  const float c = 2.0f * atan2f(sqrtf(a), sqrtf(1.0f - a));
  const float d = R * c;

  return d;
}

static bool checkpoint__private_is_next_point_reached(void) {
  bool is_next_point_reached = false;
  if (checkpoint__private_compare_float(current_coordinates.latitude, next_point_coordinates.latitude) &&
      checkpoint__private_compare_float(current_coordinates.longitude, next_point_coordinates.longitude)) {
    if (checkpoint__private_compare_float(next_point_coordinates.latitude, destination_coordinates.latitude) &&
        checkpoint__private_compare_float(next_point_coordinates.longitude, destination_coordinates.longitude)) {
      // is_destination_reached = true;
    }
    is_next_point_reached = true;
    printf("============================Next Point Reached!=============================\n"); 
  }
  return is_next_point_reached;
}

static checkpoint_candidates_t get_next_closest_point_to_car(checkpoint_candidates_t checkpoint) {
  checkpoint_candidates_t next_closest_point = checkpoint;
  const size_t previous_checkpoint_index = checkpoint.checkpoint_index;
  float closest_distance_to_car = FLT_MAX;
  for (size_t candidate_iterator = 0U; candidate_iterator < ARRAY_SIZE(checkpoint_candidates); ++candidate_iterator) {
    if (candidate_iterator != previous_checkpoint_index) {
      if (checkpoint_candidates[candidate_iterator].checkpoint_distance_from_car < closest_distance_to_car) {
        closest_distance_to_car = checkpoint_candidates[candidate_iterator].checkpoint_distance_from_car;
        next_closest_point = checkpoint_candidates[candidate_iterator];
      }
    }
  }
  return next_closest_point;
}

static checkpoint_candidates_t get_closest_point_to_car(void) {
  checkpoint_candidates_t closest_point = checkpoint_candidates[0];
  for (size_t candidate_iterator = 0U; candidate_iterator < ARRAY_SIZE(checkpoint_candidates); ++candidate_iterator) {
    if (checkpoint_candidates[candidate_iterator].checkpoint_distance_from_car <
        closest_point.checkpoint_distance_from_car) {
      closest_point = checkpoint_candidates[candidate_iterator];
    }
  }
  return closest_point;
}

static void populate_candidate_checkpoints(void) {
  for (size_t checkpoint_iterator = 0U; checkpoint_iterator < ARRAY_SIZE(checkpoints); ++checkpoint_iterator) {
    const float checkpoint_distance_from_car =
        checkpoint__private_compute_distance_to_destination(current_coordinates, checkpoints[checkpoint_iterator]);
    const float checkpoint_distance_from_dest =
        checkpoint__private_compute_distance_to_destination(checkpoints[checkpoint_iterator], destination_coordinates);
    const checkpoint_candidates_t checkpoint_candidate = {
        .checkpoint_distance_from_car = checkpoint_distance_from_car,
        .checkpoint_distance_from_dest = checkpoint_distance_from_dest,
    };
    checkpoint_candidates[checkpoint_iterator] = checkpoint_candidate;
  }
}

static void checkpoint__private_compute_nearest_checkpoint(void) {
  populate_candidate_checkpoints();
  checkpoint_candidates_t current_candidate = get_closest_point_to_car();
  printf("Current candidate initial: %d\n", current_candidate.checkpoint_index);
  distance_from_car_to_destination =
      checkpoint__private_compute_distance_to_destination(current_coordinates, destination_coordinates);
  printf("distance from car to destination: %f\n", (double)distance_from_car_to_destination);
  bool candidate_found = false;
  for (size_t checkpoint_iterator = 0U; checkpoint_iterator < ARRAY_SIZE(checkpoints); ++checkpoint_iterator) {
    if (current_candidate.checkpoint_distance_from_dest < distance_from_car_to_destination) {
      candidate_found = true;
      break;
    }
    current_candidate = get_next_closest_point_to_car(current_candidate);
  }
  if (!candidate_found) {
    printf("No Candidate Found, using destination\n");
    next_point_coordinates = destination_coordinates;
  } else {
    printf("Candidate Found: using: %u\n", current_candidate.checkpoint_index);
    next_point_coordinates = checkpoints[current_candidate.checkpoint_index];
  }
}

static void checkpoint__private_find_next_point(void) {
  if (first_point) {
    checkpoint__private_compute_nearest_checkpoint();
    geological__set_next_point_coordinates(next_point_coordinates);
    printf("First point: %f, %f\n", (double)next_point_coordinates.latitude, (double)next_point_coordinates.longitude);
    first_point = false;
  } else {
    if (checkpoint__private_is_next_point_reached()) {
      if (is_destination_reached) {
        printf("Destination Reached\n");
        next_point_coordinates.latitude = current_coordinates.latitude;
        next_point_coordinates.longitude = current_coordinates.longitude;
        geological__set_next_point_coordinates(next_point_coordinates);
      } else {
        checkpoint__private_compute_nearest_checkpoint();
        geological__set_next_point_coordinates(next_point_coordinates);
        printf("checkpoint__private_find_next_point else block: %f, %f\n", (double)next_point_coordinates.latitude,
               (double)next_point_coordinates.longitude);
      }
    }
  }
  // if (checkpoint__private_is_next_point_reached()) {
  //   if (true == is_destination_reached) {
  //     next_point_coordinates.latitude = current_coordinates.latitude;
  //     next_point_coordinates.longitude = current_coordinates.longitude;
  //     geological__set_next_point_coordinates(next_point_coordinates);
  //   }
  //   else {
  //     checkpoint__private_compute_nearest_checkpoint();
  //     geological__set_next_point_coordinates(next_point_coordinates);
  //   }
  // } else {
  //   if (false == is_next_point_calculated) {
  //     checkpoint__private_compute_nearest_checkpoint();
  //   } else if (checkpoint__private_is_next_point_reached()) {
  //     is_next_point_calculated = false;
  //     geological__set_next_point_coordinates(next_point_coordinates);
  //   }
  // }
}

static void checkpoint__private_send_distance_to_destination() {
  const dbc_GEO_GPS_DISTANCE_s message = {.GEO_GPS_DISTANCE_TO_DESTINATION = distance_from_car_to_destination};
  printf("Sending Distance to destination: %f\n", (double)distance_from_car_to_destination);
  dbc_encode_and_send_GEO_GPS_DISTANCE(NULL, &message);
}

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/

void checkpoint__init(gps_coordinates_t incoming_origin_coordinates) {
  printf("checkpoint init: %f, %f\n", (double)incoming_origin_coordinates.latitude,
         (double)incoming_origin_coordinates.longitude);
  // next_point_coordinates.latitude = incoming_origin_coordinates.latitude;
  // next_point_coordinates.longitude = incoming_origin_coordinates.latitude;
  next_point_coordinates.latitude = checkpoints[0].latitude;
  next_point_coordinates.longitude = checkpoints[0].longitude;
}

void checkpoint__run_once_10Hz(void) {
  if (gps__get_gps_lock()) {
    checkpoint__private_find_next_point();
    checkpoint__private_send_distance_to_destination();
  }
  printf("run once: current next point coordinates: %f, %f\n", (double)next_point_coordinates.latitude,
         (double)next_point_coordinates.longitude);
  printf("run once: current coordinates: %f, %f\n", (double)current_coordinates.latitude,
         (double)current_coordinates.longitude);
  printf("run once: destination coordinates: %f, %f\n", (double)destination_coordinates.latitude,
         (double)destination_coordinates.longitude);
}

void checkpoint__set_current_coordinates(gps_coordinates_t incoming_current_coordinates) {
  if (gps__get_gps_lock()) {
    if (false == is_origin_received) {
      printf("checkpoint__set_current_coordinates first pass:\n");
      printf("coordinates: %f, %f\n\n", (double)incoming_current_coordinates.latitude,
             (double)incoming_current_coordinates.longitude);
      checkpoint__init(incoming_current_coordinates);
      is_origin_received = true;
    }
    printf("checkpoint__set_current_coordinates other passes: %f, %f\n", (double)incoming_current_coordinates.latitude,
           (double)incoming_current_coordinates.longitude);
    current_coordinates.latitude = incoming_current_coordinates.latitude;
    current_coordinates.longitude = incoming_current_coordinates.longitude;
  }
}

void checkpoint__set_destination_coordinates(gps_coordinates_t incoming_destination_coordinates) {
  if (checkpoint__private_compare_float(incoming_destination_coordinates.latitude, 0.0f) ||
      checkpoint__private_compare_float(incoming_destination_coordinates.longitude, 0.0f)) {
  } else {
    destination_coordinates.latitude = incoming_destination_coordinates.latitude;
    destination_coordinates.longitude = incoming_destination_coordinates.longitude;
  }
}
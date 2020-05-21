#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "can_bus_message_handler.h"
#include "checkpoint.h"
#include "geological.h"
#include "gps.h"

/*******************************************************************************
 *
 *               P R I V A T E    D A T A    D E F I N I T I O N S
 *
 ******************************************************************************/
static gps_coordinates_t current_coordinates;
static gps_coordinates_t destination_coordinates;
static gps_coordinates_t next_point_coordinates;
static float distance_from_car_to_destination = FLT_MAX;
static bool is_destination_reached = false;
static bool is_origin_received = false;
static bool first_point = true;

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define NUMBER_OF_CHECKPOINTS (5U)

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288f
#endif

// static const gps_coordinates_t checkpoints[] = {
//     {37.337210f, -121.879911f}, {37.337161f, -121.880080f}, {37.337104f, -121.880223f}, {37.337057f, -121.880312f},
//     {37.336970f, -121.880505f}, {37.336888f, -121.880668f}, {37.336797f, -121.880850f}, {37.336700f, -121.881039f},
//     {37.336609f, -121.881229f}, {37.336513f, -121.881433f}, {37.336454f, -121.881569f}, {37.336375f, -121.881740f},
//     {37.336287f, -121.881887f}, {37.336175f, -121.881899f}, {37.336010f, -121.881832f}, {37.335886f, -121.881738f},
//     {37.335760f, -121.881643f}, {37.335636f, -121.881551f}, {37.335510f, -121.881457f}, {37.335384f, -121.881364f},
//     {37.335262f, -121.881231f}, {37.335171f, -121.881165f}, {37.335088f, -121.881100f}, {37.334988f, -121.881025f},
//     {37.334892f, -121.880953f}, {37.334799f, -121.880886f}, {37.334690f, -121.880807f},
// };

// static const gps_coordinates_t checkpoints[] = {
//     {37.33725084076829, -121.87995708005087},  {37.33719859281095, -121.88010191933768},
//     {37.33714101338732, -121.88022932426588},  {37.33708341762623, -121.88036881962722},
//     {37.33701944038801, -121.88050158897346},  {37.33695866196124, -121.8806356994242},
//     {37.33689335774423, -121.88075806767745},  {37.33682191631057, -121.88086937935157},
//     {37.336753640891644, -121.88093241126342}, {37.33670992292614, -121.88101421863837},
//     {37.336672602691564, -121.88112878977415}, {37.336629789595804, -121.88123906479467},
//     {37.336580293129316, -121.88134345414517}, {37.33652649535172, -121.88145121111916},
//     {37.33649051147251, -121.88150317441938},  {37.33646065519914, -121.88157022964475},
//     {37.3364233348407, -121.88165606033323},   {37.3363838818702, -121.8817418910217},
//     {37.33633269960732, -121.88184247385976},  {37.33622393718289, -121.88184113275526},
//     {37.33610877679715, -121.88181967508314},  {37.33600894767727, -121.88179520086163},
//     {37.33592257715729, -121.88172009900921},  {37.33584204946044, -121.88167112133641},
//     {37.33575887766558, -121.8816147949471},   {37.335678904699066, -121.88155534941984},
//     {37.33559679903153, -121.88149902303053},  {37.335518772003724, -121.88144459101244},
//     {37.33543453354067, -121.88138155910059},  {37.335352096506284, -121.88132120939775},
//     {37.335287051481075, -121.88126756521746}, {37.33521987377294, -121.88121123882814},
//     {37.33514629811895, -121.88115649963578},  {37.335072722392844, -121.88109749103745},
//     {37.334998080278254, -121.88103580023011}, {37.33491810650174, -121.88097410942277},
//     {37.33484559687077, -121.88091510082444},  {37.33476029133297, -121.88085072780808},
//     {37.33467178673507, -121.88077696706017},
// };

// // Checkpoints on main street SJSU
// static const gps_coordinates_t checkpoints[] = {
//     {37.33591903123678, -121.88177694406716},  // 1
//     {37.33567697997976, -121.88161601152626},  // 2
//     {37.33545945307449, -121.88145105567185},  // 3
//     {37.33529501826172, -121.88133381363366},  // 4
//     {37.33515797935485, -121.88122416723508},  // 5
//     {37.33507747259093, -121.88116113532323},  // 6
//     {37.33495253782028, -121.8810480367332},   // 7
//     {37.334822335717824, -121.88096287659698}, // 8
//     {37.33467442419348, -121.88084485940033},  // 9
// };

// // Checkpoints North Garage SJSU
// static const gps_coordinates_t checkpoints[] = {
//     {37.338774301702564, -121.8806393533834},  // 1
//     {37.33898328892936, -121.88017264901481},  // 2
//     {37.339141095225486, -121.88089684544883}, // 3
//     {37.339313828764105, -121.88044355212531}, // 4
//     {37.33968914965147, -121.88066885768256},  // 5
//     {37.33962304213126, -121.8812160283216},   // 6
//     // Checkpoints on main street SJSU
//     {37.33591903123678, -121.88177694406716},  // 7
//     {37.33567697997976, -121.88161601152626},  // 8
//     {37.33545945307449, -121.88145105567185},  // 9
//     {37.33529501826172, -121.88133381363366},  // 10
//     {37.33515797935485, -121.88122416723508},  // 11
//     {37.33507747259093, -121.88116113532323},  // 12
//     {37.33495253782028, -121.8810480367332},   // 13
//     {37.334822335717824, -121.88096287659698}, // 14
//     {37.33467442419348, -121.88084485940033},  // 15
// };

// Checkpoints North Garage SJSU
static const gps_coordinates_t checkpoints[] = {
    {37.339822f, -121.880966f}, // 1
    {37.339612f, -121.880783f}, // 2
    {37.339418f, -121.880640f}, // 3
    {37.339166f, -121.880458f}, // 4
    {37.338945f, -121.880286f}, // 5
};

static checkpoint_candidates_t checkpoint_candidates[NUMBER_OF_CHECKPOINTS] = {0U};
/*******************************************************************************
 *
 *                     P R I V A T E    F U N C T I O N S
 *
 ******************************************************************************/

// Reference:
// https://www.tutorialspoint.com/what-is-the-most-effective-way-for-float-and-double-comparison-in-c-cplusplus
static bool checkpoint__private_compare_float(float x, float y) {
  float epsilon = 0.0001f;
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

  const float a = sinf(delta_latitute_difference / 2.0f) * sinf(delta_latitute_difference / 2.0f) +
                  cosf(current.latitude) * cosf(destination.latitude) * sinf(delta_longitude_difference / 2.0f) *
                      sinf(delta_longitude_difference / 2.0f);

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
      is_destination_reached = true;
    }
    is_next_point_reached = true;
  }
  return is_next_point_reached;
}

static checkpoint_candidates_t get_next_closest_point_to_car(checkpoint_candidates_t checkpoint) {
  checkpoint_candidates_t next_closest_point = checkpoint;
  float closest_distance_to_car = FLT_MAX;
  for (size_t candidate_iterator = 0U; candidate_iterator < ARRAY_SIZE(checkpoint_candidates); ++candidate_iterator) {
    if (checkpoint_candidates[candidate_iterator].checkpoint_distance_from_car < closest_distance_to_car &&
        !checkpoint_candidates[candidate_iterator].visited) {
      closest_distance_to_car = checkpoint_candidates[candidate_iterator].checkpoint_distance_from_car;
      next_closest_point = checkpoint_candidates[candidate_iterator];
    }
  }
  return next_closest_point;
}

static checkpoint_candidates_t get_closest_point_to_car(void) {
  checkpoint_candidates_t closest_point = checkpoint_candidates[0];
  for (size_t candidate_iterator = 0U; candidate_iterator < ARRAY_SIZE(checkpoint_candidates); ++candidate_iterator) {
    if (checkpoint_candidates[candidate_iterator].checkpoint_distance_from_car <
            closest_point.checkpoint_distance_from_car &&
        !checkpoint_candidates[candidate_iterator].visited) {
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
        .checkpoint_index = checkpoint_iterator,
    };
    checkpoint_candidates[checkpoint_iterator] = checkpoint_candidate;
  }
}

static void checkpoint__private_compute_nearest_checkpoint(void) {
  populate_candidate_checkpoints();
  checkpoint_candidates_t current_candidate = get_closest_point_to_car();
  distance_from_car_to_destination =
      checkpoint__private_compute_distance_to_destination(current_coordinates, destination_coordinates);
  bool candidate_found = false;
  for (size_t checkpoint_iterator = 0U; checkpoint_iterator < ARRAY_SIZE(checkpoints); ++checkpoint_iterator) {
    if (current_candidate.checkpoint_distance_from_dest < distance_from_car_to_destination &&
        !checkpoint_candidates[checkpoint_iterator].visited) {
      candidate_found = true;
      break;
    }
    current_candidate = get_next_closest_point_to_car(current_candidate);
  }
  if (!candidate_found) {
    next_point_coordinates = destination_coordinates;
  } else {
    next_point_coordinates = checkpoints[current_candidate.checkpoint_index];
    checkpoint_candidates[current_candidate.checkpoint_index].visited = true;
    const dbc_GEO_GPS_COMPASS_DBG_s message = {.GEO_GPS_NEXT_POINT_INDEX = current_candidate.checkpoint_index};
    dbc_encode_and_send_GEO_GPS_COMPASS_DBG(NULL, &message);
  }
}

static void checkpoint__private_find_next_point(void) {
  if (first_point) {
    checkpoint__private_compute_nearest_checkpoint();
    geological__set_next_point_coordinates(next_point_coordinates);
    first_point = false;
  } else {
    if (checkpoint__private_is_next_point_reached()) {
      if (is_destination_reached) {
        next_point_coordinates.latitude = current_coordinates.latitude;
        next_point_coordinates.longitude = current_coordinates.longitude;
        geological__set_next_point_coordinates(next_point_coordinates);
      } else {
        checkpoint__private_compute_nearest_checkpoint();
        geological__set_next_point_coordinates(next_point_coordinates);
      }
    }
  }
}

static void checkpoint__private_send_distance_to_destination() {
  const dbc_GEO_GPS_DISTANCE_s message = {.GEO_GPS_DISTANCE_TO_DESTINATION = distance_from_car_to_destination};
  dbc_encode_and_send_GEO_GPS_DISTANCE(NULL, &message);
}

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/

void checkpoint__init(gps_coordinates_t incoming_origin_coordinates) {
  next_point_coordinates.latitude = checkpoints[0].latitude;
  next_point_coordinates.longitude = checkpoints[0].longitude;
  geological__set_next_point_coordinates(next_point_coordinates);
}

void checkpoint__run_once_10Hz(void) {
  if (gps__get_gps_lock()) {
    checkpoint__private_find_next_point();
    checkpoint__private_send_distance_to_destination();
  }
}

void checkpoint__set_current_coordinates(gps_coordinates_t incoming_current_coordinates) {
  if (gps__get_gps_lock()) {
    if (false == is_origin_received) {
      checkpoint__init(incoming_current_coordinates);
      is_origin_received = true;
    }
    current_coordinates.latitude = incoming_current_coordinates.latitude;
    current_coordinates.longitude = incoming_current_coordinates.longitude;
  }
}

void checkpoint__set_destination_coordinates(gps_coordinates_t incoming_destination_coordinates) {
  if (checkpoint__private_compare_float(incoming_destination_coordinates.latitude, 0.0f) ||
      checkpoint__private_compare_float(incoming_destination_coordinates.longitude, 0.0f)) {
  } else {
    for (size_t checkpoints_iterator = 0; checkpoints_iterator < NUMBER_OF_CHECKPOINTS; checkpoints_iterator++) {
      checkpoint_candidates[checkpoints_iterator].visited = false;
    }

    destination_coordinates.latitude = incoming_destination_coordinates.latitude;
    destination_coordinates.longitude = incoming_destination_coordinates.longitude;
  }
}

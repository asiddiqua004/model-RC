#include <math.h>

#include "geological.h"
#include "tesla_model_rc.h"
#include <stdio.h>

// #define USING_SIMULATED_HW 1
#ifndef USING_SIMULATED_HW
#define USING_SIMULATED_HW 0
#endif

#if USING_SIMULATED_HW
#include <math.h>
#include <stdlib.h>
static uint16_t simulated_trajectory_index;
static const gps_coordinates_t simulated_trajectory[] = {
    {.latitude = 37.338383f, .longitude = 121.880760f}, {.latitude = 37.338408f, .longitude = 121.880793f},
    {.latitude = 37.338517f, .longitude = 121.880871f}, {.latitude = 37.338570f, .longitude = 121.880925f},
    {.latitude = 37.338587f, .longitude = 121.880952f}, {.latitude = 37.338640f, .longitude = 121.880960f},
    {.latitude = 37.338659f, .longitude = 121.881000f}, {.latitude = 37.338697f, .longitude = 121.881027f},
    {.latitude = 37.338697f, .longitude = 121.881027f}, {.latitude = 37.338746f, .longitude = 121.881073f},
    {.latitude = 37.338804f, .longitude = 121.881108f}, {.latitude = 37.338864f, .longitude = 121.881151f},
    {.latitude = 37.338911f, .longitude = 121.881175f}, {.latitude = 37.338954f, .longitude = 121.881202f},
    {.latitude = 37.339022f, .longitude = 121.881272f}};
#else
#include "compass.h"
#include "gps.h"
#endif

/*******************************************************************************
 *
 *               P R I V A T E    D A T A    D E F I N I T I O N S
 *
 ******************************************************************************/

static float compass_heading;
static gps_coordinates_t current_coordinates;
static gps_coordinates_t destination_coordinates;
#define M_PI 3.14159265358979323846264338327950288

/*******************************************************************************
 *
 *                     P R I V A T E    F U N C T I O N S
 *
 ******************************************************************************/
#if USING_SIMULATED_HW
static float random_between_two_floats(float min, float max) {
  return (min + 1) + (((float)rand()) / (float)RAND_MAX) * (max - (min + 1));
}
#endif

static float geological__private_compute_heading_degree(void) {

  // Reference: https://www.igismap.com/formula-to-find-bearing-or-heading-angle-between-two-points-latitude-longitude/

  // X = cos θb * sin ∆L
  // Y = cos θa * sin θb – sin θa * cos θb * cos ∆L

  // β = atan2(X,Y) <- This is the angle towards our desired destination in radians.
  // This then needs to be converted to degrees -> β * π / 180

  const float theta_b = destination_coordinates.latitude;
  const float delta_L = destination_coordinates.longitude - current_coordinates.longitude;
  // const float delta_L = destination_coordinates.longitude > current_coordinates.longitude
  //                           ? destination_coordinates.longitude - current_coordinates.longitude
  //                           : current_coordinates.longitude - destination_coordinates.longitude;
  const float theta_a = current_coordinates.latitude;

  const float X = cosf(theta_b) * sinf(delta_L);
  const float Y = cosf(theta_a) * sinf(theta_b) - sinf(theta_a) * cosf(theta_b) * cosf(delta_L);
  float beta = atan2f(X, Y);
  if (beta < 0.0f) {
    beta += 2.0f * (float)M_PI;
  }
  const float beta_degrees = beta * (180.0f / (float)M_PI);

  // targetHeading = fmodf(to_degree(atan2(x,y)) + 360, 360)
  // const float adjusted_heading = beta_degrees - compass_heading;
  printf("beta: %f\nbeta_degrees: %f\n\n", (double)beta, (double)beta_degrees);
  printf("destination coordinates: lat: %f, long: %f\n\n", (double)destination_coordinates.latitude,
         (double)destination_coordinates.longitude);
  printf("current coordinates: lat: %f, long: %f\n\n", (double)current_coordinates.latitude,
         (double)current_coordinates.longitude);
  return beta_degrees;
}

#if USING_SIMULATED_HW == 0
static void geological__private_handle_compass() { compass_heading = compass__get_heading_degrees(); }

static void geological__private_handle_gps() { current_coordinates = gps__get_coordinates(); }
#endif

static void geological__private_compute_and_send_heading() {
  const float destination_heading = geological__private_compute_heading_degree();
  const dbc_GEO_GPS_HEADINGS_s message = {.GEO_GPS_HEADINGS_CURRENT = compass_heading,
                                          .GEO_GPS_HEADINGS_DESTINATION = destination_heading};
  dbc_encode_and_send_GEO_GPS_HEADINGS(NULL, &message);
}

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/

bool geological__init(void) {
#if USING_SIMULATED_HW
  return true;
#else
  gps__init();
  return compass__init();
#endif
}

void geological__run_once(void) {
#if USING_SIMULATED_HW
  current_coordinates = simulated_trajectory[simulated_trajectory_index];
  printf("simulated_trajectory[%u] = %f, %f\n\n", simulated_trajectory_index,
         (double)simulated_trajectory[simulated_trajectory_index].latitude,
         (double)simulated_trajectory[simulated_trajectory_index].longitude);
  simulated_trajectory_index = (simulated_trajectory_index + 1) % 15;
  compass_heading = fmodf((330.0f + random_between_two_floats(0.0f, 60.0f)), 360.0f);
#else
  gps__run_once();

  geological__private_handle_compass();
  geological__private_handle_gps();
#endif

  geological__private_compute_and_send_heading();
}

void geological__update_destination_coordinates(dbc_BRIDGE_GPS_HEADINGS_s *new_coordinates) {
  destination_coordinates.longitude = new_coordinates->BRIDGE_GPS_HEADINGS_LONGITUDE;
  destination_coordinates.latitude = new_coordinates->BRIDGE_GPS_HEADINGS_LATITUDE;
}

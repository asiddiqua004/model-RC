#include <float.h>
#include <math.h>
#include <stdio.h>

#include "can_bus_initializer.h"
#include "can_bus_message_handler.h"
#include "checkpoint.h"
#include "compass.h"
#include "geological.h"
#include "gps.h"

/*******************************************************************************
 *
 *               P R I V A T E    D A T A    D E F I N I T I O N S
 *
 ******************************************************************************/

static float compass_heading;
static gps_coordinates_t current_coordinates;
static gps_coordinates_t next_point_coordinates;
static gps_coordinates_t destination_coordinates;
static float distance_to_destination = FLT_MAX;
#define M_PI 3.14159265358979323846264338327950288f

/*******************************************************************************
 *
 *                     P R I V A T E    F U N C T I O N S
 *
 ******************************************************************************/

static float geological__private_compute_heading_degree(void) {

  // Reference: https://www.igismap.com/formula-to-find-bearing-or-heading-angle-between-two-points-latitude-longitude/

  // X = cos θb * sin ∆L
  // Y = cos θa * sin θb – sin θa * cos θb * cos ∆L

  // β = atan2(X,Y) <- This is the angle towards our desired destination in radians.
  // This then needs to be converted to degrees -> β * π / 180

  const float theta_b = next_point_coordinates.latitude;
  const float delta_L = next_point_coordinates.longitude - current_coordinates.longitude;
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
  const dbc_GEO_GPS_COMPASS_DBG_2_s message = {.GEO_GPS_HEADINGS_LATITUDE_INT = next_point_coordinates.latitude,
                                               .GEO_GPS_HEADINGS_LONGITUDE_INT = next_point_coordinates.longitude};
  dbc_encode_and_send_GEO_GPS_COMPASS_DBG_2(NULL, &message);
  const dbc_GEO_GPS_COMPASS_DBG_3_s message1 = {.GEO_GPS_CURRENT_LATITUDE_INT = current_coordinates.latitude,
                                                .GEO_GPS_CURRENT_LONGITUDE_INT = current_coordinates.longitude};
  dbc_encode_and_send_GEO_GPS_COMPASS_DBG_3(NULL, &message1);
  const dbc_GEO_GPS_COMPASS_DBG_s message2 = {.GEO_GPS_BETA = beta_degrees};
  dbc_encode_and_send_GEO_GPS_COMPASS_DBG(NULL, &message2);
  printf("Destination Heading degree: %f\n", (double)beta_degrees);
  return beta_degrees;
}

static void geological__private_handle_compass() {
  compass_heading = compass__get_heading_degrees();
  printf("Compass heading degree: %f\n", (double)compass_heading);
}

static void geological__private_handle_gps() {
  current_coordinates = gps__get_coordinates();
  printf("Latitude: %f  Longitude: %f\n", (double)current_coordinates.latitude, (double)current_coordinates.longitude);
}

static void geological__private_compute_and_send_heading() {
  const float destination_heading = geological__private_compute_heading_degree();
  const dbc_GEO_GPS_COMPASS_HEADINGS_s message = {.GEO_GPS_COMPASS_HEADINGS_CURRENT = compass_heading,
                                                  .GEO_GPS_COMPASS_HEADINGS_DESTINATION = destination_heading};
  dbc_encode_and_send_GEO_GPS_COMPASS_HEADINGS(NULL, &message);
}

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/

bool geological__init(void) {
  gps__init();
  return compass__init();
}

void geological__run_once(void) {
  gps__run_once();
  geological__private_handle_compass();
  geological__private_handle_gps();
  checkpoint__set_current_coordinates(current_coordinates);
  checkpoint__run_once_10Hz();
  geological__private_compute_and_send_heading();
}

void geological__update_destination_coordinates(dbc_BRIDGE_SENSOR_GPS_HEADINGS_s *new_coordinates) {
  const dbc_GEO_GPS_COMPASS_DBG_4_s message = {
      .GEO_GPS_RECEIVED_LATITUDE = new_coordinates->BRIDGE_SENSOR_GPS_HEADINGS_LATITUDE,
      .GEO_GPS_RECEIVED_LONGITUDE = new_coordinates->BRIDGE_SENSOR_GPS_HEADINGS_LONGITUDE,
  };
  dbc_encode_and_send_GEO_GPS_COMPASS_DBG_4(NULL, &message);
  destination_coordinates.longitude = new_coordinates->BRIDGE_SENSOR_GPS_HEADINGS_LONGITUDE;
  if (destination_coordinates.longitude > 1000.0f) {
    destination_coordinates.longitude -= 1000.0f;
    destination_coordinates.longitude = -destination_coordinates.longitude;
  }
  destination_coordinates.latitude = new_coordinates->BRIDGE_SENSOR_GPS_HEADINGS_LATITUDE;
  if (destination_coordinates.latitude > 1000.0f) {
    destination_coordinates.latitude -= 1000.0f;
    destination_coordinates.latitude = -destination_coordinates.latitude;
  }
  checkpoint__set_destination_coordinates(destination_coordinates);
}

void geological__set_next_point_coordinates(gps_coordinates_t incoming_next_point_coordinates) {
  next_point_coordinates.latitude = incoming_next_point_coordinates.latitude;
  next_point_coordinates.longitude = incoming_next_point_coordinates.longitude;
}

void geological__set_distance_to_destination(float distance) { distance_to_destination = distance; }
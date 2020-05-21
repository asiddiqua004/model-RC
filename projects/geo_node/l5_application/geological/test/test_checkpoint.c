#include "unity.h"

// Mocks
#include "Mockcan_bus_message_handler.h"
#include "Mockcompass.h"
#include "Mockgeological.h"
#include "Mockgps.h"

// Module includes
#include "checkpoint.c"

#include "tesla_model_rc.h"

#include "can_bus_mia_configurations.h"

/******************************************************************************
 *
 *                               D E F I N E S
 *
 ******************************************************************************/

/*****************************************************************************
 *
 *                                 E N U M S
 *
 ******************************************************************************/

/******************************************************************************
 *
 *                              T Y P E D E F S
 *
 ******************************************************************************/

/******************************************************************************
 *
 *        P R I V A T E    F U N C T I O N    D E C L A R A T I O N S
 *
 ******************************************************************************/

/******************************************************************************
 *
 *             P R I V A T E    D A T A    D E F I N I T I O N S
 *
 *****************************************************************************/

static const gps_coordinates_t test_checkpoints[] = {
    {37.339822f, -121.880966f}, // 1
    {37.339612f, -121.880783f}, // 2
    {37.339418f, -121.880640f}, // 3
    {37.339166f, -121.880458f}, // 4
    {37.338945f, -121.880286f}, // 5
};

/******************************************************************************
 *
 *                     P R I V A T E    F U N C T I O N S
 *
 *****************************************************************************/

/******************************************************************************
 *
 *                T E S T    S E T U P    &    T E A R D O W N
 *
 *****************************************************************************/
void setUp(void) {}

void tearDown(void) {}

/******************************************************************************
 *
 *                                 T E S T S
 *
 *****************************************************************************/

void test_checkpoint__init(void) {
  gps_coordinates_t next__point_coordinates = {};
  geological__set_next_point_coordinates_ExpectAnyArgs();
  checkpoint__init(next_point_coordinates);
  TEST_ASSERT_EQUAL_FLOAT(test_checkpoints[0].latitude, next_point_coordinates.latitude);
  TEST_ASSERT_EQUAL_FLOAT(test_checkpoints[0].longitude, next_point_coordinates.longitude);
}

void test_checkpoint__run_once_10Hz(void) {
  gps_coordinates_t next_point_coordinates = {};

  gps__get_gps_lock_ExpectAndReturn(true);
  geological__set_next_point_coordinates_ExpectAnyArgs();
  dbc_send_can_message_ExpectAnyArgsAndReturn(true);
  checkpoint__run_once_10Hz();

  gps__get_gps_lock_ExpectAndReturn(false);
  checkpoint__run_once_10Hz();
}

void test_checkpoint__set_current_coordinates(void) {
  gps_coordinates_t incoming_current_coordinates = {.latitude = 37.339612f, .longitude = -121.880783f};
  // Case when we have a GPS lock and first point origin is recevied
  gps__get_gps_lock_ExpectAndReturn(true);
  geological__set_next_point_coordinates_ExpectAnyArgs();
  checkpoint__set_current_coordinates(incoming_current_coordinates);
  TEST_ASSERT_EQUAL_FLOAT(test_checkpoints[0].latitude, next_point_coordinates.latitude);
  TEST_ASSERT_EQUAL_FLOAT(test_checkpoints[0].longitude, next_point_coordinates.longitude);

  // Case when we have a GPS lock and it's not the first point
  gps__get_gps_lock_ExpectAndReturn(true);
  checkpoint__set_current_coordinates(incoming_current_coordinates);
  TEST_ASSERT_EQUAL_FLOAT(incoming_current_coordinates.latitude, current_coordinates.latitude);
  TEST_ASSERT_EQUAL_FLOAT(incoming_current_coordinates.longitude, current_coordinates.longitude);

  // Case when we don't have a GPS lock and it's not the first point
  gps__get_gps_lock_ExpectAndReturn(false);
  incoming_current_coordinates.latitude = 37.339418f;
  incoming_current_coordinates.longitude = -121.880640f;
  checkpoint__set_current_coordinates(incoming_current_coordinates);
  TEST_ASSERT_EQUAL_FLOAT(37.339612f, current_coordinates.latitude);
  TEST_ASSERT_EQUAL_FLOAT(-121.880783f, current_coordinates.longitude);
}

void test_checkpoint__set_destination_coordinates(void) {
  // Valid destination for our test location
  gps_coordinates_t incoming_destination_coordinates = {.latitude = 37.339612f, .longitude = -121.880783f};
  checkpoint__set_destination_coordinates(incoming_destination_coordinates);
  TEST_ASSERT_EQUAL_FLOAT(37.339612f, destination_coordinates.latitude);
  TEST_ASSERT_EQUAL_FLOAT(-121.880783f, destination_coordinates.longitude);

  // Invalid latitude destination for our test location
  incoming_destination_coordinates.latitude = 0.0f;
  incoming_destination_coordinates.longitude = -121.880783f;
  checkpoint__set_destination_coordinates(incoming_destination_coordinates);
  TEST_ASSERT_EQUAL_FLOAT(37.339612f, destination_coordinates.latitude);
  TEST_ASSERT_EQUAL_FLOAT(-121.880783f, destination_coordinates.longitude);

  // New valid destination for our test location
  incoming_destination_coordinates.latitude = 37.339418f;
  incoming_destination_coordinates.longitude = -121.880640f;
  checkpoint__set_destination_coordinates(incoming_destination_coordinates);
  TEST_ASSERT_EQUAL_FLOAT(37.339418f, destination_coordinates.latitude);
  TEST_ASSERT_EQUAL_FLOAT(-121.880640f, destination_coordinates.longitude);

  // Invalid destination longitude for our test location
  incoming_destination_coordinates.latitude = 37.339418f;
  incoming_destination_coordinates.longitude = 0.0f;
  checkpoint__set_destination_coordinates(incoming_destination_coordinates);
  TEST_ASSERT_EQUAL_FLOAT(37.339418f, destination_coordinates.latitude);
  TEST_ASSERT_EQUAL_FLOAT(-121.880640f, destination_coordinates.longitude);

  // Invalid destination for our test location
  incoming_destination_coordinates.latitude = 0.0f;
  incoming_destination_coordinates.longitude = 0.0f;
  checkpoint__set_destination_coordinates(incoming_destination_coordinates);
  TEST_ASSERT_EQUAL_FLOAT(37.339418f, destination_coordinates.latitude);
  TEST_ASSERT_EQUAL_FLOAT(-121.880640f, destination_coordinates.longitude);
}

void test_checkpoint__private_compute_distance_to_destination(void) {
  gps_coordinates_t current = {.latitude = 37.339822f, .longitude = -121.880966f};
  gps_coordinates_t destination = {.latitude = 37.339612f, .longitude = -121.880783f};
  const float distance = checkpoint__private_compute_distance_to_destination(current, destination);
  TEST_ASSERT_EQUAL_FLOAT(16.1928f, distance);
}

void test_checkpoint__private_is_next_point_reached(void) {
  current_coordinates.latitude = 37.339822f;
  current_coordinates.longitude = -121.880966f;
  next_point_coordinates.latitude = 37.339612f;
  next_point_coordinates.longitude = -121.880783f;
  bool next_point_reached_status = checkpoint__private_is_next_point_reached();
  TEST_ASSERT_FALSE(next_point_reached_status);

  current_coordinates.latitude = 37.339822f;
  current_coordinates.longitude = -121.880966f;
  next_point_coordinates.latitude = 37.339822f;
  next_point_coordinates.longitude = -121.880966f;
  next_point_reached_status = checkpoint__private_is_next_point_reached();
  TEST_ASSERT_TRUE(next_point_reached_status);
}

void test_get_next_closest_point_to_car(void) {}

void test_get_closest_point_to_car(void) {}

void test_populate_candidate_checkpoints(void) { populate_candidate_checkpoints(); }

void test_checkpoint__private_compute_nearest_checkpoint(void) { checkpoint__private_compute_nearest_checkpoint(); }

void test_checkpoint__private_find_next_point(void) { checkpoint__private_find_next_point(); }

void test_checkpoint__private_send_distance_to_destination(void) {
  dbc_send_can_message_ExpectAnyArgsAndReturn(true);
  checkpoint__private_send_distance_to_destination();
}
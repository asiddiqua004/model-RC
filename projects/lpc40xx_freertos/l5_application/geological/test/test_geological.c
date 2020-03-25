#include "unity.h"

// Mocks
#include "Mockcan_bus_message_handler.h"
#include "Mockcompass.h"
#include "Mockgps.h"

// Module includes
#include "tesla_model_rc.h"

#include "geological.c"

#include "can_bus_mia_configurations.h"

#undef USING_SIMULATED_HW
#define USING_SIMULATED_HW 0

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

void test_geological_init(void) {
  gps__init_Expect();
  compass__init_ExpectAndReturn(true);
  geological__init();
}

void test_geological_run_once(void) {
  const gps_coordinates_t coordinates = {.latitude = 123.456f, .longitude = 789.1011f};
  gps__run_once_Expect();
  compass__get_heading_degrees_ExpectAndReturn(0.0f);
  gps__get_coordinates_ExpectAndReturn(coordinates);
  dbc_send_can_message_ExpectAnyArgsAndReturn(true);
  geological__run_once();
}

void test_geological__update_destination_coordinates(void) {
  dbc_BRIDGE_GPS_HEADINGS_s message = {.BRIDGE_GPS_HEADINGS_LONGITUDE = 1.0f, .BRIDGE_GPS_HEADINGS_LATITUDE = 2.0f};
  geological__update_destination_coordinates(&message);
  TEST_ASSERT_EQUAL(message.BRIDGE_GPS_HEADINGS_LATITUDE, destination_coordinates.latitude);
  TEST_ASSERT_EQUAL(message.BRIDGE_GPS_HEADINGS_LONGITUDE, destination_coordinates.longitude);
}

void test_geological__private_handle_compass(void) {
  compass__get_heading_degrees_ExpectAndReturn(0.0f);
  geological__private_handle_compass();
}

void test_geological__private_handle_gps(void) {
  const gps_coordinates_t coordinates = {.latitude = 123.456f, .longitude = 789.1011f};
  gps__get_coordinates_ExpectAndReturn(coordinates);
  geological__private_handle_gps();
}

//TODO: Revise Algorithm in geological__private_compute_heading_degree and retest

#include "unity.h"

#include <stdio.h>
#include <string.h>

// Mocks
#include "Mockgpio.h"
#include "Mocki2c.h"

// Module includes
#include "compass.c"

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

void test_compass__init(void) {
  uint8_t gain = {0U};
  uint8_t mode = {0U};

  i2c__read_single_ExpectAndReturn(COMPASS__BUS, COMPASS__ADDRESS, COMPASS__MEMORY_ID_REG_A, 1);
  i2c__write_single_ExpectAnyArgsAndReturn(gain);
  compass__set_gain(COMPASS__GAIN_1090);
  i2c__write_single_ExpectAnyArgsAndReturn(mode);
  compass__set_mode(COMPASS__MODE_SINGLE);
  compass__init();
}

void test_compass__get_heading_degrees(void) {
  compass__axis_data_t axis_data = {.x = 100.0f, .y = -350.0f};
  i2c__read_slave_data_ExpectAnyArgsAndReturn(1);
  i2c__write_single_ExpectAnyArgsAndReturn(1);

  const float compass_heading_degrees = compass__private_compute_heading(axis_data) * 180.0f / (float)M_PI;
  TEST_ASSERT_EQUAL(299.123427, compass_heading_degrees);
  compass__get_heading_degrees();
}
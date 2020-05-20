/***********************************************************************************************************************
 *
 *                                                  I N C L U D E S
 *
 **********************************************************************************************************************/
/* Standard Includes */
#include "unity.h"

/* Mock Includes */

/* External Includes */

/* Module Includes */

#include "traffic_light.c"

/***********************************************************************************************************************
 *
 *                                                   D E F I N E S
 *
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *
 *                                                  T Y P E D E F S
 *
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *
 *                             P R I V A T E   F U N C T I O N   D E C L A R A T I O N S
 *
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *
 *                                  P R I V A T E   D A T A   D E F I N I T I O N S
 *
 **********************************************************************************************************************/
static traffic_light_s traffic_light_struct;
static const traffic_time_s traffic_times = {
    .red_time_in_seconds = 4, .green_time_in_seconds = 2, .yellow_time_in_seconds = 1};
static const traffic_time_s traffic_times_2 = {
    .red_time_in_seconds = 3, .green_time_in_seconds = 1, .yellow_time_in_seconds = 1};
/***********************************************************************************************************************
 *
 *                                         P R I V A T E   F U N C T I O N S
 *
 **********************************************************************************************************************/
#if 0
static void print_stats(void) {
  printf("state: %u\n", (uint32_t)traffic_light_struct.current_state);
  printf("time_for_current_light: %u\n", traffic_light_struct.time_for_current_light);
  printf("current_light_start_time: %u\n", traffic_light_struct.current_light_start_time);
  printf("current_light_has_started: %u\n\n\n", traffic_light_struct.current_light_has_started);
}
#endif
/***********************************************************************************************************************
 *
 *                                     T E S T   S E T U P   &   T E A R D O W N
 *
 **********************************************************************************************************************/
void setUp(void) {}

void tearDown(void) {}

/***********************************************************************************************************************
 *
 *                                                     T E S T S
 *
 **********************************************************************************************************************/

void test_traffic_light__init(void) {
  // NULL traffic_light_s
  traffic_light__init(NULL, traffic_times);

  // Successful Init
  traffic_light__init(&traffic_light_struct, traffic_times);
  TEST_ASSERT_TRUE(traffic_light_struct.initialized);
  TEST_ASSERT_EQUAL_UINT32(traffic_light_struct.timings.green_time_in_seconds, traffic_times.green_time_in_seconds);
  TEST_ASSERT_EQUAL_UINT32(traffic_light_struct.timings.red_time_in_seconds, traffic_times.red_time_in_seconds);
  TEST_ASSERT_EQUAL_UINT32(traffic_light_struct.timings.yellow_time_in_seconds, traffic_times.yellow_time_in_seconds);

  // Initializing again should fail
  traffic_light__init(&traffic_light_struct, traffic_times_2);
  TEST_ASSERT_TRUE(traffic_light_struct.initialized);
  TEST_ASSERT_EQUAL_UINT32(traffic_light_struct.timings.green_time_in_seconds, traffic_times.green_time_in_seconds);
  TEST_ASSERT_EQUAL_UINT32(traffic_light_struct.timings.red_time_in_seconds, traffic_times.red_time_in_seconds);
  TEST_ASSERT_EQUAL_UINT32(traffic_light_struct.timings.yellow_time_in_seconds, traffic_times.yellow_time_in_seconds);
}

void test_traffic_light__run(void) {
  traffic_light__init(&traffic_light_struct, traffic_times);
  TEST_ASSERT_EQUAL(traffic_light_struct.current_state, red);

  // Increment count until we reach green
  traffic_light_e current_state = traffic_light__run(&traffic_light_struct, 0);
  TEST_ASSERT_EQUAL(red, current_state);

  current_state = traffic_light__run(&traffic_light_struct, 1);
  TEST_ASSERT_EQUAL(red, current_state);

  current_state = traffic_light__run(&traffic_light_struct, 2);
  TEST_ASSERT_EQUAL(red, current_state);

  current_state = traffic_light__run(&traffic_light_struct, 3);
  TEST_ASSERT_EQUAL(red, current_state);

  // Incremement Count until we reach yellow
  current_state = traffic_light__run(&traffic_light_struct, 4);
  TEST_ASSERT_EQUAL(green, current_state);

  current_state = traffic_light__run(&traffic_light_struct, 5);
  TEST_ASSERT_EQUAL(green, current_state);

  current_state = traffic_light__run(&traffic_light_struct, 6);
  TEST_ASSERT_EQUAL(green, current_state);

  // Increment Count until we reach red again
  current_state = traffic_light__run(&traffic_light_struct, 7);
  TEST_ASSERT_EQUAL(yellow, current_state);

  current_state = traffic_light__run(&traffic_light_struct, 8);
  TEST_ASSERT_EQUAL(yellow, current_state);

  // Incrementing one more should lead us back to red
  current_state = traffic_light__run(&traffic_light_struct, 9);
  TEST_ASSERT_EQUAL(red, current_state);

  current_state = traffic_light__run(&traffic_light_struct, 10);
  TEST_ASSERT_EQUAL(red, current_state);
}

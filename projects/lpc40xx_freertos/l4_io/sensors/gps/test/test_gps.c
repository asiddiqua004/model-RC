
#include "unity.h"

#include <stdio.h>
#include <string.h>

// Mocks
#include "Mockclock.h"
#include "Mockgpio.h"
#include "Mockqueue.h"
#include "Mockuart.h"

// Module includes
#include "gps.c"

#include "line_buffer.h"

/******************************************************************************
 *
 *                               D E F I N E S
 *
 ******************************************************************************/
static const uart_e gps_uart_test = UART__3;
static const size_t clock_rate_test = 96000000UL;
static const size_t baud_rate_test = 9600U;

#define UNUSED(x) (void)(x)
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

static const char *gsg_string_negative = "$GPGGA,230612.015,1234.5678,S,12102.4634,W,0,04,5.7,508.3,M,,,,0000*04\r\n";
static const char *gsg_string_positive = "$GPGGA,230612.015,1234.5678,N,12102.4634,E,0,04,5.7,508.3,M,,,,0000*0B\r\n";
static const char *gsg_string_checksum_fail = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*48\r\n";
static const gps_coordinates_t parsed_coordinates_test_negative = {.latitude = -1234.5678f, .longitude = -12102.4634f};
static const gps_coordinates_t parsed_coordinates_test_positive = {.latitude = 1234.5678f, .longitude = 12102.4634f};

/******************************************************************************
 *
 *                     P R I V A T E    F U N C T I O N S
 *
 *****************************************************************************/
static bool uart__get_stub_callback_negative(uart_e uart, char *input_byte, uint32_t timeout_ms, int call_count) {
  UNUSED(uart);
  UNUSED(timeout_ms);
  if (call_count <= strlen(gsg_string_negative)) {
    *input_byte = gsg_string_negative[call_count];
    return true;
  } else if (strlen(gsg_string_negative) + 1 == call_count) {
    return false;
  } else {
    TEST_FAIL_MESSAGE("uart__get_stub_callback_negative called too many times");
    return false;
  }
}

static bool uart__get_stub_callback_positive(uart_e uart, char *input_byte, uint32_t timeout_ms, int call_count) {
  UNUSED(uart);
  UNUSED(timeout_ms);
  if (call_count <= strlen(gsg_string_positive)) {
    *input_byte = gsg_string_positive[call_count];
    return true;
  } else if (strlen(gsg_string_positive) + 1 == call_count) {
    return false;
  } else {
    TEST_FAIL_MESSAGE("uart__get_stub_callback_positive called too many times");
    return false;
  }
}

static void test_gps_init_setup(void) {
  QueueHandle_t rx_queue = NULL;
  QueueHandle_t tx_queue = NULL;

  gpio_s gpio = {0U};
  clock__get_peripheral_clock_hz_ExpectAndReturn(clock_rate_test);
  uart__init_Expect(gps_uart_test, clock_rate_test, baud_rate_test);
  xQueueCreate_ExpectAndReturn(100U, sizeof(char), rx_queue);
  xQueueCreate_ExpectAndReturn(8U, sizeof(char), tx_queue);
  uart__enable_queues_ExpectAndReturn(gps_uart_test, rx_queue, tx_queue, true);
  gpio__construct_with_function_ExpectAndReturn(GPIO__PORT_4, 28U, GPIO__FUNCTION_2, gpio);
  gpio__construct_with_function_ExpectAndReturn(GPIO__PORT_4, 29U, GPIO__FUNCTION_2, gpio);
}
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

void test_gps_init(void) {
  test_gps_init_setup();
  gps__init();
}

void test_gps_run_once(void) {
  uart__get_StubWithCallback(uart__get_stub_callback_negative);
  gps__run_once();
}

void test_gps_private_absorb_data(void) {
  uart__get_StubWithCallback(uart__get_stub_callback_negative);
  gps__private_absorb_data();
}

void test_gps_private_handle_line_negative_values(void) {
  test_gps_init_setup();
  gps__init();
  uart__get_StubWithCallback(uart__get_stub_callback_negative);
  gps__private_absorb_data();

  gps__private_handle_line();

  TEST_ASSERT_EQUAL(parsed_coordinates_test_negative.latitude, parsed_coordinates.latitude);
  TEST_ASSERT_EQUAL(parsed_coordinates_test_negative.longitude, parsed_coordinates.longitude);
}

void test_gps_private_handle_line_positive_values(void) {
  test_gps_init_setup();
  gps__init();
  uart__get_StubWithCallback(uart__get_stub_callback_positive);
  gps__private_absorb_data();

  gps__private_handle_line();

  TEST_ASSERT_EQUAL(parsed_coordinates_test_positive.latitude, parsed_coordinates.latitude);
  TEST_ASSERT_EQUAL(parsed_coordinates_test_positive.longitude, parsed_coordinates.longitude);
}

void test_gps_get_parsed_coordinates(void) {
  test_gps_init_setup();
  gps__init();
  uart__get_StubWithCallback(uart__get_stub_callback_positive);
  gps__private_absorb_data();

  gps__private_handle_line();
  gps_coordinates_t coordinates = gps__get_coordinates();
  TEST_ASSERT_EQUAL(parsed_coordinates_test_positive.latitude, coordinates.latitude);
  TEST_ASSERT_EQUAL(parsed_coordinates_test_positive.longitude, coordinates.longitude);
}

void test_gps_verify_nmea_checksum(void) {
  const bool checksum_match_positive =
      gps__private_verify_nmea_checksum(gsg_string_positive, strlen(gsg_string_positive));
  TEST_ASSERT_TRUE(checksum_match_positive);

  const bool checksum_match_negative =
      gps__private_verify_nmea_checksum(gsg_string_negative, strlen(gsg_string_negative));
  TEST_ASSERT_TRUE(checksum_match_negative);

  const bool checksum_match_fail =
      gps__private_verify_nmea_checksum(gsg_string_checksum_fail, strlen(gsg_string_checksum_fail));
  TEST_ASSERT_FALSE(checksum_match_fail);
}
#include "gps.h"

#include "line_buffer.h"

#include "FreeRTOS.h"
#include "clock.h"
#include "gpio.h"
#include "queue.h"
#include "uart.h"

#include "inttypes.h"
#include <stdio.h>
#include <string.h>

/*******************************************************************************
 *
 *               P R I V A T E    D A T A    D E F I N I T I O N S
 *
 ******************************************************************************/

static const uart_e gps_uart = UART__3;
static char line_buffer[256U];
static line_buffer_s line;
static gps_coordinates_t parsed_coordinates;

/*******************************************************************************
 *
 *                     P R I V A T E    F U N C T I O N S
 *
 ******************************************************************************/

static void gps__private_absorb_data(void) {
  char byte;
  while (uart__get(gps_uart, &byte, 0U)) {
    (void)line_buffer__add_byte(&line, byte);
  }
}

static bool gps__private_verify_nmea_checksum(const char *gps_line, size_t line_size) {
  bool checksum_match = false;
  size_t character_count = 0U;
  uint32_t calculated_checksum = 0U;
  uint32_t expected_checksum = 0U;
  if ('$' == gps_line[0U]) {
    const char *current_char = &gps_line[1U];
    while (NULL != current_char && '*' != *current_char && character_count < line_size) {
      calculated_checksum ^= (*current_char);
      current_char++;
      character_count++;
    }
    if ('*' == *current_char) {
      current_char++;
      if (sscanf(current_char, "%" PRIX32 "", &expected_checksum)) {
        checksum_match = (expected_checksum == calculated_checksum);
      }
    }
  }
  return checksum_match;
}

static void gps__private_handle_line(void) {
  char gps_line[100U] = {0U};
  const char *const delimiter = ",";
  while (line_buffer__remove_line(&line, gps_line, sizeof(gps_line)) &&
         gps__private_verify_nmea_checksum(gps_line, sizeof(gps_line))) {
    char latitude_direction = '\0';
    char longitude_direction = '\0';
    const char *token = strtok(gps_line, delimiter);
    bool valid_string = (NULL != token);

    // Skip to Latitude Parameter and Store Float
    if (valid_string)
      valid_string &= (NULL != (token = strtok(NULL, delimiter)));
    if (valid_string)
      valid_string &= (NULL != (token = strtok(NULL, delimiter)));
    if (valid_string)
      valid_string &= (0U != sscanf(token, "%f", &parsed_coordinates.latitude));

    // Skip to Latitude Direction and Store Character
    if (valid_string)
      valid_string &= (NULL != (token = strtok(NULL, delimiter)));
    if (valid_string)
      valid_string &= (0U != sscanf(token, "%c", &latitude_direction));

    // Skip to Longitude Parameter and Store Float
    if (valid_string)
      valid_string &= (NULL != (token = strtok(NULL, delimiter)));
    if (valid_string)
      valid_string &= (0U != sscanf(token, "%f", &parsed_coordinates.longitude));

    // Skip to Longitude Direction and Store Character
    if (valid_string)
      valid_string &= (NULL != (token = strtok(NULL, delimiter)));
    if (valid_string)
      valid_string &= (0U != sscanf(token, "%c", &longitude_direction));

    // Handle South and West Directions
    if (valid_string) {
      if ('S' == latitude_direction) {
        parsed_coordinates.latitude = -parsed_coordinates.latitude;
      }
      if ('W' == longitude_direction) {
        parsed_coordinates.longitude = -parsed_coordinates.longitude;
      }
    }
  }
}

static void gps__private_configure_for_nmea_gngga(void) {
  /* TODO: Send ublox configuration commands to set GPS
   * to NMEA GNGGA message only over UART.
   */
}

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/

void gps__init(void) {
  line_buffer__init(&line, line_buffer, sizeof(line_buffer));
  uart__init(gps_uart, clock__get_peripheral_clock_hz(), 9600U);
  (void)gpio__construct_with_function(GPIO__PORT_4, 28U, GPIO__FUNCTION_2);
  (void)gpio__construct_with_function(GPIO__PORT_4, 29U, GPIO__FUNCTION_2);

  // RX queue should be sized such that can buffer data in UART driver until gps__run_once() is called
  // Note: Assuming 38400bps, we can get 4 chars per ms, and 40 chars per 10ms (100Hz)
  QueueHandle_t rxq_handle = xQueueCreate(100U, sizeof(char));
  QueueHandle_t txq_handle = xQueueCreate(8U, sizeof(char)); // We don't send anything to the GPS
  (void)uart__enable_queues(gps_uart, rxq_handle, txq_handle);
  gps__private_configure_for_nmea_gngga();
}

void gps__run_once_10Hz(void) {
  gps__private_absorb_data();
  gps__private_handle_line();
}

gps_coordinates_t gps__get_coordinates(void) { return parsed_coordinates; }
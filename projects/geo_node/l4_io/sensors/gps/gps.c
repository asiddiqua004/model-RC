#include "gps.h"

#include "line_buffer.h"

#include "FreeRTOS.h"
#include "board_io.h"
#include "can_bus_initializer.h"
#include "can_bus_message_handler.h"
#include "clock.h"
#include "gpio.h"
#include "queue.h"
#include "tesla_model_rc.h"
#include "uart.h"
#include "uart_printf.h"

#include "inttypes.h"
#include <math.h>
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
static uint8_t fix_quality = 0;
static bool is_gps_configured = false;
static bool gps_lock = false;

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

static gps_coordinates_t gps__private_convert_from_dms_to_decimal_degrees(float latitude, float longitude) {
  const float latitude_dms_degrees = (float)trunc((latitude / 100.0f));
  const float latitude_dms_minutes = fmodf(latitude, 100.0f);

  const float longitude_dms_degrees = (float)trunc((longitude / 100.0f));
  const float longitude_dms_minutes = fmodf(longitude, 100.0f);

  const float latitude_decimal_degrees = latitude_dms_degrees + (latitude_dms_minutes / 60.0f);
  const float longitude_decimal_degrees = longitude_dms_degrees + (longitude_dms_minutes / 60.0f);

  const gps_coordinates_t converted_coordinates = {
      .latitude = latitude_decimal_degrees,
      .longitude = longitude_decimal_degrees,
  };
  return converted_coordinates;
}

static void gps__private_handle_line(void) {
  char gps_line[300U] = {0U};
  const char *const delimiter = ",";
  while (line_buffer__remove_line(&line, gps_line, sizeof(gps_line)) &&
         gps__private_verify_nmea_checksum(gps_line, sizeof(gps_line))) {

    char latitude_direction = '\0';
    char longitude_direction = '\0';
    float latitude = 0.0f;
    float longitude = 0.0f;
    const char *token = strtok(gps_line, delimiter);
    bool valid_string = (NULL != token);

    // Skip to Latitude Parameter and Store Float
    if (valid_string)
      valid_string &= (NULL != (token = strtok(NULL, delimiter)));
    if (valid_string)
      valid_string &= (NULL != (token = strtok(NULL, delimiter)));
    if (valid_string)
      valid_string &= (0U != sscanf(token, "%f", &latitude));

    // Skip to Latitude Direction and Store Character
    if (valid_string)
      valid_string &= (NULL != (token = strtok(NULL, delimiter)));
    if (valid_string)
      valid_string &= (0U != sscanf(token, "%c", &latitude_direction));

    // Skip to Longitude Parameter and Store Float
    if (valid_string)
      valid_string &= (NULL != (token = strtok(NULL, delimiter)));
    if (valid_string)
      valid_string &= (0U != sscanf(token, "%f", &longitude));

    // Skip to Longitude Direction and Store Character
    if (valid_string)
      valid_string &= (NULL != (token = strtok(NULL, delimiter)));
    if (valid_string)
      valid_string &= (0U != sscanf(token, "%c", &longitude_direction));

    // Skip to Fix Quality and Store Integer
    if (valid_string)
      valid_string &= (NULL != (token = strtok(NULL, delimiter)));
    if (valid_string)
      valid_string &= (0U != sscanf(token, "%hhd", &fix_quality));

    // Handle South and West Directions
    if (valid_string) {
      if ('S' == latitude_direction) {
        latitude = -latitude;
      }
      if ('W' == longitude_direction) {
        longitude = -longitude;
      }
      const gps_coordinates_t converted_coordinates =
          gps__private_convert_from_dms_to_decimal_degrees(latitude, longitude);
      parsed_coordinates.latitude = converted_coordinates.latitude;
      parsed_coordinates.longitude = converted_coordinates.longitude;
    }
  }
}

static void gps__private_configure_for_nmea_gngga(void) {

  // Disable all NMEA messages
  // char disable_gngga_message[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x00,
  //                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x23};
  // char disable_gngll_message[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x01,
  //                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A};
  // char disable_gngsa_message[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x02,
  //                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x31};
  // char disable_gngsv_message[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x03,
  //                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x38};
  // char disable_gnrmc_message[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x04,
  //                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x3F};
  // char disable_gnvtg_message[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x05,
  //                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x46};
  // char disable_gngrs_message[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x06,
  //                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x4D};
  // char disable_gngst_message[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x07,
  //                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x54};
  // char disable_gnzda_message[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x08,
  //                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x5B};
  // char disable_gngbs_message[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x09,
  //                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x62};
  // char disable_gndtm_message[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x0A,
  //                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x69};
  // char disable_gngns_message[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x0D,
  //                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x7E};
  // char disable_gnvlw_message[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x0F,
  //                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x8C};

  // // Enable only NMEA GNGGA message
  // char enable_gngga_message[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x00,
  //                                0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28};

  const char disable_all_nmea_messages_except_gngga[] = {
      0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A, 0xB5,
      0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x31, 0xB5, 0x62,
      0x06, 0x01, 0x08, 0x00, 0xF0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x38, 0xB5, 0x62, 0x06,
      0x01, 0x08, 0x00, 0xF0, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x3F, 0xB5, 0x62, 0x06, 0x01,
      0x08, 0x00, 0xF0, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x46, 0x28,
  };

  size_t i = 0;
  while (disable_all_nmea_messages_except_gngga[i] != 0x28) {
    uart__put(gps_uart, disable_all_nmea_messages_except_gngga[i], 0U);
    i++;
  }
}

void gps__private_send_debug_messages(void) {
  dbc_GEO_DEBUG_s geo_debug_messages = {0U};
  geo_debug_messages.GEO_DEBUG_CAN_INIT = can_bus_initalizer__get_can_init_status();
  geo_debug_messages.GEO_DEBUG_GPS_INIT = is_gps_configured;
  geo_debug_messages.GEO_DEBUG_GPS_LOCK = gps_lock;
  dbc_encode_and_send_GEO_DEBUG(NULL, &geo_debug_messages);
}
/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/

void gps__init(void) {
  line_buffer__init(&line, line_buffer, sizeof(line_buffer));
  uart__init(gps_uart, clock__get_peripheral_clock_hz(), 9600);
  (void)gpio__construct_with_function(GPIO__PORT_4, 28U, GPIO__FUNCTION_2); // P4.28 (Tx3) --> GPS Rx (Yellow wire)
  (void)gpio__construct_with_function(GPIO__PORT_4, 29U, GPIO__FUNCTION_2); // P4.29 (Rx3) --> GPS Tx (Green wire)

  // RX queue should be sized such that can buffer data in UART driver until gps__run_once() is called
  // Note: Assuming 38400bps, we can get 4 chars per ms, and 40 chars per 10ms (100Hz)
  QueueHandle_t rxq_handle = xQueueCreate(300U, sizeof(char));
  QueueHandle_t txq_handle = xQueueCreate(300U, sizeof(char));
  (void)uart__enable_queues(gps_uart, rxq_handle, txq_handle);
}

void gps__run_once(void) {
  if (false == is_gps_configured) {
    is_gps_configured = true;
    gps__private_configure_for_nmea_gngga();
  }
  gps__private_absorb_data();
  gps__private_handle_line();
  gps__private_send_debug_messages();
}

gps_coordinates_t gps__get_coordinates(void) { return parsed_coordinates; }

bool gps__get_gps_lock(void) {
  if (1 == fix_quality || 2 == fix_quality) {
    gps_lock = true;
    gpio__toggle(board_io__get_led3());
  } else if (0 == fix_quality) {
    gpio__set(board_io__get_led3());
    gps_lock = false;
  }
  return gps_lock;
}

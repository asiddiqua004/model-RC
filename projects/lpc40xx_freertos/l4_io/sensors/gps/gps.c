#include "gps.h"

#include "line_buffer.h"

#include "FreeRTOS.h"
#include "board_io.h"
#include "clock.h"
#include "gpio.h"
#include "queue.h"
#include "uart.h"
#include "uart_printf.h"

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
static uint8_t fix_quality = 0;
static bool is_gps_configured = false;
// static bool is_gps_disconnected = false;

/*******************************************************************************
 *
 *                     P R I V A T E    F U N C T I O N S
 *
 ******************************************************************************/

static void gps__private_absorb_data(void) {
  char byte;
  while (uart__get(gps_uart, &byte, 0U)) {
    (void)line_buffer__add_byte(&line, byte);
    printf("%c", byte);
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

    // Skip to Fix Quality and Store Integer
    if (valid_string)
      valid_string &= (NULL != (token = strtok(NULL, delimiter)));
    if (valid_string)
      valid_string &= (0U != sscanf(token, "%hhd", &fix_quality));

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
  // if (false == line_buffer__remove_line(&line, gps_line, sizeof(gps_line))) {
  //   // Fix quality is set to 0 in the case the GPS is disconnected
  //   fix_quality = 0;
  //   // is_gps_configured = false;
  //   is_gps_disconnected = true;
  // } else {
  //   is_gps_disconnected = false;
  // }
}

static void gps__private_configure_for_nmea_gngga(void) {

  // // Disable all NMEA messages
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

  // Set Baudrate to 38400bps
  // char set_baudrate_to_38400[] = {0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00,
  // 0x00, 0x96, 0x00, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x90};
  const char disable_all_nmea_messages_and_enable_gngga[] = {
      0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A, 0xB5,
      0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x31, 0xB5, 0x62,
      0x06, 0x01, 0x08, 0x00, 0xF0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x38, 0xB5, 0x62, 0x06,
      0x01, 0x08, 0x00, 0xF0, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x3F, 0xB5, 0x62, 0x06, 0x01,
      0x08, 0x00, 0xF0, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x46, 0x28};

  size_t i = 0;
  while (disable_all_nmea_messages_and_enable_gngga[i] != 0x28) {
    uart__put(gps_uart, disable_all_nmea_messages_and_enable_gngga[i], 0U);
    i++;
  }

  // is_gps_configured = true;
  // uart_puts(gps_uart, disable_all_nmea_messages_and_enable_gngga);
}

static bool gps__private_get_gps_lock(void) {
  bool gps_lock = false;
  if (1 == fix_quality || 2 == fix_quality) {
    gps_lock = true;
    gpio__toggle(board_io__get_led3());
  } else if (0 == fix_quality) {
    gpio__set(board_io__get_led3());
    gps_lock = false;
  }
  return gps_lock;
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
  // gps__private_configure_for_nmea_gngga();
}

void gps__run_once(void) {
  if (false == is_gps_configured) {
    is_gps_configured = true;
    gps__private_configure_for_nmea_gngga();
  }

  gps__private_absorb_data();
  gps__private_handle_line();
  printf("GPS lock = %u\n\n", gps__private_get_gps_lock());
  printf("parsed fix quality: %d\n", fix_quality);
  // if (is_gps_disconnected == true) {
  //   printf("GPS disconnected\n");
  // }
}

gps_coordinates_t gps__get_coordinates(void) { return parsed_coordinates; }
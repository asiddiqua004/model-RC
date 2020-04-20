#include "wifi_message_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { STATUS_ID = '$', COORDINATE_ID = '#' } line_message_type_e;

static BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE_e vehicle_navigation_state = BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE_STOP;
static float GPS_headings_longitude;
static float GPS_headings_latitude;

static void wifi_message_handler__set_vehicle_navigation_state(const char *line) {
  if (0 == strcmp(line, "start")) {
    vehicle_navigation_state = BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE_NAVIGATE;
  } else if (0 == strcmp(line, "stop")) {
    vehicle_navigation_state = BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE_STOP;
  }
}

static void wifi_message_handler__set_GPS_headings(const char *line, const size_t line_length) {
  const size_t total_gps_line_length =
      strlen("Sxxxx.xxxx,Syyyy.yyyy"); // where S is sign, x is latitude, and y is longitude
  if (total_gps_line_length == line_length) {
    char latitude[10] = {0};
    char latitude_sign = line[0];

    char longitude[10] = {0};
    char longitude_sign = line[11];

    strncpy(latitude, line + 1, sizeof(latitude) - 1);
    strncpy(longitude, line + 12, sizeof(longitude) - 1);

    GPS_headings_latitude = atof(latitude);
    GPS_headings_longitude = atof(longitude);

    if (latitude_sign == '-') {
      GPS_headings_latitude *= -1;
    }

    if (longitude_sign == '-') {
      GPS_headings_longitude *= -1;
    }
  }
}

void wifi_message_handler__parse_line(const char *line, const size_t line_length) {
  const char message_identifier = line[0];
  switch (message_identifier) {
  case STATUS_ID:
    wifi_message_handler__set_vehicle_navigation_state(line + 1);
    break;
  case COORDINATE_ID:
    wifi_message_handler__set_GPS_headings(line + 1, line_length - 1);
    break;
  default:
    break;
  }

  // remove when done debugging
  printf("line found: %s\n", line);
}

BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE_e wifi_message_handler__get_vehicle_navigation_state(void) {
  return vehicle_navigation_state;
}

float wifi_message_handler__get_GPS_headings_latitude(void) { return GPS_headings_latitude; }

float wifi_message_handler__get_GPS_headings_longitude(void) { return GPS_headings_longitude; }

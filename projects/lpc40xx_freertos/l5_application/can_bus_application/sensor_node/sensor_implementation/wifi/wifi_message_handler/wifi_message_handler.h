#include <stddef.h>

#include "tesla_model_rc.h"

void wifi_message_handler__parse_line(const char *line, const size_t line_length);

BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE_e wifi_message_handler__get_vehicle_navigation_state(void);

float wifi_message_handler__get_GPS_headings_latitude(void);

float wifi_message_handler__get_GPS_headings_longitude(void);
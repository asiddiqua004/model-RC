#pragma once

#include <stdint.h>

// Enables UART 3 at 74880 baud rate for interfacing with Wifi device
void wifi_implementation__initialize(void);
// Polling test echo which will send and received over UART3 and echo received value over UART0
void wifi_implementation__polled_test_echo(void);

void wifi_implementation__fill_line_buffer(void);

void wifi_implementation__handle_line(void);

void wifi_implementation__send_line(const uint8_t front_radar, const uint8_t left_radar, const uint8_t right_radar,
                                    const uint8_t back_radar);

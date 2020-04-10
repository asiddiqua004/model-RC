// Enables UART 3 at 74880 baud rate for interfacing with Wifi device
void wifi_implementation__initialize(void);
// Polling test echo which will send and received over UART3 and echo received value over UART0
void wifi_implementation__polled_test_echo(void);

void wifi_implementation__fill_line_buffer(void);

void wifi_implementation__handle_line(void);

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clock.h" // needed for UART initialization
#include "delay.h" //included for the delay
#include "gpio.h"
#include "serial_lcd.h"
#include "uart.h"
#include "uart_printf.h"
// Rtos dependency for the UART driver
#include "FreeRTOS.h"
#include "queue.h"

static uart_e lcd_uart = UART__3;

/****************PRIVATE FUNCTIONS ****************/

static uint8_t lcd_uart_tx(uint8_t *buf, uint8_t num_bytes) {
  uint8_t x, y = 0;
  for (x = 0; x < num_bytes; x++) {
    y |= uart__put(lcd_uart, buf[x], UINT32_MAX);
    // delay__ms(1);
  }
  return y;
}

static uint8_t lcd_uart_write_reg(uint8_t addr, uint8_t data) {
  uint8_t x;
  uint8_t buf[2];
  // Send read request
  buf[0] = CONST_LCD_UART_REG_WRITE | addr;
  buf[1] = data;
  x = lcd_uart_tx(buf, 2);
  delay__ms(1);
  return x;
}

// // Write Register
// static uint8_t lcd_write_reg(uint8_t addr, uint8_t data) {
//   uint8_t x = 0;
//   x = lcd_uart_write_reg(addr, data);
//   return x;
// }

static uint8_t lcdSpecialFunction(uint8_t func) {
  uint8_t x;
  x = lcd_uart_write_reg(CONST_ADDR_SPCL_REG, func);
  x |= lcd_uart_write_reg(CONST_ADDR_UNLOCK1_REG, CONST_UNLOCK1);
  x |= lcd_uart_write_reg(CONST_ADDR_UNLOCK2_REG, CONST_UNLOCK2);
  LCD_WAIT_SPL_FUNC;
  return x;
}

static uint8_t lcdReset(void) { return lcdSpecialFunction(CONST_SPL_FUNC_CPU_RESET); }

static uint8_t lcdBacklight(uint8_t val) {
  uint8_t y;
  y = lcd_uart_write_reg(CONST_ADDR_SPCL_W_REG_1, val);
  y |= lcdSpecialFunction(CONST_SPL_FUNC_LCD_BACKLIGHT);
  return y;
}

static uint8_t lcdContrast(uint8_t val) {
  uint8_t y;
  y = lcd_uart_write_reg(CONST_ADDR_SPCL_W_REG_1, val);
  y |= lcdSpecialFunction(CONST_SPL_FUNC_LCD_CONTRAST);
  return y;
}

/****************PUBLIC FUNCTIONS ****************/
void lcd_row_write(uint8_t row, uint8_t *new_data) {
  uint8_t x, y;
  uint8_t data_string[CONST_LCD_WIDTH + 1];

  // Check row number
  if ((row < 1) || (row > CONST_LCD_NUM_ROWS)) {
    printf("Row %i is out of bounds", row);
    return 0;
  }
  // Check length of string
  y = strlen(new_data);
  if (y > CONST_LCD_WIDTH) {
    printf("Width %i is out of bounds", y);
    return 0;
  }
  // Initialize to empty
  memset(data_string, CONST_LCD_EMPTY, sizeof(data_string));
  // copy the new data
  memcpy(data_string, new_data, y);
  // Write to LCD
  x = 0;
  for (y = 0; y < CONST_LCD_WIDTH; y++) {
    x |= lcd_uart_write_reg(CONST_ADDR_LCD_DATA_START + y, data_string[y]);
  }
  // Issue update command
  x |= lcd_uart_write_reg(CONST_ADDR_LCD_ACTION, (row - 1) | CONST_LCD_ACTION_GO_BIT);
  // Wait for LCD update
  delay__ms(2);
  // return x;
}

void lcd_row_clear(uint8_t line_num) { lcd_row_write(line_num, " "); }

void clear_screen(void) {
  lcd_row_write(1, " ");
  lcd_row_write(2, " ");
  lcd_row_write(3, " ");
  lcd_row_write(4, " ");
}

void init_serial_lcd(void) {
  uart__init(lcd_uart, clock__get_peripheral_clock_hz(), 38400);
  // printf("uart initialized\n");
  // Uart3
  gpio__construct_with_function(GPIO__PORT_4, 28, GPIO__FUNCTION_2); // P4.28 as TXD3
  gpio__construct_with_function(GPIO__PORT_4, 29, GPIO__FUNCTION_2); // P4.29 as RXD3

  QueueHandle_t rxq_handle = xQueueCreate(32, sizeof(char));
  QueueHandle_t txq_handle = xQueueCreate(85, sizeof(char));
  uart__enable_queues(lcd_uart, rxq_handle, txq_handle);

  // set backlight and contrast
  clear_screen();
  lcdBacklight(140);
  lcdContrast(60);

  //   sprintf(buf, "Welcome");
  //   lcd_row_write(1, buf);
  //   delay__ms(10);

  //   sprintf(buf, "Waiting to start");
  //   lcd_row_write(2, buf);
  //   delay__ms(10);
}

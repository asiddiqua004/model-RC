#pragma once

#include "delay.h"
#include <stdint.h>

/////////////////////////////////////////////////
// Unlock commands
/////////////////////////////////////////////////
#define CONST_SPL_FUNC_CPU_RESET 0
#define CONST_SPL_FUNC_LCD_CONTRAST 3
#define CONST_SPL_FUNC_LCD_CONTRAST_SAVE 4
#define CONST_SPL_FUNC_LCD_BACKLIGHT 5
#define CONST_SPL_FUNC_LCD_BACKLIGHT_SAVE 6
#define CONST_SPL_FUNC_INTERFACE 14

/////////////////////////////////////////////////
// Communication modes
/////////////////////////////////////////////////
#define CONST_MODE_UART_9600 1
#define CONST_MODE_UART_19200 2
#define CONST_MODE_UART_38400 3
#define CONST_MODE_UART_57600 4
#define CONST_MODE_UART_115200 5

#define CONST_ADDR_SPCL_REG 18
#define CONST_ADDR_UNLOCK1_REG 19
#define CONST_ADDR_UNLOCK2_REG 20

#define CONST_UNLOCK1 0x12
#define CONST_UNLOCK2 0x34

#define CONST_LCD_NUM_ROWS 4
#define CONST_LCD_WIDTH 20
#define CONST_MAX_LCD_CONTRAST 160
#define CONST_MAX_LCD_BACKLIGHT 160
#define CONST_LCD_EMPTY 0x20
#define CONST_LCD_DATA_SIZE 21
#define CONST_ADDR_LCD_DATA_END (CONST_ADDR_LCD_DATA_START + CONST_LCD_DATA_SIZE)
#define CONST_LAST_REG_ADDR CONST_ADDR_LCD_DATA_END

#define LCD_END_PACKET_DELAY                                                                                           \
  { delay__ms(10); }
#define LCD_WAIT_SPL_FUNC                                                                                              \
  { delay__ms(5); }

#define CONST_ADDR_SPCL_W_REG_1 17
#define CONST_LCD_UART_REG_WRITE (0x80)
#define CONST_ADDR_LCD_ACTION 21
#define CONST_ADDR_LCD_DATA_START 22
#define CONST_LCD_ACTION_GO_BIT (1 << 7)

// 20X4 lcd with uart driver.
// Rows start from 1.

void init_serial_lcd(void);
void clear_screen(void);
void lcd_row_clear(uint8_t line_num);
void lcd_row_write(uint8_t row, uint8_t *new_data);

// void print_seg(uint8_t line_num, uint8_t pos, char* char_msg, int len);
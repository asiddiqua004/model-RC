#pragma once

#include "i2c.h"
#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
 *
 *                               D E F I N E S
 *
 ******************************************************************************/

// Reference: https://github.com/jrowberg/i2cdevlib/blob/master/Arduino/HMC5883L/HMC5883L.h

// clang-format off

#define COMPASS_CONFIG_REG_A_AVERAGE_BIT    6
#define COMPASS_CONFIG_REG_A_AVERAGE_LENGTH 2
#define COMPASS_CONFIG_REG_A_RATE_BIT       4
#define COMPASS_CONFIG_REG_A_RATE_LENGTH    3
#define COMPASS_CONFIG_REG_A_BIAS_BIT       1
#define COMPASS_CONFIG_REG_A_BIAS_LENGTH    2

#define COMPASS_SAMPLES_AVERAGING_1         0x00
#define COMPASS_SAMPLES_AVERAGING_2         0x01
#define COMPASS_SAMPLES_AVERAGING_4         0x02
#define COMPASS_SAMPLES_AVERAGING_8         0x03

#define COMPASS_DATA_OUT_RATE_0P75          0x00
#define COMPASS_DATA_OUT_RATE_1P5           0x01
#define COMPASS_DATA_OUT_RATE_3             0x02
#define COMPASS_DATA_OUT_RATE_7P5           0x03
#define COMPASS_DATA_OUT_RATE_15            0x04
#define COMPASS_DATA_OUT_RATE_30            0x05
#define COMPASS_DATA_OUT_RATE_75            0x06

#define COMPASS_BIAS_NORMAL                 0x00
#define COMPASS_BIAS_POSITIVE               0x01
#define COMPASS_BIAS_NEGATIVE               0x02

#define COMPASS_CONFIG_REG_B_GAIN_BIT       7
#define COMPASS_CONFIG_REG_B_GAIN_LENGTH    3

#define COMPASS_GAIN_1370                   0x00
#define COMPASS_GAIN_1090                   0x01
#define COMPASS_GAIN_820                    0x02
#define COMPASS_GAIN_660                    0x03
#define COMPASS_GAIN_440                    0x04
#define COMPASS_GAIN_390                    0x05
#define COMPASS_GAIN_330                    0x06
#define COMPASS_GAIN_220                    0x07

#define COMPASS_MODE_REG_BIT                1
#define COMPASS_MODE_REG_LENGTH             2

#define COMPASS_MODE_CONTINUOUS             0x00
#define COMPASS_MODE_SINGLE                 0x01
#define COMPASS_MODE_IDLE                   0x02

#define COMPASS_STATUS_REG_LOCK_BIT         1
#define COMPASS_STATUS_REG_READY_BIT        0

// clang-format on

/*******************************************************************************
 *
 *                                 E N U M S
 *
 ******************************************************************************/

// HMC5883L Digital Compass I2C registers
typedef enum {
  COMPASS__MEMORY_CONFIG_REG_A = 0x00,
  COMPASS__MEMORY_CONFIG_REG_B = 0x01,
  COMPASS__MEMORY_MODE_REG = 0x02,
  COMPASS__MEMORY_DATA_OUT_X_MSB = 0x03,
  COMPASS__MEMORY_DATA_OUT_X_LSB = 0x04,
  COMPASS__MEMORY_DATA_OUT_Z_MSB = 0x05,
  COMPASS__MEMORY_DATA_OUT_Z_LSB = 0x06,
  COMPASS__MEMORY_DATA_OUT_Y_MSB = 0x07,
  COMPASS__MEMORY_DATA_OUT_Y_LSB = 0x08,
  COMPASS__MEMORY_STATUS_REG = 0x09,
  COMPASS__MEMORY_ID_REG_A = 0x0A,
  COMPASS__MEMORY_ID_REG_B = 0x0B,
  COMPASS__MEMORY_ID_REG_C = 0x0C,
} compass__memory_e;

/*******************************************************************************
 *
 *                               T Y P E D E F S
 *
 ******************************************************************************/

typedef struct {
  int16_t x, y, z;
} compass__axis_data_t;

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/

bool compass__init(void);

float compass__get_heading_degrees(void);

void compass__set_gain(uint8_t gain);
void compass__set_mode(uint8_t mode);

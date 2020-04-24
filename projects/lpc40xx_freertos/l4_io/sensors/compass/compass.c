#include <math.h>
#include <stdio.h>

#include "compass.h"
#include "i2c.h"

/*******************************************************************************
 *
 *               P I N S    U S E D    O N    S J 2 B O A R D
 *
 *                  SJ2 BOARD           COMPASS (HCM5883L)
 *                 P0.10 (SDA)   --->    SDA (White wire)
 *                 P0.11 (SCL2)  --->    SCL (Orange wire)
 ******************************************************************************/

/*******************************************************************************
 *
 *               P R I V A T E    D A T A    D E F I N I T I O N S
 *
 ******************************************************************************/
// Private data for modules should be rare because all data should be part
// of the module's struct instance

static const uint8_t COMPASS__EXPECTED_ID = 72U;
static const i2c_e COMPASS__BUS = I2C__2;
static const uint8_t COMPASS__ADDRESS = 0x3C;
static uint8_t mode;

#define M_PI 3.14159265358979323846264338327950288

/*******************************************************************************
 *
 *                     P R I V A T E    F U N C T I O N S
 *
 ******************************************************************************/

static float compass__private_compute_heading(compass__axis_data_t axis_data) {
  // Reference: https://github.com/adafruit/Adafruit_HMC5883_Unified/blob/master/examples/magsensor/magsensor.ino
  float compass_heading = atan2f(axis_data.y, axis_data.x);

  const float compass_declination_angle = 0.23f; // http://www.magnetic-declination.com
  compass_heading += compass_declination_angle;

  // We need to normalize this angle in radians before converting to degrees.
  // This way, instead of having -90* for example, we will have 270*. This will make processing
  // this heading easier moving forward.
  if (compass_heading < 0.0f) {
    compass_heading += 2.0f * (float)M_PI;
  }
  if (compass_heading > 2.0f * (float)M_PI) {
    compass_heading -= 2.0f * (float)M_PI;
  }
  return compass_heading;
}

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/

bool compass__init(void) {
  bool compass__init_success = false;

  const uint8_t COMPASS__ACTUAL_ID = i2c__read_single(COMPASS__BUS, COMPASS__ADDRESS, COMPASS__MEMORY_ID_REG_A);

  if (COMPASS__ACTUAL_ID == COMPASS__EXPECTED_ID) {
    // Set default settings
    i2c__write_single(
        COMPASS__BUS, COMPASS__ADDRESS, COMPASS__MEMORY_CONFIG_REG_A,
        (COMPASS__SAMPLES_AVERAGING_8 << (COMPASS__CONFIG_REG_A_AVERAGE_BIT - COMPASS__CONFIG_REG_A_AVERAGE_LENGTH +
                                          1)) |
            (COMPASS__DATA_OUT_RATE_15 << (COMPASS__CONFIG_REG_A_RATE_BIT - COMPASS__CONFIG_REG_A_RATE_LENGTH + 1)) |
            (COMPASS__BIAS_NORMAL << (COMPASS__CONFIG_REG_A_BIAS_BIT - COMPASS__CONFIG_REG_A_BIAS_LENGTH + 1)));

    compass__set_gain(COMPASS__GAIN_1090);
    compass__set_mode(COMPASS__MODE_SINGLE);
    compass__init_success = true;
  }
  return compass__init_success;
}

float compass__get_heading_degrees(void) {
  compass__axis_data_t axis_data = {0};

  uint8_t axis_raw_data[6] = {0U};
  i2c__read_slave_data(COMPASS__BUS, COMPASS__ADDRESS, COMPASS__MEMORY_DATA_OUT_X_MSB, axis_raw_data,
                       sizeof(axis_raw_data));

  if (mode == COMPASS__MODE_SINGLE)
    i2c__write_single(COMPASS__BUS, COMPASS__ADDRESS, COMPASS__MEMORY_MODE_REG,
                      COMPASS__MODE_SINGLE << (COMPASS__MODE_REG_BIT - COMPASS__MODE_REG_LENGTH + 1));

  axis_data.x = (int16_t)(axis_raw_data[1] | (int16_t)axis_raw_data[0] << 8);
  axis_data.y = (int16_t)(axis_raw_data[5] | (int16_t)axis_raw_data[4] << 8);
  axis_data.z = (int16_t)(axis_raw_data[3] | (int16_t)axis_raw_data[2] << 8);

  const float compass_heading_degrees = compass__private_compute_heading(axis_data) * 180.0f / (float)M_PI;

  return compass_heading_degrees;
}

void compass__set_gain(uint8_t gain) {
  i2c__write_single(COMPASS__BUS, COMPASS__ADDRESS, COMPASS__MEMORY_CONFIG_REG_B,
                    gain << (COMPASS__CONFIG_REG_B_GAIN_BIT - COMPASS__CONFIG_REG_B_GAIN_LENGTH + 1));
}

void compass__set_mode(uint8_t new_mode) {
  i2c__write_single(COMPASS__BUS, COMPASS__ADDRESS, COMPASS__MEMORY_MODE_REG,
                    new_mode << (COMPASS__MODE_REG_BIT - COMPASS__MODE_REG_LENGTH + 1));
  mode = new_mode;
}
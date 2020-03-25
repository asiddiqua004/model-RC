#include <math.h>

#include "compass.h"
#include "gpio.h"
/*******************************************************************************
 *
 *               P R I V A T E    D A T A    D E F I N I T I O N S
 *
 ******************************************************************************/
// Private data for modules should be rare because all data should be part
// of the module's struct instance

static const uint8_t COMPASS__EXPECTED_ID = 0xD4;
static const i2c_e COMPASS__BUS = I2C__2;
static const uint8_t COMPASS__ADDRESS = 0x1E; // Default address 0x1E
#define M_PI 3.14159265358979323846264338327950288

/*******************************************************************************
 *
 *                     P R I V A T E    F U N C T I O N S
 *
 ******************************************************************************/

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/

bool compass__init(void) {
  (void)gpio__construct_with_function(GPIO__PORT_0, 10, GPIO__FUNCTION_2);
  (void)gpio__construct_with_function(GPIO__PORT_0, 11, GPIO__FUNCTION_2);
  i2c__write_single(
      COMPASS__BUS, COMPASS__ADDRESS, COMPASS__MEMORY_CONFIG_REG_A,
      (COMPASS_SAMPLES_AVERAGING_8 << (COMPASS_CONFIG_REG_A_AVERAGE_BIT - COMPASS_CONFIG_REG_A_AVERAGE_LENGTH + 1)) |
          (COMPASS_DATA_OUT_RATE_15 << (COMPASS_CONFIG_REG_A_RATE_BIT - COMPASS_CONFIG_REG_A_RATE_LENGTH + 1)) |
          (COMPASS_BIAS_NORMAL << (COMPASS_CONFIG_REG_A_BIAS_BIT - COMPASS_CONFIG_REG_A_BIAS_LENGTH + 1)));

  compass__set_gain(COMPASS_GAIN_1090);
  compass__set_mode(COMPASS_MODE_SINGLE);

  const uint8_t COMPASS__ACTUAL_ID = i2c__read_single(COMPASS__BUS, COMPASS__ADDRESS, COMPASS__MEMORY_ID_REG_A);
  return (COMPASS__EXPECTED_ID == COMPASS__ACTUAL_ID);
}

float compass__get_heading_degrees(void) {
  compass__axis_data_t axis_data = {0U};

  uint8_t axis_raw_data[6] = {0U};
  i2c__read_slave_data(COMPASS__BUS, COMPASS__ADDRESS, COMPASS__MEMORY_DATA_OUT_X_LSB, axis_raw_data,
                       sizeof(axis_raw_data));

  axis_data.x = ((uint16_t)axis_raw_data[0] << 8) | axis_raw_data[1];
  axis_data.y = ((uint16_t)axis_raw_data[4] << 8) | axis_raw_data[5];
  axis_data.z = ((uint16_t)axis_raw_data[2] << 8) | axis_raw_data[3];

  // Reference: https://github.com/adafruit/Adafruit_HMC5883_Unified/blob/master/examples/magsensor/magsensor.ino
  float compass_heading = atan2f(axis_data.y, axis_data.x);

  const float compass_declination_angle = 0.22f; // http://www.magnetic-declination.com
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

  const float compass_heading_degrees = compass_heading * 180.0f / (float)M_PI;

  return compass_heading_degrees;
}

void compass__set_gain(uint8_t gain) {
  i2c__write_single(COMPASS__BUS, COMPASS__ADDRESS, COMPASS__MEMORY_CONFIG_REG_B,
                    gain << (COMPASS_CONFIG_REG_B_GAIN_BIT - COMPASS_CONFIG_REG_B_GAIN_LENGTH + 1));
}

void compass__set_mode(uint8_t mode) {
  i2c__write_single(COMPASS__BUS, COMPASS__ADDRESS, COMPASS__MEMORY_MODE_REG,
                    mode << (COMPASS_MODE_REG_BIT - COMPASS_MODE_REG_LENGTH + 1));
}
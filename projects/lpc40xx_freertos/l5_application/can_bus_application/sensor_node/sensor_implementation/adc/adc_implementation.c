#include "adc_implementation.h"

#include "adc.h"
#include "gpio.h"

void adc_implementation__initialization(void) {
  gpio__construct_with_function(GPIO__PORT_0, 25U, GPIO__FUNCTION_1); // ADC02: P0.25

  adc__initialize();
}

float adc_implementation__get_battery_voltage(void) {
  const adc_channel_e battery_channel_adc = ADC__CHANNEL_2;

  static const float adc_conversion = 3.3F / 4095.0F; // VREFP tied to 3.3V regulated output; 2^12 ADC resolution
  static const float voltage_divider_multiplier = (100000.0F + 1000000.0F) / 100000.0F; // R2 = 100k ohm, R1 = 1M ohm

  return adc__get_adc_value(battery_channel_adc) * adc_conversion * voltage_divider_multiplier;
}

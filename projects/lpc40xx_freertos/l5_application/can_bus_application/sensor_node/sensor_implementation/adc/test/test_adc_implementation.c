#include "unity.h"

#include "Mockadc.h"
#include "Mockgpio.h"

#include "adc_implementation.c"

void setUp(void) {}
void tearDown(void) {}

void test_adc_implementation__initialization(void) {
  gpio_s gpio = {0};
  gpio__construct_with_function_ExpectAndReturn(GPIO__PORT_0, 25, GPIO__FUNCTION_1, gpio);
  gpio__set_as_input_Expect(gpio);
  adc__initialize_Expect();

  adc_implementation__initialization();
}

void test_adc_implementation__get_battery_voltage_conversions(void) {
  const float voltage_dividing_ratio = 100000.0F / (100000.0F + 1000000.0F);
  const float adc_step_resolution = 4095.0F / 3.3F;

  float battery_voltage = 7.23F;

  adc__get_adc_value_ExpectAndReturn(ADC__CHANNEL_2, battery_voltage * voltage_dividing_ratio * adc_step_resolution);
  const float voltage_out_one = adc_implementation__get_battery_voltage();
  TEST_ASSERT_EQUAL_size_t(voltage_out_one * 10, battery_voltage * 10);

  battery_voltage = 0.0F;

  adc__get_adc_value_ExpectAndReturn(ADC__CHANNEL_2, battery_voltage * voltage_dividing_ratio * adc_step_resolution);
  const float voltage_out_two = adc_implementation__get_battery_voltage();
  TEST_ASSERT_EQUAL_size_t(voltage_out_two * 10, battery_voltage * 10);

  battery_voltage = 4.67F;

  adc__get_adc_value_ExpectAndReturn(ADC__CHANNEL_2, battery_voltage * voltage_dividing_ratio * adc_step_resolution);
  const float voltage_out_three = adc_implementation__get_battery_voltage();
  TEST_ASSERT_EQUAL_size_t(voltage_out_three * 10, battery_voltage * 10);

  battery_voltage = 1.53F;

  adc__get_adc_value_ExpectAndReturn(ADC__CHANNEL_2, battery_voltage * voltage_dividing_ratio * adc_step_resolution);
  const float voltage_out_four = adc_implementation__get_battery_voltage();
  TEST_ASSERT_EQUAL_size_t(voltage_out_four * 10, battery_voltage * 10);
}

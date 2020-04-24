#include "unity.h"

#include "Mockadc.h"
#include "Mockgpio.h"

#include "adc_implementation.c"

void setUp(void) {}
void tearDown(void) {}

void test_adc_implementation__initialization(void) {
  gpio_s gpio = {0};
  gpio__construct_with_function_ExpectAndReturn(GPIO__PORT_0, 25, GPIO__FUNCTION_1, gpio);
  adc__initialize_Expect();

  adc_implementation__initialization();
}

uint16_t voltage_to_adc_bits(float voltage) { return (4095 / 3.3) * voltage; }

float test_adc_implementation__get_battery_voltage(void) {
  float battery_voltage = 7.23;

  adc__get_adc_value_ExpectAndReturn(ADC__CHANNEL_2, voltage_to_adc_bits(battery_voltage));
  TEST_ASSERT_EQUAL_FLOAT(adc_implementation__get_battery_voltage(), battery_voltage);

  battery_voltage = 0.0;

  adc__get_adc_value_ExpectAndReturn(ADC__CHANNEL_2, voltage_to_adc_bits(battery_voltage));
  TEST_ASSERT_EQUAL_FLOAT(adc_implementation__get_battery_voltage(), battery_voltage);

  battery_voltage = 4.67;

  adc__get_adc_value_ExpectAndReturn(ADC__CHANNEL_2, voltage_to_adc_bits(battery_voltage));
  TEST_ASSERT_EQUAL_FLOAT(adc_implementation__get_battery_voltage(), battery_voltage);
}

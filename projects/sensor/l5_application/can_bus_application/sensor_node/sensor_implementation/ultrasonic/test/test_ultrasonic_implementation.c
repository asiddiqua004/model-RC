#include "unity.h"

#include "Mockavg_buffer.h"
#include "Mockgpio2_interrupts.h"
#include "Mocksys_time.h"
#include "Mockultrasonic.h"

#include "ultrasonic_implementation.h"

void setUp(void) {}
void tearDown(void) {}

void test_ultrasonic_implementation__initialize(void) {
  ultrasonic_s ultrasonic = {0};

  ultrasonic__initialize_Expect(&ultrasonic, GPIO__PORT_0, 6U, GPIO__PORT_2, 0U);
  ultrasonic__initialize_Expect(&ultrasonic, GPIO__PORT_0, 7U, GPIO__PORT_2, 1U);
  ultrasonic__initialize_Expect(&ultrasonic, GPIO__PORT_0, 8U, GPIO__PORT_2, 2U);
  ultrasonic__initialize_Expect(&ultrasonic, GPIO__PORT_0, 9U, GPIO__PORT_2, 4U);

  gpio2_interrupts__attach_handler_Expect(0U, FALLING, NULL);
  gpio2_interrupts__attach_handler_IgnoreArg_isr_handler();
  gpio2_interrupts__attach_handler_Expect(1U, FALLING, NULL);
  gpio2_interrupts__attach_handler_IgnoreArg_isr_handler();
  gpio2_interrupts__attach_handler_Expect(2U, FALLING, NULL);
  gpio2_interrupts__attach_handler_IgnoreArg_isr_handler();
  gpio2_interrupts__attach_handler_Expect(4U, FALLING, NULL);
  gpio2_interrupts__attach_handler_IgnoreArg_isr_handler();
  gpio2_interrupts__initialize_Expect();

  avg_buffer_s avg_buffer = {0};
  uint8_t buffer[2];
  const size_t avg_buffer_capacity = 2;

  // Initialize averaging buffers
  avg_buffer__initialize_Expect(&avg_buffer, buffer, avg_buffer_capacity);
  avg_buffer__initialize_Expect(&avg_buffer, buffer, avg_buffer_capacity);
  avg_buffer__initialize_Expect(&avg_buffer, buffer, avg_buffer_capacity);
  avg_buffer__initialize_Expect(&avg_buffer, buffer, avg_buffer_capacity);

  ultrasonic_implementation__initialize();
}

void test_ultrasonic_implementation__initiate_ultrasonics_range(void) {
  ultrasonic_s ultrasonic = {0};

  ultrasonic__initiate_range_Expect(&ultrasonic);
  ultrasonic__initiate_range_Expect(&ultrasonic);
  ultrasonic__initiate_range_Expect(&ultrasonic);
  ultrasonic__initiate_range_Expect(&ultrasonic);

  ultrasonic_implementation__initiate_ultrasonics_range();
}

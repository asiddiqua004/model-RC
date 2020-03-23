
#include "unity.h"

// Mocks
#include "Mockcan_bus_message_handler.h"
#include "Mockgpio.h"
#include "Mockpwm1.h"

// Module includes
#include "motor_control.c"
#include "tesla_model_rc.h"

/******************************************************************************
 *
 *                               D E F I N E S
 *
 ******************************************************************************/

/*****************************************************************************
 *
 *                                 E N U M S
 *
 ******************************************************************************/

/******************************************************************************
 *
 *                              T Y P E D E F S
 *
 ******************************************************************************/

/******************************************************************************
 *
 *        P R I V A T E    F U N C T I O N    D E C L A R A T I O N S
 *
 ******************************************************************************/

/******************************************************************************
 *
 *             P R I V A T E    D A T A    D E F I N I T I O N S
 *
 *****************************************************************************/

/******************************************************************************
 *
 *                     P R I V A T E    F U N C T I O N S
 *
 *****************************************************************************/

/******************************************************************************
 *
 *                T E S T    S E T U P    &    T E A R D O W N
 *
 *****************************************************************************/
void setUp(void) {}

void tearDown(void) {}

/******************************************************************************
 *
 *                                 T E S T S
 *
 *****************************************************************************/

void test_motor_control__initialize(void) {
  gpio_s gpio = {0U};
  pwm1__init_single_edge_Expect(100U);
  gpio__construct_with_function_ExpectAndReturn(GPIO__PORT_2, 0, GPIO__FUNCTION_1, gpio);
  gpio__construct_with_function_ExpectAndReturn(GPIO__PORT_2, 4, GPIO__FUNCTION_1, gpio);
  gpio__construct_as_output_ExpectAndReturn(GPIO__PORT_2, 1, gpio);
  gpio__construct_as_output_ExpectAndReturn(GPIO__PORT_2, 2, gpio);
  motor_control__initialize();
}

void test_motor_control__handle_speed(void) {
  gpio_s gpio = {0U};

  motor_control_state.current_speed_kph_mapped = 0.0f;

  // Stop motor condition
  pwm1__set_duty_cycle_Expect(pwm_channel_speed, motor_control_state.current_speed_kph_mapped);
  gpio__reset_Expect(gpio);
  gpio__reset_Expect(gpio);
  motor_control__handle_speed();

  motor_control_state.current_speed_kph_mapped = 80.0f;

  // Motor forward condition
  pwm1__set_duty_cycle_Expect(pwm_channel_speed, motor_control_state.current_speed_kph_mapped);
  gpio__reset_Expect(gpio);
  gpio__set_Expect(gpio);
  motor_control__handle_speed();
}

void test_motor_control__handle_steering(void) {

  motor_control_state.current_steer_degrees_mapped = -2.0f;

  // Steer left
  pwm1__set_duty_cycle_Expect(pwm_channel_steering, motor_control_state.current_steer_degrees_mapped);
  motor_control__handle_steering();

  motor_control_state.current_steer_degrees_mapped = 2.0f;

  // Steer right
  pwm1__set_duty_cycle_Expect(pwm_channel_steering, motor_control_state.current_steer_degrees_mapped);
  motor_control__handle_steering();
}

void test_motor_control__update_speed_and_steering(void) {
  dbc_DRIVER_MOTOR_CONTROL_s message = {.DRIVER_MOTOR_CONTROL_SPEED_KPH = 5.0f, .DRIVER_MOTOR_CONTROL_STEER = 1.2f};

  motor_control__update_speed_and_steering(&message);
  TEST_ASSERT_EQUAL_FLOAT(50.0f, motor_control_state.current_speed_kph_mapped);
  TEST_ASSERT_EQUAL_FLOAT(21.6f, motor_control_state.current_steer_degrees_mapped);

  // Test outbounds
  memset(&motor_control_state, 0U, sizeof(motor_control_state));

  message.DRIVER_MOTOR_CONTROL_SPEED_KPH = 16.0f;
  message.DRIVER_MOTOR_CONTROL_STEER = 5.0f;

  motor_control__update_speed_and_steering(&message);
  TEST_ASSERT_EQUAL_FLOAT(0.0f, motor_control_state.current_speed_kph_mapped);
  TEST_ASSERT_EQUAL_FLOAT(0.0f, motor_control_state.current_steer_degrees_mapped);
}
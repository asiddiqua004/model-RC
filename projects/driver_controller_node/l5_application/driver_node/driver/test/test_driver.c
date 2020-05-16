#include "unity.h"

#include "Mockexternal_lcd.h"
#include "Mockgpio.h"
#include "driver.c"

#define GEO_ANGLE_LESS 30.0
#define GEO_ANGLE_MORE 45.0
#define GEO_DISTANCE_LESS 8
#define GEO_DISTANCE_MORE 18
#define GEO_DISTANCE_ZERO 0

// clang-format off
                                     // N       L               M              R            B
uint8_t sensor_lookup_table[9][5] = { {0,  OBSTACLE_NEAR, OBSTACLE_NEAR, OBSTACLE_NEAR, OBSTACLE_FAR}, //0 
                                       {1, OBSTACLE_VERY_NEAR, OBSTACLE_FAR, OBSTACLE_FAR, OBSTACLE_FAR}, //1
                                       {1, OBSTACLE_FAR, OBSTACLE_VERY_NEAR,OBSTACLE_FAR, OBSTACLE_FAR}, //2
                                       {1, OBSTACLE_VERY_NEAR, OBSTACLE_FAR,OBSTACLE_VERY_NEAR, OBSTACLE_FAR}, //3
                                       {1, OBSTACLE_FAR, OBSTACLE_NEAR, OBSTACLE_NEAR, OBSTACLE_FAR},//4
                                       {1, OBSTACLE_NEAR, OBSTACLE_NEAR,OBSTACLE_FAR, OBSTACLE_FAR},//5
                                       {1, OBSTACLE_FAR, OBSTACLE_FAR,OBSTACLE_NEAR, OBSTACLE_FAR},//6
                                       {1, OBSTACLE_FAR, OBSTACLE_FAR, OBSTACLE_FAR, OBSTACLE_FAR}, //7
                                       {1, OBSTACLE_NEAR, OBSTACLE_NEAR, OBSTACLE_NEAR, OBSTACLE_FAR}, //8
};
// clang-format  on

// clang-format off
                                     // N  Destination        Current      Distance
uint8_t geo_lookup_table[10][4] = {    {0, GEO_ANGLE_MORE, GEO_ANGLE_MORE, GEO_DISTANCE_MORE}, //0 
                                       {1, GEO_ANGLE_MORE, GEO_ANGLE_MORE, GEO_DISTANCE_MORE}, //1
                                       {1, GEO_ANGLE_LESS, GEO_ANGLE_MORE, GEO_DISTANCE_MORE}, //2
                                       {1, GEO_ANGLE_MORE, GEO_ANGLE_LESS, GEO_DISTANCE_MORE}, //3
                                       {1, GEO_ANGLE_MORE, GEO_ANGLE_MORE, GEO_DISTANCE_LESS},//4
                                       {1, GEO_ANGLE_MORE, GEO_ANGLE_MORE, GEO_DISTANCE_ZERO},//5
                                       {1, GEO_ANGLE_LESS, GEO_ANGLE_MORE, GEO_DISTANCE_LESS},//6
                                       {1, GEO_ANGLE_LESS, GEO_ANGLE_MORE, GEO_DISTANCE_ZERO}, //7
                                       {1, GEO_ANGLE_MORE, GEO_ANGLE_LESS, GEO_DISTANCE_LESS}, //8
                                       {1, GEO_ANGLE_MORE, GEO_ANGLE_LESS, GEO_DISTANCE_ZERO}, //9
};
// clang-format  on

void setUp(void) {

}

void tearDown(void) {
  driver_node.motor = 0;
  driver_node.bridge_sensor = 0;
  driver_node.geo = 0;

  driver_sensor.left = 0;
  driver_sensor.right = 0;
  driver_sensor.front = 0;
  driver_sensor.back = 0;

  driver_geo.current_heading = 0.0f;
  driver_geo.destination_heading = 0.0f;

  driver_navigation_state = 0;
  driver_battery_voltage = 5.5f;
  obstacle_status = NO_MOVEMENT;
  state_machine_state = INIT;
  navigation_status = NO_NAV;
  motor_test_flag = false;
}

void set_sensor_obstacle_status(uint8_t index){
  driver_navigation_state = sensor_lookup_table[index][0];
  driver_sensor.left = sensor_lookup_table[index][1];
  driver_sensor.right = sensor_lookup_table[index][2];
  driver_sensor.front = sensor_lookup_table[index][3];
  driver_sensor.back = sensor_lookup_table[index][4];
}

void set_navigation_obstacle_status(uint8_t index){
  driver_navigation_state = geo_lookup_table[index][0];
  driver_geo.destination_heading = geo_lookup_table[index][1];
  driver_geo.current_heading= geo_lookup_table[index][2];
  geo_distance = geo_lookup_table[index][3];
}

void test__determine_obstacle_status_no_movement(void) {
  obstacle_status = STRAIGHT_MOVEMENT;
  set_sensor_obstacle_status(0U);
  determine_obstacle_status();
  TEST_ASSERT_EQUAL(NO_MOVEMENT, obstacle_status);
}
void test__determine_obstacle_status_soft_right_movement(void) {
  set_sensor_obstacle_status(1U);
  determine_obstacle_status();
  TEST_ASSERT_EQUAL(SOFT_RIGHT_MOVEMENT, obstacle_status);
}

void test__determine_obstacle_status_soft_left_movement(void) {
  set_sensor_obstacle_status(2U);
  determine_obstacle_status();
  TEST_ASSERT_EQUAL(SOFT_LEFT_MOVEMENT, obstacle_status);
}

void test__determine_obstacle_status_hard_right_movement(void) {
  set_sensor_obstacle_status(3U);
  determine_obstacle_status();
  TEST_ASSERT_EQUAL(HARD_RIGHT_MOVEMENT, obstacle_status);
}

void test__determine_obstacle_status_hard_left_movement(void) {
  set_sensor_obstacle_status(4U);
  determine_obstacle_status();
  TEST_ASSERT_EQUAL(HARD_LEFT_MOVEMENT, obstacle_status);
}

void test__determine_obstacle_status_slow_straight_movement(void) {
set_sensor_obstacle_status(5U);
  determine_obstacle_status();
  TEST_ASSERT_EQUAL(SLOW_STRAIGHT_MOVEMENT, obstacle_status);
}

void test__determine_obstacle_status_left_or_right_movement(void) {
set_sensor_obstacle_status(6U);
  determine_obstacle_status();
  TEST_ASSERT_EQUAL(HARD_LEFT_OR_RIGHT_MOVEMENT, obstacle_status);
}

void test__determine_obstacle_status_straight_movement(void) {
set_sensor_obstacle_status(7U);
  determine_obstacle_status();
  TEST_ASSERT_EQUAL(STRAIGHT_MOVEMENT, obstacle_status);
}
void test__determine_obstacle_status_hard_reverse_movement(void) {
set_sensor_obstacle_status(8U);
  determine_obstacle_status();
  TEST_ASSERT_EQUAL(REVERSE_MOVEMENT, obstacle_status);
}

void test__determine_navigation_status_no_nav(void){
  set_navigation_obstacle_status(0U);
  determine_navigation_status();
  TEST_ASSERT_EQUAL(NO_NAV,navigation_status);
}

void test__determine_navigation_status_straight_nav(void){
set_navigation_obstacle_status(1U);
  determine_navigation_status();
  TEST_ASSERT_EQUAL(STRAIGHT_NAV,navigation_status);
}

void test__determine_navigation_status_right_nav(void){
set_navigation_obstacle_status(2U);
  determine_navigation_status();
  TEST_ASSERT_EQUAL(RIGHT_NAV,navigation_status);
}

void test__determine_navigation_status_left_nav(void){
set_navigation_obstacle_status(3U);
  determine_navigation_status();
  TEST_ASSERT_EQUAL(LEFT_NAV,navigation_status);
}


void set_motor_commands_state_transition(motor_s motor){
    set_lcd_motor_status_Expect(motor);
    set_lcd_driver_state_Expect(NULL);
    set_lcd_driver_state_IgnoreArg_incoming_driver_state();
}

void test__compute__motor_commands_state_transition(void){
    motor_s motor= {};
    gpio_s motor_test_switch = {};
    driver_node.motor = 1 ;driver_node.bridge_sensor = 1 ;driver_node.geo = 1;
    state_machine_state = WAIT;
    driver_navigation_state=0;
    motor.speed =MOTOR_STOP_KPH ;
    motor.steer =STEER_STRAIGHT ;
    set_motor_commands_state_transition(motor);
    gpio__get_ExpectAndReturn(motor_test_switch, false);
    compute__motor_commands();
    TEST_ASSERT_EQUAL(WAIT, state_machine_state);
  
    //Commented because of heart_beat_nodes
    // state_machine_state = WAIT;
    // driver_navigation_state=1;
    // driver_node.motor = 1 ;driver_node.bridge_sensor = 0 ;driver_node.geo = 1;
    // set_lcd_motor_status_Expect(motor);
    // set_lcd_driver_state_Expect(NULL);
    // set_lcd_driver_state_IgnoreArg_incoming_driver_state();
    // compute__motor_commands();
    // TEST_ASSERT_EQUAL(WAIT, state_machine_state);

    state_machine_state = WAIT;
    driver_navigation_state=1;
    set_motor_commands_state_transition(motor);
    gpio__get_ExpectAndReturn(motor_test_switch, false);
    compute__motor_commands();
    TEST_ASSERT_EQUAL(OBSTACLE, state_machine_state);

    set_sensor_obstacle_status(7U);
    motor.speed = MOTOR_MED_KPH;
    motor.steer = STEER_STRAIGHT;
    set_motor_commands_state_transition(motor);
    compute__motor_commands();
    TEST_ASSERT_EQUAL(NAVIGATE, state_machine_state);

}

void test__driver__get_motor_commands(void){
  motor_s motor= { };
  motor.speed =MOTOR_STOP_KPH ; 
  motor.steer =STEER_STRAIGHT ;
  gpio_s motor_test_switch = {};
  //INIT state
  set_motor_commands_state_transition(motor);
  gpio__construct_as_input_ExpectAndReturn(GPIO__PORT_0, 15,motor_test_switch);
  driver__get_motor_commands();
  TEST_ASSERT_EQUAL(MOTOR_STOP_KPH, motor_commands.DRIVER_MOTOR_CONTROL_SPEED_KPH);
  TEST_ASSERT_EQUAL(STEER_STRAIGHT,motor_commands.DRIVER_MOTOR_CONTROL_STEER);
  TEST_ASSERT_EQUAL(WAIT,state_machine_state);

  //WAIT state navigation =0
  set_motor_commands_state_transition(motor);
  gpio__get_ExpectAndReturn(motor_test_switch, false);
  driver__get_motor_commands();
  TEST_ASSERT_EQUAL(MOTOR_STOP_KPH, motor_commands.DRIVER_MOTOR_CONTROL_SPEED_KPH);
  TEST_ASSERT_EQUAL(STEER_STRAIGHT,motor_commands.DRIVER_MOTOR_CONTROL_STEER);
  TEST_ASSERT_EQUAL(WAIT,state_machine_state);

  driver_node.motor = 1;
  driver_node.bridge_sensor = 1;
  driver_node.geo = 1;
  driver_navigation_state = 1;

    //WAIT state power down
  set_motor_commands_state_transition(motor);
  gpio__get_ExpectAndReturn(motor_test_switch, false);
  driver_battery_voltage = 3.0f;
  driver__get_motor_commands();
  TEST_ASSERT_EQUAL(MOTOR_STOP_KPH, motor_commands.DRIVER_MOTOR_CONTROL_SPEED_KPH);
  TEST_ASSERT_EQUAL(STEER_STRAIGHT,motor_commands.DRIVER_MOTOR_CONTROL_STEER);
  TEST_ASSERT_EQUAL(WAIT,state_machine_state);

  //WAIT state test-start 
  driver_battery_voltage = 5.5f;
  motor.speed =MOTOR_TEST_KPH ; 
  motor.steer =STEER_STRAIGHT ;
  set_motor_commands_state_transition(motor);
  gpio__get_ExpectAndReturn(motor_test_switch, true);
  driver__get_motor_commands();
  TEST_ASSERT_EQUAL(MOTOR_TEST_KPH, motor_commands.DRIVER_MOTOR_CONTROL_SPEED_KPH);
  TEST_ASSERT_EQUAL(STEER_STRAIGHT,motor_commands.DRIVER_MOTOR_CONTROL_STEER);
  TEST_ASSERT_EQUAL(WAIT,state_machine_state);

  //WAIT state test-stop && navigation =1
  motor.speed =MOTOR_STOP_KPH ; 
  motor.steer =STEER_STRAIGHT ;
  set_motor_commands_state_transition(motor);
  gpio__get_ExpectAndReturn(motor_test_switch, true);
  driver__get_motor_commands();
  TEST_ASSERT_EQUAL(MOTOR_STOP_KPH, motor_commands.DRIVER_MOTOR_CONTROL_SPEED_KPH);
  TEST_ASSERT_EQUAL(STEER_STRAIGHT,motor_commands.DRIVER_MOTOR_CONTROL_STEER);
  TEST_ASSERT_EQUAL(WAIT,state_machine_state);

   //WAIT state test-stop && navigation =1
  set_motor_commands_state_transition(motor);
  gpio__get_ExpectAndReturn(motor_test_switch, false);
  driver__get_motor_commands();
  TEST_ASSERT_EQUAL(MOTOR_STOP_KPH, motor_commands.DRIVER_MOTOR_CONTROL_SPEED_KPH);
  TEST_ASSERT_EQUAL(STEER_STRAIGHT,motor_commands.DRIVER_MOTOR_CONTROL_STEER);
  TEST_ASSERT_EQUAL(OBSTACLE,state_machine_state);
  TEST_ASSERT_EQUAL(NO_MOVEMENT, obstacle_status);

  //OBSTACLE 
  set_sensor_obstacle_status(1U);
  motor.speed =MOTOR_MED_KPH ;
  motor.steer =STEER_SOFT_RIGHT ;
  set_motor_commands_state_transition(motor);
  driver__get_motor_commands();
  TEST_ASSERT_EQUAL(MOTOR_MED_KPH, motor_commands.DRIVER_MOTOR_CONTROL_SPEED_KPH);
  TEST_ASSERT_EQUAL(STEER_SOFT_RIGHT,motor_commands.DRIVER_MOTOR_CONTROL_STEER);
  TEST_ASSERT_EQUAL(OBSTACLE,state_machine_state);
  TEST_ASSERT_EQUAL(SOFT_RIGHT_MOVEMENT, obstacle_status);

  set_sensor_obstacle_status(3U);
  motor.speed = MOTOR_SLOW_KPH;
  motor.steer = STEER_RIGHT;
  set_motor_commands_state_transition(motor);
  driver__get_motor_commands();
  TEST_ASSERT_EQUAL(MOTOR_SLOW_KPH, motor_commands.DRIVER_MOTOR_CONTROL_SPEED_KPH);
  TEST_ASSERT_EQUAL(STEER_RIGHT,motor_commands.DRIVER_MOTOR_CONTROL_STEER);
  TEST_ASSERT_EQUAL(OBSTACLE,state_machine_state);
  TEST_ASSERT_EQUAL(HARD_RIGHT_MOVEMENT, obstacle_status);

  set_sensor_obstacle_status(8U);
  driver_sensor.back = sensor_lookup_table[8][4];
  motor.speed = MOTOR_REV_MED_KPH;
  motor.steer = STEER_SOFT_RIGHT;
  set_motor_commands_state_transition(motor);
  driver__get_motor_commands();
  TEST_ASSERT_EQUAL(MOTOR_REV_MED_KPH, motor_commands.DRIVER_MOTOR_CONTROL_SPEED_KPH);
  TEST_ASSERT_EQUAL(STEER_SOFT_RIGHT,motor_commands.DRIVER_MOTOR_CONTROL_STEER);
  TEST_ASSERT_EQUAL(OBSTACLE,state_machine_state);
  TEST_ASSERT_EQUAL(REVERSE_MOVEMENT, obstacle_status);
 
  driver_sensor.back = 5;
  motor.speed = MOTOR_STOP_KPH;
  motor.steer = STEER_STRAIGHT;
  set_motor_commands_state_transition(motor);
  driver__get_motor_commands();
  TEST_ASSERT_EQUAL(MOTOR_STOP_KPH, motor_commands.DRIVER_MOTOR_CONTROL_SPEED_KPH);
  TEST_ASSERT_EQUAL(STEER_STRAIGHT,motor_commands.DRIVER_MOTOR_CONTROL_STEER);
  TEST_ASSERT_EQUAL(OBSTACLE,state_machine_state);
  TEST_ASSERT_EQUAL(STOP, obstacle_status);

 //Navigation testing
  set_sensor_obstacle_status(7U);
  set_navigation_obstacle_status(1U);
  motor.speed = MOTOR_MED_KPH;
  motor.steer = STEER_STRAIGHT;
  set_motor_commands_state_transition(motor);
  driver__get_motor_commands();
  TEST_ASSERT_EQUAL(NAVIGATE, state_machine_state);
  TEST_ASSERT_EQUAL(STRAIGHT_MOVEMENT, obstacle_status);
  TEST_ASSERT_EQUAL(NO_NAV,navigation_status);


 }


void test__driver__get_motor_commands_navigation_speed_zero(void){
  motor_s motor= {0};
 //Navigation testing
  state_machine_state = OBSTACLE;
  driver_node.motor = 1 ;driver_node.bridge_sensor = 1 ;driver_node.geo = 1;

  set_sensor_obstacle_status(7U);
  motor.speed = MOTOR_MED_KPH;
  motor.steer = STEER_STRAIGHT;
  set_motor_commands_state_transition(motor);
  driver__get_motor_commands();
  TEST_ASSERT_EQUAL(NAVIGATE, state_machine_state);
  TEST_ASSERT_EQUAL(STRAIGHT_MOVEMENT, obstacle_status);
  TEST_ASSERT_EQUAL(NO_NAV,navigation_status);

  set_navigation_obstacle_status(5U);
  motor.speed = MOTOR_STOP_KPH;
  motor.steer = STEER_STRAIGHT;
  set_motor_commands_state_transition(motor);
  driver__get_motor_commands();
  TEST_ASSERT_EQUAL(STRAIGHT_NAV,navigation_status);

  set_navigation_obstacle_status(5U);
  set_motor_commands_state_transition(motor);
  driver__get_motor_commands();
  TEST_ASSERT_EQUAL(STRAIGHT_NAV,navigation_status);

  set_navigation_obstacle_status(7U);
  set_motor_commands_state_transition(motor);
  driver__get_motor_commands();
  TEST_ASSERT_EQUAL(RIGHT_NAV,navigation_status);

  set_navigation_obstacle_status(9U);
  set_motor_commands_state_transition(motor);
  driver__get_motor_commands();
  TEST_ASSERT_EQUAL(LEFT_NAV,navigation_status);
}

void test__driver__get_motor_commands_navigation_speed_med(void){
 motor_s motor= {0};
 //Navigation testing
  state_machine_state = OBSTACLE;
  driver_node.motor = 1 ;driver_node.bridge_sensor = 1 ;driver_node.geo = 1;
set_sensor_obstacle_status(7U);
  motor.speed = MOTOR_MED_KPH;
  motor.steer = STEER_STRAIGHT;
  set_motor_commands_state_transition(motor);
  driver__get_motor_commands();
  TEST_ASSERT_EQUAL(NAVIGATE, state_machine_state);
  TEST_ASSERT_EQUAL(STRAIGHT_MOVEMENT, obstacle_status);
  TEST_ASSERT_EQUAL(NO_NAV,navigation_status);


  set_navigation_obstacle_status(1U);
  get_navigation_steering();
  motor.steer = navigation_steering;
  set_motor_commands_state_transition(motor);
  driver__get_motor_commands();
  TEST_ASSERT_EQUAL(STRAIGHT_NAV,navigation_status);

  set_navigation_obstacle_status(2U);
  get_navigation_steering();
  motor.steer = navigation_steering;
  TEST_ASSERT_EQUAL(((2 * (driver_geo.destination_heading - driver_geo.current_heading))/45)+2,navigation_steering);
  set_motor_commands_state_transition(motor);
  driver__get_motor_commands();
  TEST_ASSERT_EQUAL(RIGHT_NAV,navigation_status);

set_navigation_obstacle_status(3U);
  get_navigation_steering();
  motor.steer = navigation_steering;
  TEST_ASSERT_EQUAL(((2 * (driver_geo.destination_heading - driver_geo.current_heading))/45)+2,navigation_steering);
  set_motor_commands_state_transition(motor);
  driver__get_motor_commands();
  TEST_ASSERT_EQUAL(LEFT_NAV,navigation_status);

}

void test__driver__get_motor_commands_navigation_speed_slow(void){
 motor_s motor= {0};
 //Navigation testing
  state_machine_state = OBSTACLE;
  driver_node.motor = 1 ;driver_node.bridge_sensor = 1 ;driver_node.geo = 1;
set_sensor_obstacle_status(7U);
  motor.speed = MOTOR_MED_KPH;
  motor.steer = STEER_STRAIGHT;
  set_motor_commands_state_transition(motor);
  driver__get_motor_commands();
  TEST_ASSERT_EQUAL(NAVIGATE, state_machine_state);
  TEST_ASSERT_EQUAL(STRAIGHT_MOVEMENT, obstacle_status);
  TEST_ASSERT_EQUAL(NO_NAV,navigation_status);


set_navigation_obstacle_status(4U);
  motor.speed = MOTOR_SLOW_KPH;
  get_navigation_steering();
  motor.steer = navigation_steering;
  TEST_ASSERT_EQUAL(((2 * (driver_geo.destination_heading - driver_geo.current_heading))/45)+2,navigation_steering);
  set_motor_commands_state_transition(motor);
  driver__get_motor_commands();
  TEST_ASSERT_EQUAL(STRAIGHT_NAV,navigation_status);

set_navigation_obstacle_status(6U);
  get_navigation_steering();
  motor.steer = navigation_steering;
  TEST_ASSERT_EQUAL(((2 * (driver_geo.destination_heading - driver_geo.current_heading))/45)+2,navigation_steering);
  set_motor_commands_state_transition(motor);
  driver__get_motor_commands();
  TEST_ASSERT_EQUAL(RIGHT_NAV,navigation_status);

set_navigation_obstacle_status(8U);
  get_navigation_steering();
  motor.steer = navigation_steering;
  TEST_ASSERT_EQUAL(((2 * (driver_geo.destination_heading - driver_geo.current_heading))/45)+2,navigation_steering);
  set_motor_commands_state_transition(motor);
  driver__get_motor_commands();
  TEST_ASSERT_EQUAL(LEFT_NAV,navigation_status);

}

void test__get_navigation_steering(void){
  driver_geo.destination_heading  = 360;
  get_navigation_steering();
  TEST_ASSERT_EQUAL_FLOAT( 2 ,navigation_steering);

  driver_geo.destination_heading  = -360;
  get_navigation_steering();
  TEST_ASSERT_EQUAL_FLOAT( 2 ,navigation_steering);

  driver_geo.destination_heading  = 180;
  get_navigation_steering();
  TEST_ASSERT_EQUAL_FLOAT( 4 ,navigation_steering);

  driver_geo.destination_heading  = -180;
  get_navigation_steering();
  TEST_ASSERT_EQUAL_FLOAT( 0 ,navigation_steering);

  driver_geo.destination_heading  = 45;
  get_navigation_steering();
  TEST_ASSERT_EQUAL_FLOAT( 4 ,navigation_steering);

  driver_geo.destination_heading  = -45;
  get_navigation_steering();
  TEST_ASSERT_EQUAL_FLOAT( 0 ,navigation_steering);

  driver_geo.destination_heading  = 60;
  driver_geo.current_heading = 30;
  get_navigation_steering();
  TEST_ASSERT_EQUAL_FLOAT( 3.333334 ,navigation_steering);

  driver_geo.destination_heading  = 30;
  driver_geo.current_heading = 60;
  get_navigation_steering();
  TEST_ASSERT_EQUAL_FLOAT( 0.6666666 ,navigation_steering);

}

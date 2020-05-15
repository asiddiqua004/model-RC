#include "driver.h"

/****************PRIVATE VARIABLES ****************/
static node_alive_s driver_node = {
    .motor = 0,
    .bridge_sensor = 0,
    .geo = 0,
};
static motor_s driver_motor = {
    .speed = 0,
    .steer = 0,
};
static sensor_s driver_sensor = {
    .left = 0,
    .right = 0,
    .front = 0,
    .back = 0,
};

static geo_s driver_geo = {
    .current_heading = 0.0f,
    .destination_heading = 0.0f,
};

static uint8_t driver_navigation_state = 1;
static float driver_battery_voltage = 0.0f;

static dbc_DRIVER_MOTOR_CONTROL_s motor_commands = {0};

static state_machine_state_e state_machine_state = INIT;

static obstacle_status_e obstacle_status = NO_MOVEMENT; // 0

static navigation_status_e navigation_status = NO_NAV;
static float navigation_steering = 0.0f;
static int8_t geo_distance = 5;

/****************PRIVATE FUNCTIONS ****************/

static void set_motor_status(float speed, float steer) {
  motor_commands.DRIVER_MOTOR_CONTROL_SPEED_KPH = driver_motor.speed = speed;
  motor_commands.DRIVER_MOTOR_CONTROL_STEER = driver_motor.steer = steer;
  set_lcd_motor_status(driver_motor);
}
// TODO implement heartbeat functionality
static bool all_nodes_alive(void) {
  // bool status = false;
  // if (driver_node.motor == 1 && driver_node.bridge_sensor == 1 && driver_node.geo == 1) {
  //   status = true;
  // }
  // return status;
  return true;
}
// TODO implement reverse logic
/* obstacle_status
Nav   LEFT	MIDDLE RIGHT  BACK    	Decision         Number
0    x	      x	     x  	 x        OB-NM               0 //No movement
1    0	      0	     0	   x        OB-STR              16
1    0	      0	     1     x        OB-SLM              18
1    0	      1	     0  	 x        OB-HLM OR OB-HRM    20
1    0	      1	     1  	 x        OB-HLM              22
1    1	      0	     0  	 x        OB-SRM              24
1    1	      0	     1  	 x        OB-SSM              26//Slow staright movement
1    1	      1	     0  	 x        OB-HRM              28
1    1	      1	     1  	 0        OB-REV              30
1    1	      1	     1  	 1        OB-STOP             31
*/
static void determine_obstacle_status(void) {
  if (0 == driver_navigation_state) {
    obstacle_status = 0;
    return;
  } else {
    obstacle_status |= (1 << 4);
  }
  if (driver_sensor.left <= OBSTACLE_NEAR || driver_sensor.left <= OBSTACLE_MID) {
    if (driver_sensor.left <= OBSTACLE_VERY_NEAR - 2) {
      // do nothing
    } else {
      obstacle_status |= (1 << 3);
    }
  } else if (driver_sensor.left > OBSTACLE_MID) {
    obstacle_status &= ~(1 << 3);
  }
  if (driver_sensor.right <= OBSTACLE_NEAR || driver_sensor.right <= OBSTACLE_MID) {
    if (driver_sensor.right <= OBSTACLE_VERY_NEAR - 2) {
      // do nothing
    } else {
      obstacle_status |= (1 << 1);
    }
  } else if (driver_sensor.right > OBSTACLE_MID) {
    obstacle_status &= ~(1 << 1);
  }
  if (driver_sensor.front <= OBSTACLE_NEAR || driver_sensor.front <= OBSTACLE_MID) {
    if (driver_sensor.front <= OBSTACLE_VERY_NEAR - 2) {
      // do nothing
    } else {
      obstacle_status |= (1 << 2);
    }
  } else if (driver_sensor.front > OBSTACLE_MID) {
    obstacle_status &= ~(1 << 2);
  }
  if (obstacle_status < 30) {
    obstacle_status &= ~(1 << 0);
  } else if (obstacle_status == 30) {
    if (driver_sensor.back <= OBSTACLE_NEAR || driver_sensor.back <= OBSTACLE_MID) {
      if (driver_sensor.back <= OBSTACLE_VERY_NEAR - 2) {
        // do nothing
      } else {
        obstacle_status |= (1 << 0);
      }
    } else if (driver_sensor.back > OBSTACLE_MID) {
      obstacle_status &= ~(1 << 0);
    }
  }
}

/* navigation_status
 (distance>10)
Nav Destination        Current	 	 Decision             Number //N L S R
0      x	               x	       	NAV-NM               0   //No navigation
1      x	     ==        x	     	  NAV-STR              10   //1 0 1 0
1      x	     <         x	       	NAV-RM               9   //1 0 0 1
1      x	     >         x	       	NAV-LM               12  //1 1 0 0
 (distance<10)
0      x	               x	       	NAV-NM               0   //No navigation
1      x	     ==        x	     	  NAV-STR              10   //1 0 0 0
1      x	     <         x	       	NAV-RM               9   //1 0 0 1
1      x	     >         x	       	NAV-LM               12  //1 1 0 0
 (distance<2)
1      x	               x	       	NAV-STP              0
*/
// TODO: implement proper steering values.
void get_navigation_steering(void) {
  float turning_angle = driver_geo.destination_heading - driver_geo.current_heading;
  if (turning_angle > 180.0) {
    turning_angle -= 360.0;
  } else if (turning_angle < -180.0) {
    turning_angle += 360.0;
  }

  turning_angle = (turning_angle > 45.0) ? 45.0 : turning_angle;
  turning_angle = (turning_angle < -45.0) ? -45.0 : turning_angle;

  // navigation_steering = (2 * turning_angle) / 45;

  navigation_steering = (2.0 * (turning_angle / 45)) + 2.0;
}

static void get_navigation_direction(void) {
  if (driver_geo.destination_heading == driver_geo.current_heading) {
    navigation_status &= ~(7 << 0);
    get_navigation_steering();
    // navigation_steering = 0.0f;
    navigation_status |= (1 << 1);
  } else if (driver_geo.destination_heading > driver_geo.current_heading) {
    navigation_status &= ~(7 << 0);
    get_navigation_steering();
    // navigation_steering = -2.0f;
    navigation_status |= (1 << 2);
  } else if (driver_geo.destination_heading < driver_geo.current_heading) {
    navigation_status &= ~(7 << 0);
    get_navigation_steering();
    // navigation_steering = 2.0f;
    navigation_status |= (1 << 0);
  }
}

static void determine_navigation_status(void) {
  if (0 == driver_navigation_state) {
    navigation_status = 0;
    return;
  } else {
    navigation_status |= (1 << 3);
  }
  get_navigation_direction();
}

static void compute__motor_commands(void) {

  switch (state_machine_state) {
  case INIT:
    set_motor_status(MOTOR_STOP_KPH, STEER_STRAIGHT);
    state_machine_state = WAIT;
    set_lcd_driver_state("INIT");
    break;
  case WAIT:
    set_motor_status(MOTOR_STOP_KPH, STEER_STRAIGHT);
    if (0 != driver_navigation_state && all_nodes_alive()) {
      state_machine_state = OBSTACLE;
    } else {
      state_machine_state = WAIT;
    }
    set_lcd_driver_state("WAIT");
    break;
  case OBSTACLE:
    determine_obstacle_status();
    switch (obstacle_status) {
    case NO_MOVEMENT:
      set_lcd_driver_state("OB-NM");
      set_motor_status(MOTOR_STOP_KPH, STEER_STRAIGHT);
      break;
    case STRAIGHT_MOVEMENT:
      state_machine_state = NAVIGATE;
      set_lcd_driver_state("OB-STR");
      set_motor_status(MOTOR_MED_KPH, STEER_STRAIGHT);
      break;
    case SOFT_LEFT_MOVEMENT:
      set_lcd_driver_state("OB-SLM");
      set_motor_status(MOTOR_MED_KPH, STEER_SOFT_LEFT);
      break;
    case HARD_LEFT_OR_RIGHT_MOVEMENT:
      set_lcd_driver_state("OB-HLRM");
      if (driver_sensor.left < driver_sensor.right) {
        set_motor_status(MOTOR_SLOW_KPH, STEER_RIGHT);
      } else {
        set_motor_status(MOTOR_SLOW_KPH, STEER_LEFT);
      }
      break;
    case HARD_LEFT_MOVEMENT:
      set_lcd_driver_state("OB-HLM");
      set_motor_status(MOTOR_SLOW_KPH, STEER_LEFT);
      break;
    case SOFT_RIGHT_MOVEMENT:
      set_lcd_driver_state("OB-SRM");
      set_motor_status(MOTOR_MED_KPH, STEER_SOFT_RIGHT);
      break;
    case SLOW_STRAIGHT_MOVEMENT:
      set_lcd_driver_state("OB-SSM");
      set_motor_status(MOTOR_SLOW_KPH, STEER_STRAIGHT);
      break;
    case HARD_RIGHT_MOVEMENT:
      set_lcd_driver_state("OB-HRM");
      set_motor_status(MOTOR_SLOW_KPH, STEER_RIGHT);
      break;
    case REVERSE_MOVEMENT:
      set_lcd_driver_state("OB-REV");
      set_motor_status(MOTOR_REV_MED_KPH, STEER_STRAIGHT);
      break;
    case STOP:
      set_lcd_driver_state("OB-STP");
      set_motor_status(MOTOR_STOP_KPH, STEER_STRAIGHT);
      break;
    default:
      set_lcd_driver_state("OB-NMD");
      set_motor_status(MOTOR_STOP_KPH, STEER_STRAIGHT);
      break;
    }
    break;

  case NAVIGATE:
    determine_navigation_status();
    determine_obstacle_status();
    if (obstacle_status == STRAIGHT_MOVEMENT) {
      switch (navigation_status) {
      case NO_NAV:
        set_lcd_driver_state("NAV_NM");
        set_motor_status(MOTOR_STOP_KPH, STEER_STRAIGHT);
        break;
      case LEFT_NAV:
        set_lcd_driver_state("NAV_LM");
        if (geo_distance > 10) {
          set_motor_status(MOTOR_MED_KPH, navigation_steering);
        }
        if (geo_distance < 1) {
          set_motor_status(MOTOR_STOP_KPH, STEER_STRAIGHT);
        }
        if (geo_distance >= 1 && geo_distance <= 10) {
          set_motor_status(MOTOR_SLOW_KPH, navigation_steering);
        }
        break;
      case STRAIGHT_NAV:
        set_lcd_driver_state("NAV_STR");
        if (geo_distance > 10) {
          set_motor_status(MOTOR_MED_KPH, navigation_steering);
        }
        if (geo_distance < 1) {
          set_motor_status(MOTOR_STOP_KPH, STEER_STRAIGHT);
        }
        if (geo_distance >= 1 && geo_distance <= 10) {
          set_motor_status(MOTOR_SLOW_KPH, navigation_steering);
        }
        break;
      case RIGHT_NAV:
        set_lcd_driver_state("NAV_RM");
        if (geo_distance > 10) {
          set_motor_status(MOTOR_MED_KPH, navigation_steering);
        }
        if (geo_distance < 1) {
          set_motor_status(MOTOR_STOP_KPH, STEER_STRAIGHT);
        }
        if (geo_distance >= 1 && geo_distance <= 10) {
          set_motor_status(MOTOR_SLOW_KPH, navigation_steering);
        }
        break;
      default:
        set_lcd_driver_state("NAV_NMD");
        set_motor_status(MOTOR_STOP_KPH, STEER_STRAIGHT);
        break;
      }
    } else if (obstacle_status != STRAIGHT_MOVEMENT) {
      state_machine_state = OBSTACLE;
    }
    break;
  }
}

/****************PUBLIC FUNCTIONS ****************/
void driver__process_heartbeats(dbc_MOTOR_HEARTBEAT_s motor_hbt, dbc_BRIDGE_SENSOR_HEARTBEAT_s bridge_sensor_hbt,
                                dbc_GEO_HEARTBEAT_s geo_hbt) {
  driver_node.motor = motor_hbt.MOTOR_HEARTBEAT_CMD;
  driver_node.bridge_sensor = bridge_sensor_hbt.BRIDGE_SENSOR_HEARTBEAT_CMD;
  driver_node.geo = geo_hbt.GEO_HEARTBEAT_CMD;
  set_lcd_node_heart_beats(driver_node);
}

void driver__process_sensor_input(dbc_BRIDGE_SENSOR_SONARS_s incoming_sensor_data) {
  driver_sensor.left = incoming_sensor_data.BRIDGE_SENSOR_SONARS_LEFT;
  driver_sensor.right = incoming_sensor_data.BRIDGE_SENSOR_SONARS_RIGHT;
  driver_sensor.back = incoming_sensor_data.BRIDGE_SENSOR_SONARS_BACK;
  driver_sensor.front = incoming_sensor_data.BRIDGE_SENSOR_SONARS_FRONT;
  set_lcd_sensor_status(driver_sensor);
}
void driver__process_geo_controller_directions(dbc_GEO_GPS_COMPASS_HEADINGS_s incoming_geo_heading) {
  driver_geo.current_heading = incoming_geo_heading.GEO_GPS_COMPASS_HEADINGS_CURRENT;
  driver_geo.destination_heading = incoming_geo_heading.GEO_GPS_COMPASS_HEADINGS_DESTINATION;
  set_lcd_compass_status(driver_geo);
}
// TODO implement navigation functionality
void driver__process_navigation_state(dbc_BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE_s navigation_state) {
  // driver_navigation_state = navigation_state.BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE;
}
void driver__process_battery_voltage(dbc_BRIDGE_SENSOR_VOLTAGE_s battery_voltage) {
  driver_battery_voltage = battery_voltage.BRIDGE_SENSOR_VOLTAGE;
  set_lcd_battery_voltage(driver_battery_voltage);
}
// TODO implement mia functionality
void driver__process_mia_signal(bool enable_mia_screen, uint8_t mia_replacement_signal_number) {
  if (enable_mia_screen == true) {
    set_lcd_mia_replacement_signal_number(mia_replacement_signal_number);
  } else {
    set_lcd_mia_replacement_signal_number(0);
  }
}

bool driver__get_node_heart_beats(void) { return (driver_node.bridge_sensor & driver_node.geo & driver_node.motor); }

dbc_DRIVER_MOTOR_CONTROL_s driver__get_motor_commands(void) {
  compute__motor_commands();
  return motor_commands;
}

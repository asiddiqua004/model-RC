#pragma once

#include "tesla_model_rc.h"

void driver__process_input(dbc_SENSOR_SONARS_s sensor_data);
dbc_DRIVER_MOTOR_CONTROL_s driver__get_motor_commands(void);
#include <stdint.h>

#include "can_bus_mia_configurations.h"
#include "tesla_model_rc.h"

/*******************************************************************************
 *
 *               P R I V A T E    D A T A    D E F I N I T I O N S
 *
 ******************************************************************************/
// Private data for modules should be rare because all data should be part
// of the module's struct instance

const uint32_t dbc_mia_threshold_DRIVER_HEARTBEAT = 3 * 1000;
const uint32_t dbc_mia_threshold_MOTOR_HEARTBEAT = 3 * 1000;
const uint32_t dbc_mia_threshold_DRIVER_MOTOR_CONTROL = 3 * 100;
const uint32_t dbc_mia_threshold_SENSOR_SONARS = 3 * 50;

const dbc_DRIVER_HEARTBEAT_s dbc_mia_replacement_DRIVER_HEARTBEAT;
const dbc_MOTOR_HEARTBEAT_s dbc_mia_replacement_MOTOR_HEARTBEAT;
const dbc_DRIVER_MOTOR_CONTROL_s dbc_mia_replacement_DRIVER_MOTOR_CONTROL = {.DRIVER_MOTOR_CONTROL_SPEED_KPH = 0.0f,
                                                                             .DRIVER_MOTOR_CONTROL_STEER = 0.0f};
const dbc_SENSOR_SONARS_s dbc_mia_replacement_SENSOR_SONARS;

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
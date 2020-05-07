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
const uint32_t dbc_mia_threshold_MOTOR_ACTUAL_SPEED = 3 * 50;
const uint32_t dbc_mia_threshold_BRIDGE_SENSOR_SONARS = 3 * 50;
const uint32_t dbc_mia_threshold_GEO_GPS_COMPASS_HEADINGS = 3 * 100;
const uint32_t dbc_mia_threshold_BRIDGE_SENSOR_GPS_HEADINGS = 3 * 100;
const uint32_t dbc_mia_threshold_BRIDGE_SENSOR_VOLTAGE = 3 * 100;
const uint32_t dbc_mia_threshold_BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE = 3 * 100;

const dbc_DRIVER_HEARTBEAT_s dbc_mia_replacement_DRIVER_HEARTBEAT;
const dbc_MOTOR_HEARTBEAT_s dbc_mia_replacement_MOTOR_HEARTBEAT;
const dbc_DRIVER_MOTOR_CONTROL_s dbc_mia_replacement_DRIVER_MOTOR_CONTROL;
const dbc_MOTOR_ACTUAL_SPEED_s dbc_mia_replacement_MOTOR_ACTUAL_SPEED;
const dbc_BRIDGE_SENSOR_SONARS_s dbc_mia_replacement_BRIDGE_SENSOR_SONARS;
const dbc_GEO_GPS_COMPASS_HEADINGS_s dbc_mia_replacement_GEO_GPS_COMPASS_HEADINGS;
const dbc_BRIDGE_SENSOR_GPS_HEADINGS_s dbc_mia_replacement_BRIDGE_SENSOR_GPS_HEADINGS;
const dbc_BRIDGE_SENSOR_VOLTAGE_s dbc_mia_replacement_BRIDGE_SENSOR_VOLTAGE;
const dbc_BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE_s dbc_mia_replacement_BRIDGE_SENSOR_VEHICLE_NAVIGATION_STATE;

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
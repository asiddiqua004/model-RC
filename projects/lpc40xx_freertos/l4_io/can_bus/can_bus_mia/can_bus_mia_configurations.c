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

const uint32_t dbc_mia_threshold_ACCELEROMETER_VALUE = 3 * 100;

const dbc_ACCELEROMETER_VALUE_s dbc_mia_replacement_ACCELEROMETER_VALUE;

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
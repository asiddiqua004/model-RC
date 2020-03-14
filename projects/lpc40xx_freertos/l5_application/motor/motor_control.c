#include <inttypes.h>
#include <stdio.h>

#include "can_bus_message_handler.h"

#include "board_io.h"
#include "gpio.h"
#include "tesla_model_rc.h"

/*******************************************************************************
 *
 *               P R I V A T E    D A T A    D E F I N I T I O N S
 *
 ******************************************************************************/

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

void motor_control__run_once(void) {
  // TODO: Handle recevied messages for speed, steer, and commands
}
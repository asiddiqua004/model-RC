#include "can_bus_initializer.h"

/*******************************************************************************
 *
 *               P R I V A T E    D A T A    D E F I N I T I O N S
 *
 ******************************************************************************/
// Private data for modules should be rare because all data should be part
// of the module's struct instance

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

bool can_bus_initializer__init(void) {
  const bool can_init_status = can__init(can_num, can_baudrate_kbps, can_rx_queue_size, can_tx_queue_size,
                                         can_bus_off_callback, can_bus_data_overrun_callback);
  if (can_init_status) {
    can__bypass_filter_accept_all_msgs();
    can__reset_bus(can_num);
  }

  return can_init_status;
}

void can_bus_initializer__handle_bus_off(void) {
  if (can__is_bus_off(can_num)) {
    can__reset_bus(can_num);
  }
}
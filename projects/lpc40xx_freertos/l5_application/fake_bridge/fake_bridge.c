// #define BRIDGE_NODE 1

#ifndef BRIDGE_NODE
#define BRIDGE_NODE 0
#endif

#include "fake_bridge.h"
#include "can_bus.h"
#include "can_bus_message_handler.h"
#include "tesla_model_rc.h"

/*******************************************************************************
 *
 *               P R I V A T E    D A T A    D E F I N I T I O N S
 *
 ******************************************************************************/

#if BRIDGE_NODE == 1
static const dbc_BRIDGE_GPS_HEADINGS_s bridge_gps_headings_message = {.BRIDGE_GPS_HEADINGS_LATITUDE = 37.3389322f,
                                                                      .BRIDGE_GPS_HEADINGS_LONGITUDE = 121.8837316f};
#endif

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

#if BRIDGE_NODE == 1
bool fake_bridge__transmit_gps_messages_10Hz(void) {
  bool sent_gps_message_success = false;
  sent_gps_message_success = dbc_encode_and_send_BRIDGE_GPS_HEADINGS(NULL, &bridge_gps_headings_message);
  return sent_gps_message_success;
}
#endif

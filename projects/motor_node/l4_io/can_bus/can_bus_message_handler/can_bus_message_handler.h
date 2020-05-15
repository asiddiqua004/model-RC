/**
 * @file
 * Can Bus Initializer Module
 *
 * Thread Safety Assessment:
 * Not Thread Safe
 */
#pragma once

#include <stdbool.h>

#include "can_bus.h"

/*******************************************************************************
 *
 *                               D E F I N E S
 *
 ******************************************************************************/
const static can__num_e can_bus_num = can1;

typedef enum {
  CAN_BUS_MESSAGE_HANDLER__MESSAGE_TEST,
} can_bus_messsage_handler__message_type_e;

/*******************************************************************************
 *
 *                                 E N U M S
 *
 ******************************************************************************/

/*******************************************************************************
 *
 *                               T Y P E D E F S
 *
 ******************************************************************************/

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/

bool can_bus_message_handler__send_test(void);

void can_bus_message_handler__manage_mia_10Hz(void);

void can_bus_message_handler__transmit_messages_10Hz(void);

void can_bus_message_handler__handle_all_incoming_messages_10Hz(void);

bool dbc_send_can_message(void *argument_from_dbc_encode_and_send, uint32_t message_id, const uint8_t bytes[8],
                          uint8_t dlc);
void can_bus_message_handler__transmit_messages_10Hz(void);
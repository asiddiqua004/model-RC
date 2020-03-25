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

static const can__num_e can_num = can1;
static const uint32_t can_baudrate_kbps = 250U;
static const uint16_t can_rx_queue_size = 75U;
static const uint16_t can_tx_queue_size = 75U;
static const can_void_func_t can_bus_off_callback;
static const can_void_func_t can_bus_data_overrun_callback;

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
bool can_bus_initializer__init(void);

void can_bus_initializer__handle_bus_off(void);
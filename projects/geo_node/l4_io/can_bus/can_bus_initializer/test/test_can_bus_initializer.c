#include "unity.h"

// Mocks
#include "Mockcan_bus.h"

// Module includes
#include "can_bus_initializer.c"

/******************************************************************************
 *
 *                               D E F I N E S
 *
 ******************************************************************************/

/*****************************************************************************
 *
 *                                 E N U M S
 *
 ******************************************************************************/

/******************************************************************************
 *
 *                              T Y P E D E F S
 *
 ******************************************************************************/

/******************************************************************************
 *
 *        P R I V A T E    F U N C T I O N    D E C L A R A T I O N S
 *
 ******************************************************************************/

/******************************************************************************
 *
 *             P R I V A T E    D A T A    D E F I N I T I O N S
 *
 *****************************************************************************/

/******************************************************************************
 *
 *                     P R I V A T E    F U N C T I O N S
 *
 *****************************************************************************/

/******************************************************************************
 *
 *                T E S T    S E T U P    &    T E A R D O W N
 *
 *****************************************************************************/
void setUp(void) {}

void tearDown(void) {}

/******************************************************************************
 *
 *                                 T E S T S
 *
 *****************************************************************************/

void test_init_success(void) {
  can__init_ExpectAndReturn(can_num, can_baudrate_kbps, can_rx_queue_size, can_tx_queue_size, can_bus_off_callback,
                            can_bus_data_overrun_callback, true);
  can__bypass_filter_accept_all_msgs_Expect();
  can__reset_bus_Expect(can_num);
  const bool can_init_status = can_bus_initializer__init();
  TEST_ASSERT_TRUE_MESSAGE(can_init_status, "can_bus_initializer__init() failed to pass the successful case.");
}

void test_init_failure(void) {
  can__init_ExpectAndReturn(can_num, can_baudrate_kbps, can_rx_queue_size, can_tx_queue_size, can_bus_off_callback,
                            can_bus_data_overrun_callback, false);
  const bool can_init_status = can_bus_initializer__init();
  TEST_ASSERT_FALSE_MESSAGE(can_init_status, "can_bus_initializer__init() failed to pass the failure case.");
}
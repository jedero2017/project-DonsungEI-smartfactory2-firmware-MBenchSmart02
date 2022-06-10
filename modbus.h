/*
 * modbus.h
 *
 *  Created on: 2021. 12. 15.
 *      Author: sjhong
 */

#ifndef MODBUS_H_
#define MODBUS_H_


#include <stdbool.h>
#include <ti/drivers/UART.h>

extern UART_Handle modbus_pm_handle;
extern UART_Handle modbus_tm_handle;


void modbus_init();

bool modbus_read_input_reg(UART_Handle h_uart, int dev_no, int start_addr, int quantity, uint8_t* data);

uint16_t modbus_get_crc(uint8_t * buf, uint16_t len);


#endif /* MODBUS_H_ */

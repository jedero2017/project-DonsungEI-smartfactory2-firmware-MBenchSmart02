/*
 * uart.h
 *
 *  Created on: 2019. 11. 5.
 *      Author: sjhong
 */

#ifndef UART_H_
#define UART_H_

//UART_Handle h_uart_lcd;

void uart0_init();
void * uart0_thread();
void uart0_cb(UART_Handle handle, void *buf, size_t count);


void uart0_msg(uint8_t* msg, int len);

#endif /* UART_H_ */

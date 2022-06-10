/*
 * uart.c
 *
 *  Created on: 2019. 10. 31.
 *      Author: sjhong
 */
#include <ti/drivers/UART.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Clock.h>

#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Semaphore.h>

#include "uart.h"

#include "ti_drivers_config.h"

#include "procedure.h"
#include "command.h"


Semaphore_Handle uart0_read_sem;

#define UART_BUFFER_SIZE 64
unsigned char uart_buffer[UART_BUFFER_SIZE];
int uart_buffer_inx = 0;

UART_Handle uart0_handle;

void uart0_init()
{
    UART_Params uartParams;

    UART_init();

    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;
    uartParams.readMode = UART_MODE_CALLBACK;
    uartParams.readCallback = uart0_cb;

    uart0_handle = UART_open(CONFIG_UART_0, &uartParams);

}

void * uart0_thread()
{
    char        input;

    uart0_read_sem = Semaphore_create(0, NULL, NULL);


    while(1)
    {
        UART_read(uart0_handle, &input, 1);
        Semaphore_pend(uart0_read_sem, BIOS_WAIT_FOREVER);

        if(uart_buffer_inx < UART_BUFFER_SIZE)
        {
            uart_buffer[uart_buffer_inx] = input;
            uart_buffer_inx++;

        }
        else
            uart_buffer_inx = 0;

        if(input == '\r')
        {
            command_parse((char*)uart_buffer, uart_buffer_inx, PORT_TYPE_UART, NULL);
            //uart_msg(cmd_send_data, cmd_send_data_len);
            uart_buffer_inx = 0;
        }

        if(input == '\n')  //skip
            uart_buffer_inx = 0;

    }
}

void uart0_cb(UART_Handle h_uart, void *buf, size_t count)
{
    Semaphore_post(uart0_read_sem);
}

void uart0_msg(uint8_t* msg, int len)
{
    int i;

    for(i = 0 ; i < len ; i++)
        UART_write(uart0_handle, &msg[i], 1);
}

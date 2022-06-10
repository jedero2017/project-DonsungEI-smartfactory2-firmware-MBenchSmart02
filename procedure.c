/*
 * procedure.c
 *
 *  Created on: 2019. 11. 11.
 *      Author: sjhong
 */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/ndk/inc/netmain.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>

#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <stdio.h>
#include <string.h>

#include "ti_drivers_config.h"

#include "procedure.h"
#include "eeprom.h"
#include "spi.h"
#include "command.h"
#include "uart.h"
#include "modbus.h"
#include "service.h"
#include "vse153.h"
#include "adc_epi.h"

#define COMMON_SEND_BUF_MAX 4096
uint8_t common_send_buf[COMMON_SEND_BUF_MAX];
int common_send_buf_len;

//Semaphore_Handle sem_dsp_data = NULL;
//Semaphore_Handle sem_dsp = NULL;
//Semaphore_Handle sem_host_send = NULL;

uint8_t temp[64];
bool mbus_ret;

void main_proc_thread()
{
    int i;
    Task_Params params;

    GPIO_init();

//    spi_init();
    uart0_init();
    adc_epi_init();

    modbus_init();


    //sem_dsp = Semaphore_create(0, NULL, NULL);
    //sem_dsp_data = Semaphore_create(0, NULL, NULL);
    //sem_host_send = Semaphore_create(0, NULL, NULL);

    volatile uint16_t * padc = (uint16_t*)0x60000000;


    Task_Params_init(&params);

    params.instance->name = "uart thread";
    params.priority = 5;
    params.stackSize = 2048;

    Task_create((Task_FuncPtr)uart0_thread, &params, NULL);

    uint16_t adc_value[8];


    service_init();
//    vse153_init();
//
//    uint32_t ip = inet_addr("192.168.18.119");
//
//    vse153_connect(ip, 502);

    //Task_create((Task_FuncPtr) dsp_thread, &params, NULL);

//    Task_Params_init(&params);
//    params.instance->name = "thread dsp data";
//    params.priority = 5;
//    params.stackSize = 2048;
//
//    Task_create((Task_FuncPtr)dsp_data_thread, &params, NULL);

    //make_test_data();

    //Task_sleep(1000);
    //GPIO_enableInt(CONFIG_GPIO_DPMINTLn);

    while (1)
    {
//        mbus_ret = modbus_read_input_reg(modbus_tm_handle, 1, 1000, 1, temp);
//        if(mbus_ret == false)
 //           Task_sleep(10);

//        GPIO_write(CONFIG_GPIO_ADCONVLS, 1);
//        GPIO_write(CONFIG_GPIO_ADCONVLS, 0);
//
//        for(i = 0 ; i < 8 ; i++)
//        {
//            adc_value[i] = padc[4];
//        }
//
//        i = GPIO_read(CONFIG_GPIO_DI0);
//        i = GPIO_read(CONFIG_GPIO_DI0);
//        GPIO_write(CONFIG_GPIO_DO0, 1);
//        GPIO_write(CONFIG_GPIO_DO0, 0);

        Task_sleep(100);

    }
}

////mailbox cmd 종류에 따라서 semaphore를 다르게 사용함
//void dsp_data_thread()
//{
//    while(1)
//    {
//        Semaphore_pend(sem_dsp_data, BIOS_WAIT_FOREVER);
//    }
//}

void dsp_thread()
{
//    while (1)
//    {
//        Semaphore_pend(sem_dsp, BIOS_WAIT_FOREVER);
//
//        switch (dsp_cmd)
//        {
//        case 0x0001:
//            host_test();
//            break;
//
//        case 0x0100:
//            host_rgen_set();
//            break;
//
//        case 0x0101:
//            host_rgen_get();
//            break;
//
//        case 0x0102:
//            host_pwm_set();
//            break;
//
//        case 0x0103:
//            host_pwm_get();
//            break;
//
//        case 0x0104:
//            host_data_set();
//            break;
//
//        case 0x0105:
//            host_clim_set();
//            break;
//
//        case 0x0106:
//            host_clim_get();
//            break;
//
//        case 0x0107:
//            host_vlim_set();
//            break;
//
//        case 0x0108:
//            host_vlim_get();
//            break;
//
//        case 0x0109:
//            host_do_set();
//            break;
//
//        case 0x010A:
//            host_do_get();
//            break;
//
//        case 0x010B:
//            host_linefreq_get();
//            break;
//
//        case 0x0200:
//            host_rise_fault();
//            break;
//
//        case 0x010C:
//            host_boost_set();
//            break;
//
//        case 0x010D:
//            host_boost_get();
//            break;
//
//        case 0x010E:
//            host_speed_set();
//            break;
//
//        case 0x1001:
//            host_data_proc(0);
//            break;
//
//        case 0x1002:
//            host_data_proc(1);
//            break;
//        }
//    }
}

void process_command(uint8_t *buffer, int len)
{
//    uint16_t command;
//
//    command = buffer[8] + (uint16_t) buffer[9] * 256;
//
//    switch (command)
//    {
//    case 0x0001: //test request
//        dsp_test();
//        break;
//
//    case 0x0100: //rgen set
//        dsp_rgen_set(buffer);
//        break;
//
//    case 0x0101: //rgen get
//        dsp_rgen_get(buffer);
//        break;
//
//    case 0x0102: //pwm set
//        dsp_pwm_set(buffer);
//        break;
//
//    case 0x0103: //pwm get
//        dsp_pwm_get(buffer);
//        break;
//
//    case 0x0104: //data start/stop
//        dsp_data_set(buffer);
//        break;
//
//    case 0x0105: //clim set
//        dsp_clim_set(buffer);
//        break;
//
//    case 0x0106: //clim get
//        dsp_clim_get();
//        break;
//
//    case 0x0107: //vlim set
//        dsp_vlim_set(buffer);
//        break;
//
//    case 0x0108: //vlim get
//        dsp_vlim_get();
//        break;
//
//    case 0x0109: //do set
//        dsp_do_set(buffer);
//        break;
//
//    case 0x010A: //do get
//        dsp_do_get();
//        break;
//
//    case 0x010B: //line frequency get
//        dsp_linefreq_get();
//        break;
//
//    case 0x010C: //boost set
//        dsp_boost_set(buffer);
//        break;
//
//    case 0x010D: //boost get
//        dsp_boost_get(buffer);
//        break;
//
//    case 0x010E: //speed set
//           dsp_speed_set(buffer);
//           break;
//
//    }
}

void send_to_port(send_port_type port_type, void *handle)
{
    int ret;
    if (port_type == PORT_TYPE_TCP)
        ret = NDK_send(*(SOCKET*) handle, common_send_buf, common_send_buf_len,
                       0);
    else if (port_type == PORT_TYPE_UART)
        uart0_msg(common_send_buf, common_send_buf_len);

    ret = ret + 0;
}


/*
 * service.c
 *
 *  Created on: 2022. 1. 9.
 *      Author: sjhong
 */

#include <stdbool.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/BIOS.h>

#include <ti/ndk/inc/netmain.h>
#include <ti/ndk/inc/nettools/nettools.h>

#include <ti/ndk/slnetif/slnetifndk.h>
#include <ti/net/slnet.h>
#include <ti/net/slnetif.h>
#include <ti/net/slnetutils.h>


#include "ti_drivers_config.h"
#include "service.h"
#include "modbus.h"
#include "vse153.h"

//Semaphore_Handle sem_service_send = NULL;
Semaphore_Handle sem_data_send_timer = NULL;

struct sockaddr_in client;

Timer_Handle timer_data_send;

uint8_t service_send_buf[1024];
uint8_t service_send_len;

float temperature[5];
float speed[1];
float vibration[2];
float powermeter[3];

int pm_type;

SOCKET service_socket;


void service_init()
{
    Timer_Params params;
    Task_Params t_params;

    Semaphore_Params sem_params;

//    sem_service_send = Semaphore_create(0, NULL, NULL);

    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr("192.168.18.123");
    client.sin_port = NDK_htons(1000);

    Timer_init();

    Semaphore_Params_init(&sem_params);
    sem_params.mode = Semaphore_Mode_BINARY;

    sem_data_send_timer = Semaphore_create(0, &sem_params, NULL);

    Timer_Params_init(&params);
    params.period = 1000000; //1000ms
    params.periodUnits = Timer_PERIOD_US;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = service_data_send_timer_callback;

    timer_data_send = Timer_open(CONFIG_TIMER_0, &params);

    Task_Params_init(&t_params);
    t_params.instance->name = "data send thread";
    t_params.priority = 5;
    t_params.stackSize = 2048;

    Task_create((Task_FuncPtr)service_data_send_thread, &t_params, NULL);


}

bool service_check_packet(uint8_t * buffer, int len)
{

    uint16_t packet_len;

        if(len < 6)
            return false;

        if(buffer[0] == 'J' && buffer[1] == 'D')
        {
            packet_len = buffer[2] + (uint16_t)buffer[3] * 256;

            if(packet_len + 4 == len)
                return true;
        }

    return false;
}

void service_process(uint8_t * buffer, int len)
{
    uint16_t cmd;

    uint32_t send_ip;
    uint16_t send_port;
    uint16_t period;

    cmd = buffer[4] + (uint16_t) buffer[5] * 256;

    GPIO_write(CONFIG_GPIO_FRONTLED1, 1);


    switch (cmd)
    {
    case 0x0001: //init
        send_ip = (uint32_t)((uint32_t)buffer[6] | (uint32_t)(buffer[7] << 8)
                | (uint32_t)(buffer[8] << 16) | (uint32_t)(buffer[9] << 24));
        client.sin_addr.s_addr = send_ip;
        send_port = (uint16_t)buffer[10] + (uint16_t)buffer[11]*256;
        client.sin_port = NDK_htons(send_port);

        pm_type = buffer[12];

        //make response packet
        service_send_buf[0] = 'J';
        service_send_buf[1] = 'D';
        service_send_buf[2] = 2;
        service_send_buf[3] = 0;
        service_send_buf[4] = 1;
        service_send_buf[5] = 0;

        //send to client
        sendto(service_socket, service_send_buf, 6, 0, (struct sockaddr*)&client, sizeof(client));
        break;

    case 0x0002: //start

        //change period on data_send_timer
        period = (uint16_t)buffer[6] + (uint16_t)buffer[7]*256; //ms
        Timer_setPeriod(timer_data_send, Timer_PERIOD_US, (uint32_t)period * 1000); //us

        //make response packet
        service_send_buf[0] = 'J';
        service_send_buf[1] = 'D';
        service_send_buf[2] = 2;
        service_send_buf[3] = 0;
        service_send_buf[4] = 2;
        service_send_buf[5] = 0;

        //send to client

        sendto(service_socket, service_send_buf, 6, 0, (struct sockaddr*)&client, sizeof(client));
        Task_sleep(100);

        //timer start

        Timer_start(timer_data_send);
        break;

    case 0x0003: //stop

        //timer stop
        Timer_stop(timer_data_send);

        //make response packet
        service_send_buf[0] = 'J';
        service_send_buf[1] = 'D';
        service_send_buf[2] = 2;
        service_send_buf[3] = 0;
        service_send_buf[4] = 3;
        service_send_buf[5] = 0;

        //send to client

        sendto(service_socket, service_send_buf, 6, 0, (struct sockaddr*)&client, sizeof(client));
        Task_sleep(100);
        break;
   }

    GPIO_write(CONFIG_GPIO_FRONTLED1, 0);

}

void service_data_send_timer_callback(Timer_Handle h_timer)
{
    Semaphore_post(sem_data_send_timer);
}

void service_data_send_thread()
{
    bool ret = false;

    fdOpenSession(Task_self());

    while(1)
    {
        Semaphore_pend(sem_data_send_timer, BIOS_WAIT_FOREVER);
        GPIO_write(CONFIG_GPIO_FRONTLED1, 1);

        //get data

        //ret = service_get_temperature(temperature, 5);
        ret = service_get_temperature(temperature, 3);  //in smart factory device, temperature device count is 3

        if(ret == false)
            memset(temperature, 0, sizeof(temperature));

        ret = service_get_powermeter(powermeter, 3);

        if(ret == false)
            memset(powermeter, 0, sizeof(powermeter));

        ret = service_get_vibration(vibration, 2);

        if(ret == false)
            memset(vibration, 0, sizeof(vibration));


        ret = service_get_speed(speed, 1);

        if(ret == false)
            memset(speed, 0, sizeof(speed));

        //make data packet
        service_send_buf[0] = 'J';
        service_send_buf[1] = 'D';
        service_send_buf[2] = 46;
        service_send_buf[3] = 0;
        service_send_buf[4] = 0x00;
        service_send_buf[5] = 0x10;

        memcpy(&service_send_buf[6], &powermeter[0], 4);
        memcpy(&service_send_buf[10], &powermeter[1], 4);
        memcpy(&service_send_buf[14], &powermeter[2], 4);

        memcpy(&service_send_buf[18], &temperature[0], 4);
        memcpy(&service_send_buf[22], &temperature[1], 4);
        memcpy(&service_send_buf[26], &temperature[2], 4);
        memcpy(&service_send_buf[30], &temperature[3], 4);
        memcpy(&service_send_buf[34], &temperature[4], 4);

        memcpy(&service_send_buf[38], &speed[0], 4);

        memcpy(&service_send_buf[42], &vibration[0], 4);
        memcpy(&service_send_buf[46], &vibration[1], 4);

        //send packet to client
        int retc = sendto(service_socket, service_send_buf, 50, 0, (struct sockaddr*)&client, sizeof(client));

        if(retc < 0)
        {
            retc = fdError();
        }

        GPIO_write(CONFIG_GPIO_FRONTLED1, 0);
   }
}

bool service_get_temperature(float* arr_tem, int len)
{
    uint8_t data[2];
    bool ret;
    int i ;
    uint16_t temperature;

    for(i = 0; i < len ; i++)
    {
        ret = modbus_read_input_reg(modbus_tm_handle, i + 1, 0x03E8, 1, data);
        if(ret == false)
            return false;

        //소숫점 1자리 fix

        temperature = data[0] * 256 + data[1];

        arr_tem[i] = (float)temperature/10;
    }


    return true;
}

bool service_get_powermeter(float* arr_pow, int len)
{
    bool ret;
    int pm_addr;

    uint8_t buffer[20];

    float voltage[3];
    float current[3];

    if(pm_type == 0) //low voltage
        pm_addr = 1;
    else
        pm_addr = 2;

    //voltage
//    ret = modbus_read_input_reg(modbus_pm_handle, pm_addr, 0x200, 6, buffer);
//
//    if(ret == false)
//        return false;
//
//    voltage[0] = read_big_endian_float(&buffer[0]);
//    voltage[1] = read_big_endian_float(&buffer[4]);
//    voltage[2] = read_big_endian_float(&buffer[8]);


    ret = modbus_read_input_reg(modbus_pm_handle, pm_addr, 0x800, 2, buffer);

    if(ret == false)
        return false;

    voltage[0] = read_big_endian_float(&buffer[0]);
    voltage[1] = read_big_endian_float(&buffer[0]);
    voltage[2] = read_big_endian_float(&buffer[0]);


    arr_pow[0] = (voltage[0] + voltage[1] + voltage[2])/3;

    //current
    ret = modbus_read_input_reg(modbus_pm_handle, pm_addr, 0x20E, 6, buffer);
    if(ret == false)
        return false;

    current[0] = read_big_endian_float(&buffer[0]);
    current[1] = read_big_endian_float(&buffer[4]);
    current[2] = read_big_endian_float(&buffer[8]);

    arr_pow[1] = (current[0] + current[1] + current[2])/1.732;

    //power
    ret = modbus_read_input_reg(modbus_pm_handle, pm_addr, 0x424, 2, buffer);
    if(ret == false)
        return false;

    arr_pow[2] = read_big_endian_float(&buffer[0]);;

    return true;
}

bool service_get_vibration(float* arr_vib, int len)
{
    bool ret = vse153_get_data(arr_vib,  len);
    return ret;
}

bool service_get_speed(float* arr_speed, int len)
{
    return true;
}

float read_big_endian_float(uint8_t* bytes)
{
    float result;

    *((uint8_t*)&result) = bytes[3];
    *((uint8_t*)&result + 1) = bytes[2];
    *((uint8_t*)&result + 2) = bytes[1];
    *((uint8_t*)&result + 3) = bytes[0];

    return result;
}

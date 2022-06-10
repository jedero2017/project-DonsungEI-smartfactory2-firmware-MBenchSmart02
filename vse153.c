/*
 * vse153.c
 *
 *  Created on: 2022. 1. 9.
 *      Author: sjhong
 */


//vse153 with modbustcp

#include <stdbool.h>

#include <ti/ndk/inc/netmain.h>
#include <ti/ndk/inc/nettools/nettools.h>

#include <ti/ndk/slnetif/slnetifndk.h>
#include <ti/net/slnet.h>
#include <ti/net/slnetif.h>
#include <ti/net/slnetutils.h>


#include <arpa/inet.h>

#include "service.h"


bool vse153_connected = false;

SOCKET vse153_sock;

uint8_t vse153_recv_buf[64];


void vse153_init()
{
    //fdOpenSession(TaskSelf());


}

bool vse153_connect(uint32_t ip, int port)
{
    int retc;
    struct sockaddr_in sock_addr;

    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = ip;
    sock_addr.sin_port = htons(port);

    //fdOpenSession(TaskSelf());

    vse153_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (vse153_sock < 0)
    {
        close(vse153_sock);
    }

    struct timeval sttTime;
    sttTime.tv_sec = 0;
    sttTime.tv_usec = 10000;

    retc = setsockopt( vse153_sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&sttTime, sizeof(sttTime) );
    retc = setsockopt( vse153_sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&sttTime, sizeof(sttTime) );

    retc = connect(vse153_sock, (struct sockaddr*) &sock_addr, sizeof(sock_addr));

    if (retc < 0)
    {
        close(vse153_sock);
        vse153_connected = false;
    }
    else
        vse153_connected = true;


    return vse153_connected;

}

bool vse153_get_data(float* data, int len)
{
    int ret;
    float velocity, acceleration;

    if(vse153_connected == false)
        vse153_connect(inet_addr("192.168.18.101"), 502);

    if(vse153_connected == false)
        return false;

    uint8_t send_buff[] = {0,0,0,0,0,6,1,4,0,0,0,4};

    ret = send(vse153_sock, send_buff, 12, 0);
    if(ret < 0)
    {
        close(vse153_sock);
        vse153_connected = false;
        return false;
    }

    ret = recv(vse153_sock, vse153_recv_buf, 256, 0);
    if(ret < 0)
    {
        close(vse153_sock);
        vse153_connected = false;
        return false;
    }

    velocity = read_big_endian_float(&vse153_recv_buf[9]);
    acceleration = read_big_endian_float(&vse153_recv_buf[13]);

    data[0] = velocity;
    data[1] = acceleration;

    return true;
}


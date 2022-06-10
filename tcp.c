
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include <pthread.h>

/* BSD support */
/*
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <netdb.h>
*/

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/Types.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/ndk/inc/netmain.h>
#include <ti/ndk/inc/nettools/nettools.h>

#include <ti/ndk/slnetif/slnetifndk.h>
#include <ti/net/slnet.h>
#include <ti/net/slnetif.h>
#include <ti/net/slnetutils.h>

//#include <ti/display/Display.h>
#include <ti/drivers/emac/EMACMSP432E4.h>
#include <ti/drivers/GPIO.h>

#include <ti/devices/msp432e4/driverlib/flash.h>

#include "ti_drivers_config.h"


#include "eeprom.h"
#include "procedure.h"
#include "command.h"
#include "service.h"
#include "vse153.h"

#define TCPPORT 1000

#define TCPHANDLERSTACK 2048
#define IFPRI  4   /* Ethernet interface priority */

/* Prototypes */
extern void *tcpHandler(void *arg0);


#define TCPPACKETSIZE 1024
#define NUMTCPWORKERS 3
#define MAXPORTLEN    6

//extern Display_Handle display;

//extern void fdOpenSession();
//extern void fdCloseSession();
//extern void *TaskSelf();

/*
 *  ======== tcpWorker ========
 *  Task to handle TCP connection. Can be multiple Tasks running
 *  this function.
 */

SOCKET prev_fd = 0;

uint8_t tcp_rcv_buf[TCPPACKETSIZE];

void *tcp_rcv_thread(void *arg0)
{
    SOCKET  clientfd = *(SOCKET *)arg0;
    int  bytesRcvd;

    fdOpenSession(TaskSelf());

    if(prev_fd != 0)
    {
        NDK_shutdown(prev_fd, SHUT_WR);

        fdClose(prev_fd);

//        if(sem_host_send != NULL)
//            Semaphore_post(sem_host_send);
    }

    prev_fd = clientfd;

    //host_socket = clientfd;


    float data[2];

    service_socket = clientfd;


    while ((bytesRcvd = NDK_recv(clientfd, tcp_rcv_buf, TCPPACKETSIZE, 0)) > 0)
    {
//        command_parse(tcp_buffer, bytesRcvd, PORT_TYPE_TCP, (void*)&clientfd);
        GPIO_write(CONFIG_GPIO_FRONTLED1, 1);

        if(service_check_packet(tcp_rcv_buf, bytesRcvd) == true)
            service_process(tcp_rcv_buf, bytesRcvd);
        GPIO_write(CONFIG_GPIO_FRONTLED1, 0);
    }


    fdClose(clientfd);

    //host_socket = NULL;

    fdCloseSession(TaskSelf());

    return (NULL);
}

void *tcp_send_thread(void *arg0)
{
    SOCKET  clientfd = *(SOCKET *)arg0;
    int fd_result;

    fdOpenSession(TaskSelf());

    fdShare(clientfd);

    while (1)
    {
//        Semaphore_pend(sem_host_send, BIOS_WAIT_FOREVER);
//
//        if(fdStatus(clientfd, FDSTATUS_SEND, &fd_result) == -1)
//            break;
//
//        if(host_send(clientfd) != 1)
//            break;
    }

    fdClose(clientfd);

    fdCloseSession(TaskSelf());

    return (NULL);
}

/*
 *  ======== tcpHandler ========
 *  Creates new Task to handle new TCP connections.
 */
void *tcpHandler(void *arg0)
{
    pthread_t          thread;
    pthread_attr_t     attrs;
    struct sched_param priParam;
    int                retc;
    int                detachState;
    int                status;
    SOCKET             server_sock;
    struct sockaddr_in clientAddr;
    int                optval;
    static SOCKET      client_sock;
    int                optlen = sizeof(optval);
    int                addrlen;
    int send_buf_len;

    struct sockaddr_in* sock_addr;
    //int errno;

    sock_addr = (struct sockaddr_in*)arg0;


//    sock_addr->sin_family = AF_INET;
//    sock_addr->sin_addr.s_addr = inet_addr("192.168.18.119");
//    sock_addr->sin_port = NDK_htons(502);

    fdOpenSession(TaskSelf());

    //vse153_connect(inet_addr("192.168.18.119"), 502);
    server_sock = NDK_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    //server_sock = NDK_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //int ret = NDK_connect(server_sock, (struct sockaddr*)&sock_addr, sizeof(struct sockaddr));

    //ret = fdError();

    send_buf_len = 4096;
    status = NDK_setsockopt(server_sock, SOL_SOCKET, SO_SNDBUF, &send_buf_len, sizeof(send_buf_len));


    if (server_sock == INVALID_SOCKET) {
        goto shutdown;
    }

    status = NDK_bind(server_sock, (struct sockaddr*)sock_addr, sizeof(struct sockaddr));
    if (status == -1) {
        goto shutdown;
    }

/*
    status = NDK_listen(server_sock, NUMTCPWORKERS);
    if (status == -1) {
        goto shutdown;
    }

    optval = 1;
    status = NDK_setsockopt(server_sock, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);
    if (status == -1) {
        goto shutdown;
    }

    //shutdown 시에 즉시 종료되도로 linger option을 수정
    struct linger lg;

    lg.l_onoff = 1;
    lg.l_linger = 0;

    status = NDK_setsockopt(server_sock, SOL_SOCKET, SO_LINGER, &lg, sizeof(lg));
    if (status == -1) {
        goto shutdown;
    }
*/
/*
    while(1)
    {

        client_sock = NDK_accept(server_sock, (struct sockaddr *)&clientAddr, &addrlen);

        if(client_sock == INVALID_SOCKET)
        {
//            errno = fdError();
            fdError();

//            if(errno == NDK_ENFILE)
                continue;
//            else
//                break;
        }

        pthread_attr_init(&attrs);
        priParam.sched_priority = 3;

        detachState = PTHREAD_CREATE_DETACHED;
        retc = pthread_attr_setdetachstate(&attrs, detachState);
        if (retc != 0) {
            break;
        }

        pthread_attr_setschedparam(&attrs, &priParam);

        retc |= pthread_attr_setstacksize(&attrs, 2048);
        if (retc != 0) {
            break;
        }

        retc = pthread_create(&thread, &attrs, tcp_rcv_thread, (void *)&client_sock);
        if (retc != 0) {
            break;
        }

        retc = pthread_create(&thread, &attrs, tcp_send_thread, (void *)&client_sock);
        if (retc != 0) {
            break;
        }


        addrlen = sizeof(clientAddr);
    }

*/

    pthread_attr_init(&attrs);
    priParam.sched_priority = 3;

    detachState = PTHREAD_CREATE_DETACHED;
    retc = pthread_attr_setdetachstate(&attrs, detachState);
    if (retc != 0) {
     goto shutdown;
    }

    pthread_attr_setschedparam(&attrs, &priParam);

    retc |= pthread_attr_setstacksize(&attrs, 2048);
    if (retc != 0) {
        goto shutdown;
    }

    retc = pthread_create(&thread, &attrs, tcp_rcv_thread, (void *)&server_sock);
    if (retc != 0) {
        goto shutdown;
    }

//    retc = pthread_create(&thread, &attrs, tcp_send_thread, (void *)&server_sock);
//    if (retc != 0) {
//        goto shutdown;
//    }



    //vse153_init();
    //vse153_connect(inet_addr("192.168.18.119"), 502);

    //uint32_t ip = inet_addr("192.168.18.119");
    //vse153_connect(ip, 502);


    while(1)
    {
        Task_sleep(100);
    }

shutdown:
    if (server_sock != INVALID_SOCKET) {
        fdClose(server_sock);
    }

    fdCloseSession(TaskSelf());

    return (NULL);
}

/*
 *  ======== netIPAddrHook ========
 *  user defined network IP address hook
 */
void netIPAddrHook(uint32_t IPAddr, unsigned int IfIdx, unsigned int fAdd)
{
    pthread_t           thread;
    pthread_attr_t      attrs;
    struct sched_param  priParam;
    int                 retc;
    int                 detachState;
    static struct sockaddr_in     argsock;

    argsock.sin_family = AF_INET;
    argsock.sin_addr.s_addr = inet_addr("0.0.0.0");
    argsock.sin_port = NDK_htons(TCPPORT);


    if (fAdd)
    {
        pthread_attr_init(&attrs);
        priParam.sched_priority = 1;

        detachState = PTHREAD_CREATE_DETACHED;
        retc = pthread_attr_setdetachstate(&attrs, detachState);
        if (retc != 0) {
            return;
        }

        pthread_attr_setschedparam(&attrs, &priParam);

        retc |= pthread_attr_setstacksize(&attrs, TCPHANDLERSTACK);
        if (retc != 0) {
            return;
        }

        retc = pthread_create(&thread, &attrs, tcpHandler, (void *)&argsock);
        if (retc != 0) {
            return;
        }

    }

}

/*
 *  ======== netOpenHook ========
 *  NDK network open hook
 */
void netOpenHook()
{
//    Display_printf(display, 0, 0, "netOpenHook\n");
}

void change_ip_addr(uint8_t ipaddr[4], uint8_t subnet[4], uint8_t gateway[4])
{

}

char str_ip_addr[20];
char str_subnet_mask[20];
char str_gateway[20];


//eeprom에서 ip를 읽어 str_ 변수에 넣음
void read_ip_config()
{
    uint32_t ipaddr, subnet, gateway;

    eeprom_read(&ipaddr, 0, 4);
    eeprom_read(&subnet, 4, 4);
    eeprom_read(&gateway,8, 4);

    sprintf(str_ip_addr, "%d.%d.%d.%d", ipaddr >> 24, (ipaddr >> 16) & 0xff,  (ipaddr >> 8) & 0xff, ipaddr & 0xff);
    sprintf(str_subnet_mask, "%d.%d.%d.%d", subnet >> 24, (subnet >> 16) & 0xff,  (subnet >> 8) & 0xff, subnet & 0xff);
    sprintf(str_gateway, "%d.%d.%d.%d", gateway >> 24, (gateway >> 16) & 0xff,  (gateway >> 8) & 0xff, gateway & 0xff);

}

void read_mac()
{
    uint32_t user0, user1;
    uint8_t mac[8];

    eeprom_read((uint32_t*)mac, 0x10, 8);

    user0 = ((uint32_t)mac[2] << 16) | ((uint32_t)mac[1] << 8) | (uint32_t)mac[0];
    user1 = ((uint32_t)mac[5] << 16) | ((uint32_t)mac[4] << 8) | (uint32_t)mac[3];

    //flash 의 user0, user1 register에 mac 을 기록하면 rtos에서 그걸 불러다 쓴다. 주의:commit(FlashUserSave)을 하면 mac을 바꿀 수 없음.
    FlashUserSet(user0, user1);

}

void netInitHook(void *hCfg)
{
    extern char *LocalIPAddr;
    extern char *LocalIPMask;
    extern char *GatewayIP;

    read_ip_config();

    LocalIPAddr = str_ip_addr;
    LocalIPMask = str_subnet_mask;
    GatewayIP = str_gateway;

    read_mac();
}

void netRebootHook(void * hCfg, int rc)
{
    CI_IPNET NA;
    CI_ROUTE RT;

    extern char *DomainName;

    read_ip_config();

    /* configure IP address manually on interface 1 */
    /* setup manual IP address */
    memset(&NA, 0, sizeof(NA));
    NA.IPAddr = inet_addr(str_ip_addr);
    NA.IPMask = inet_addr(str_subnet_mask);
    strcpy(NA.Domain, DomainName);
    NA.NetType = 0;

    uint32_t hCfgEntry;
    CfgGetEntry(hCfg, CFGTAG_IPNET, 1, 1, (void*)&hCfgEntry);

    CfgRemoveEntry(hCfg, (void*)hCfgEntry);


    CfgAddEntry(hCfg, CFGTAG_IPNET, 1, 0,
            sizeof(CI_IPNET), (unsigned char *)&NA, 0);


    memset(&RT, 0, sizeof(RT));
    RT.IPDestAddr = 0;
    RT.IPDestMask = 0;
    RT.IPGateAddr = inet_addr(str_gateway);

    CfgGetEntry(hCfg, CFGTAG_ROUTE, 0, 1, (void*)&hCfgEntry);

    CfgRemoveEntry(hCfg, (void*)hCfgEntry);

    CfgAddEntry(hCfg, CFGTAG_ROUTE, 0, 0,
            sizeof(CI_ROUTE), (unsigned char *)&RT, 0);

}

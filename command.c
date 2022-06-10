/*
 * command.c
 *
 *  Created on: 2019. 12. 16.
 *      Author: sjhong
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <xdc/runtime/Types.h>
#include <ti/ndk/inc/netmain.h>
#include <ti/drivers/UART.h>

#include <ti/sysbios/knl/Semaphore.h>

#include <ti/devices/msp432e4/driverlib/driverlib.h>

#include "procedure.h"
#include "command.h"
#include "eeprom.h"
#include "uart.h"

void command_parse(char * cmd, int len, send_port_type port_type, void* handle)
{
    int cmd_len;
    int s, e;
    enum commands c;
    char str[100];
    uint32_t ip;
    uint8_t mac[8];
    int ret;

    extern int32_t shutter_adv;

    make_lowercase(cmd, len);
    cmd_len = remove_duplication(cmd, len);

    get_command(cmd, cmd_len, 0, &s, &e);

    c = distinct_cmd(cmd, s, e);

    switch(c)
    {
    case CMD_BOOTLOADER:

        MAP_SysTickIntDisable();
        MAP_SysTickDisable();

        NVIC->ICER[0] = 0xffffffff;
        NVIC->ICER[1] = 0xffffffff;
        NVIC->ICER[2] = 0xffffffff;
        NVIC->ICER[3] = 0xffffffff;

        (*((void (*)(void))(*(uint32_t *)0x2c)))();
        break;

    case CMD_SET_MAC:

        ret = parse_mac(cmd, e, cmd_len-1, (uint8_t*)&mac);
        if(ret == 0){
            eeprom_write((uint32_t*)&mac, 0x10, 8);
            sprintf((char*)common_send_buf, "OK, mac address will be changed after reboot\r\n");
            common_send_buf_len = strlen((char*)common_send_buf);
        }
        else
        {
            sprintf((char*)common_send_buf, "invalid mac\r\n");
            common_send_buf_len = strlen((char*)common_send_buf);
        }

        send_to_port(port_type, handle);
        break;

    case CMD_SET_TCP_IP:
        ip = parse_ip(cmd, e, cmd_len-1);
        if(ip != 0){
            eeprom_write(&ip, 0, 4);
            sprintf((char*)common_send_buf, "OK\r\n");
            common_send_buf_len = strlen((char*)common_send_buf);
        }
        else
        {
            sprintf((char*)common_send_buf, "invalid ip\r\n");
            common_send_buf_len = strlen((char*)common_send_buf);
        }

        send_to_port(port_type, handle);

        break;

    case CMD_SET_TCP_SM:
        ip = parse_ip(cmd, e, cmd_len-1);
        if(ip != 0){
            eeprom_write(&ip, 4, 4);
            sprintf((char*)common_send_buf, "OK\r\n");
            common_send_buf_len = strlen((char*)common_send_buf);
        }
        else
        {
            sprintf((char*)common_send_buf, "invalid subnet mask\r\n");
            common_send_buf_len = strlen((char*)common_send_buf);
        }

        send_to_port(port_type, handle);

        break;


    case CMD_SET_TCP_GW:
        ip = parse_ip(cmd, e, cmd_len-1);
        if(ip != 0){
            eeprom_write(&ip, 8, 4);
            sprintf((char*)common_send_buf, "OK\r\n");
            common_send_buf_len = strlen((char*)common_send_buf);
        }
        else
        {
            sprintf((char*)common_send_buf, "invalid gateway\r\n");
            common_send_buf_len = strlen((char*)common_send_buf);
        }

        send_to_port(port_type, handle);

        break;

    case CMD_IP_RENEW:

        NC_NetStop(1);
        sprintf((char*)common_send_buf, "OK\r\nTCP Connection will be closed\r\n");
        common_send_buf_len = strlen((char*)common_send_buf);

        send_to_port(port_type, handle);

       break;

    case CMD_GET_IPSET:
        sprintf((char*)common_send_buf, "OK\r\n");
        common_send_buf_len = strlen((char*)common_send_buf);

        eeprom_read(&ip, 0, 4);
        sprintf(str, "ip : %d.%d.%d.%d\r\n", ip >> 24, (ip >> 16) & 0xff,  (ip >> 8) & 0xff, ip & 0xff);
        memcpy(common_send_buf + common_send_buf_len, str, strlen(str));
        common_send_buf_len += strlen(str);

        eeprom_read(&ip, 4, 4);
        sprintf(str, "subnet : %d.%d.%d.%d\r\n", ip >> 24, (ip >> 16) & 0xff,  (ip >> 8) & 0xff, ip & 0xff);
        memcpy(common_send_buf + common_send_buf_len, str, strlen(str));
        common_send_buf_len += strlen(str);

        eeprom_read(&ip, 8, 4);
        sprintf(str, "gateway : %d.%d.%d.%d\r\n", ip >> 24, (ip >> 16) & 0xff,  (ip >> 8) & 0xff, ip & 0xff);
        memcpy(common_send_buf + common_send_buf_len, str, strlen(str));
        common_send_buf_len += strlen(str);

        eeprom_read((uint32_t*)mac, 0x10, 8);
        sprintf(str, "mac : %02X:%02X:%02X:%02X:%02X:%02X\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        memcpy(common_send_buf + common_send_buf_len, str, strlen(str));
        common_send_buf_len += strlen(str);

        send_to_port(port_type, handle);

        break;

    case CMD_NONE:
        sprintf((char*)common_send_buf, ">");
        common_send_buf_len = strlen((char*)common_send_buf);

        send_to_port(port_type, handle);

        break;

    case CMD_UNKNOWN:
        sprintf((char*)common_send_buf, "ERROR, -1, invalid command\r\n");
        common_send_buf_len = strlen((char*)common_send_buf);

        send_to_port(port_type, handle);

        break;
    }

}

int parse_data(char* str, int len, int start, char* buff)
{

    int hex_val;

    int i, buff_len;
    char t;

    int flag = 0;

    buff_len = 0;
    flag = 0;
    for(i = start + 1 ; i < len ; i++)
    {
        t = str[i];

        if(t == ' ')
        {
            buff[buff_len] = hex_val;
            hex_val = 0;
            buff_len++;
            flag = 0;
        }
        else if (t >= '0' && t <= '9')
        {
            hex_val = hex_val * 16 + t - '0';
            flag = 1;
        }
        else if (t >= 'a' && t <='f')
        {
            hex_val = hex_val * 16 + t - 'a' + 10;
            flag = 1;
        }
    }

    if(flag == 1)
    {
        buff[buff_len] = hex_val;
        buff_len++;
    }

    return buff_len;
}

int parse_mac(char* str, int start, int end, uint8_t* mac)
{
    int index = 0;
    int i;
    int temp;

    mac[0] = mac[1] = mac[2] = mac[3] = mac[4] = mac[5] = mac[6] = mac[7] = 0;

    for (i = start; i <= end; i++) {

        if (str[i] >= '0' && str[i] <= '9')
            temp = str[i] - 0x30;
        else if (str[i] >= 'A' && str[i] <= 'F')
            temp = str[i] - 'A' + 10;
        else if (str[i] >= 'a' && str[i] <= 'f')
            temp = str[i] - 'a' + 10;
        else if (str[i] == ':' )
        {
            index++;
            continue;
        }
        else
            continue;

        mac[index] *= 16;
        mac[index] += temp;

        if(index >= 6)
            break;
    }

    if(index == 5)
        return 0;
    else
        return -1;
}

uint32_t parse_ip(char* str, int start, int end)
{
    int index = 0;
    int i;
    uint8_t ip[4];
    uint32_t result;

    ip[0] = ip[1] = ip[2] = ip[3] = 0;

    for (i = start; i <= end; i++)
    {
        if (str[i] >= '0' && str[i] <= '9')
        {
            ip[index] *= 10;
            ip[index] += str[i] - '0';
        }
        else if (str[i] == '.')
        {
            index++;
        }

     if(index >= 4)
         break;
    }

    if(index == 3)
    {
        result = ((uint32_t)ip[0] << 24) |
                 ((uint32_t)ip[1] << 16) |
                 ((uint32_t)ip[2] << 8) |
                  (uint32_t)ip[3];
    }
    else
        result = 0;

    return result;
}

double parse_double(char* str, int start, int end)
{
    double result = 0;
    int sign = 1;
    int flag_underpoint = 0;
    double a;
    int i, j;

    for(i = start ; i < end ; i++)
    {
        if(str[i] == '+')
            sign = 1;
        else if(str[i] == '-')
            sign = -1;
        else if(str[i] == '.')
            flag_underpoint = 1;
        else if(str[i] >= '0' && str[i] <= '9')
        {
            a = (double)(str[i] - '0');

            if(flag_underpoint == 0)
            {
                result = result * 10 + a;
            }
            else
            {
                for(j = 0 ; j < flag_underpoint ; j++)
                    a = a / 10;

                result = result + a;
                flag_underpoint++;
            }
        }
        else
            return 1e100; //아주 큰 수 = 오류
    }

    return sign * result;
}


int parse_int(char* str, int start, int end)
{
    int result = 0;
    int sign = 1;
    int a;
    int i;

    for(i = start ; i < end ; i++)
    {
        if(str[i] == '+')
            sign = 1;
        else if(str[i] == '-')
            sign = -1;
        else if(str[i] >= '0' && str[i] <= '9')
        {
            a = (int)(str[i] - '0');

            result = result * 10 + a;
        }
        else
            return 0;
    }

    return sign * result;
}

int compare(char* buffer1, char* buffer2, int len)
{
    int i;

    for(i = 0 ; i < len ; i++)
    {
        if(buffer1[i] != buffer2[i])
            return 0;
    }
    return 1;
}

enum commands distinct_cmd(char* cmd, int start, int end)
{
    int len;

    len = end - start;

    if (len == 5 && compare(&cmd[start], "setip", 5))
        return CMD_SET_TCP_IP;

    if (len == 5 && compare(&cmd[start], "setgw", 5))
        return CMD_SET_TCP_GW;

    if (len == 5 && compare(&cmd[start], "setsm", 5))
        return CMD_SET_TCP_SM;

    if (len == 6 && compare(&cmd[start], "setmac", 6))
        return CMD_SET_MAC;

    if (len == 5 && compare(&cmd[start], "getip", 5))
        return CMD_GET_IPSET;

    if (len == 7 && compare(&cmd[start], "iprenew", 7))
        return CMD_IP_RENEW;

    if (len == 10 && compare(&cmd[start], "bootloader", 10))
        return CMD_BOOTLOADER;

    if (len == 0)
        return CMD_NONE;

    return CMD_UNKNOWN;
}

void make_lowercase(char* str, int len)
{
    int i;

    for(i = 0 ; i < len ; i++)
    {
        if(str[i] >= 65 && str[i] <= 90)
            str[i] += 32;
    }
}

int is_whitechar(char c)
{
    if(c == ' ' || c == '\t')
        return 1;
    else
        return 0;
}

int is_validchar(char c)
{
    if(c >= 'A' && c <= 'Z')
        return 1;

    if(c >= 'a' && c <= 'z')
            return 1;

    if(c >= '0' && c <= '9')
        return 1;

    if(c == ' ' || c == '-' || c == ',' || c == '+' || c == '-' || c == '.' || c == ':')
        return 1;

    return 0;
}

int remove_duplication(char* cmd_args, int len)
{

    //white char의 중복을 제거함
    //알파벳, 숫자 , 스페이스, '-' , ',' 말고 다 제거

    int i;
    int count = 0;
    char * buffer;
    int find = 0;

    buffer = malloc(len);

    for(i = 0 ; i < len ; i++)
    {
        if(!is_validchar(cmd_args[i]))
            continue;

       if(is_whitechar(cmd_args[i]))
       {
           if(find == 0)
           {
               find = 1;
               buffer[count++] = cmd_args[i];
           }

           continue;
       }
       else
       {
           find = 0;
           buffer[count++] = cmd_args[i];
       }
    }

    for(i = 0 ; i < count ; i++)
        cmd_args[i] = buffer[i];

    free(buffer);

    return count;
}

void get_command(char* cmd_args, int len, int start, int* cmd_start, int* cmd_end)
{
    int i;

    *cmd_start = 0;
    *cmd_end = len;

    for(i = start ; i < len ; i++)
    {
        if(cmd_args[i] >= 'a' && cmd_args[i] <= 'z')
        {
            *cmd_start = i;
            break;
        }
    }


    for(i = *cmd_start + 1 ; i < len ; i++)
    {
        if(cmd_args[i] == ' ' || cmd_args[i] == '\t')
        {
            *cmd_end = i;
            break;
        }
    }
}




void get_range(char * cmd_args, int len, int start, int* range_start, int* range_end)
{
    int i;
    *range_start = len;
    *range_end = len;

    for(i = start ; i < len ; i++)
    {
        if(cmd_args[i] == 'a' && cmd_args[i+1] == 'l' && cmd_args[i+2] == 'l')
        {
            *range_start = i;
            *range_end = i+3;
            return;
        }

        if(cmd_args[i] >= '0' && cmd_args[i] <= '9')
        {
            *range_start = i;
            break;
        }
    }


    for(i = *range_start + 1 ; i < len ; i++)
    {
        if((cmd_args[i] >= '0' && cmd_args[i] <= '9') || cmd_args[i] == ',')
            continue;
        if(cmd_args[i] == ' ' || cmd_args[i] == '-')
        {
            if(cmd_args[i+1] >= '0' && cmd_args[i+1] <= '9')
                continue;
            else
            {
                *range_end = i;
                break;
            }
        }
        else
        {
            *range_end = i;
            break;
        }
    }
}

void get_option(char* cmd_args, int len, int start, int* option_start, int* option_end)
{
    int i;
    *option_start = 0;
    *option_end = len;

    for(i = start ; i < len ; i++)
    {
        if(cmd_args[i] == '-')
        {
            *option_start = i;
            break;
        }

    }


    for(i = *option_start + 1 ; i < len ; i++)
    {
        if(cmd_args[i] == ' ' || cmd_args[i] == '\t')
        {
            *option_end = i;
            break;
        }
    }
}

void get_value(char* cmd_args, int len, int start, int* value_start, int* value_end)
{
    int i;
    *value_start = 0;
    *value_end = len;

    for(i = start ; i < len ; i++)
    {
        if((cmd_args[i] >= '0' && cmd_args[i] <= '9') || cmd_args[i] == '+' || cmd_args[i] == '-')
        {
            *value_start = i;
            break;
        }
    }


    for(i = *value_start + 1 ; i < len ; i++)
    {
        if((cmd_args[i] >= '0' && cmd_args[i] <= '9') || cmd_args[i] =='.')
            continue;
        else
        {
            *value_end = i;
            break;
        }
    }
}


uint16_t to_uint16(char* buf, int len)
{
    int i;
    uint16_t result;

    result = 0;

    for(i = 0 ; i < len ; i++)
    {
        if(buf[i] == ' ' || buf[i] == '\t')
            continue;

        if(buf[i] >= '0' && buf[i] <= '9')
        {
            result = result * 10 + (buf[i] - '0');
        }
        else
            return 0;
    }
    return result;
}

void get_range_channels(char* cmd_args, int range_start, int range_end, range_channels* p_rc)
{
    int i,j;

    char buf1[10];
    char buf2[10];
    int inx1, inx2;
    int buf_flag;
    int r1, r2;

    p_rc->count = 0;

    inx1 = 0;
    inx2 = 0;
    buf_flag = 0;

    for(i = range_start ; i <= range_end ; i++)
    {
       if(cmd_args[i] == '-')
       {
           buf_flag = 1;
       }
       else if(cmd_args[i] == ',' || i == range_end)
       {
           if(buf_flag == 0) //단일 값
           {
               if(p_rc->count < 100)
                   p_rc->ch[p_rc->count] = to_uint16(buf1, inx1);

               p_rc->count++;
               inx1 = 0;
               inx2 = 0;
               buf_flag = 0;
           }
           else // range
           {
               r1 = to_uint16(buf1, inx1);
               r2 = to_uint16(buf2, inx2);

               for(j = r1 ; j <= r2 ; j++)
               {
                   if(p_rc->count < 100)
                       p_rc->ch[p_rc->count] = j;

                   p_rc->count++;
               }

               inx1 = 0;
               inx2 = 0;
               buf_flag = 0;
           }

       }
       else
       {
           if(buf_flag == 0)
           {
               if(inx1 < 10)
                   buf1[inx1++] = cmd_args[i];
           }
           else
           {
               if(inx2 < 10)
                   buf2[inx2++] = cmd_args[i];
           }
       }

    }

}

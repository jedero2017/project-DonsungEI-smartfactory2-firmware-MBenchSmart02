/*
 * command.h
 *
 *  Created on: 2019. 12. 16.
 *      Author: sjhong
 */

#ifndef COMMAND_H_
#define COMMAND_H_


typedef struct {
    int count;
    uint16_t ch[100];
}range_channels;

enum commands {
    CMD_INIT,
    CMD_SELFCHECK,

    CMD_SET_TCP_IP,
    CMD_SET_TCP_GW,
    CMD_SET_TCP_SM,
    CMD_SET_MAC,
    CMD_GET_IPSET,
    CMD_IP_RENEW,

    CMD_SYNC,
    CMD_FUZE,
    CMD_MSLALL,
    CMD_TLM,

    CMD_CLK,
    CMD_BYPASS,

    CMD_NONE,
    CMD_UNKNOWN,

    CMD_BOOTLOADER,

    CMD_SDLC,
    CMD_SHUTTER,
    CMD_GETADC,
    CMD_GETCHV,

    CMD_SETSADV,
    CMD_SDLCISO
};


extern int8_t vertical[5];

extern uint8_t cmd_send_data[];
extern uint16_t cmd_send_data_len;

uint32_t parse_ip(char* str, int start, int end);
int parse_mac(char* str, int start, int end, uint8_t* mac);

void command_parse(char * cmd, int len, send_port_type port_type, void* handle);

int unregister_vertical(int res_no);
int register_vertical(int res_no);

int is_valid_scb_channel(range_channels * p_rc);
int is_valid_gcb_channel(range_channels * p_rc);
int is_valid_pcb_channel(range_channels * p_rc);
int is_valid_do_channel(range_channels * p_rc);
int is_valid_ao_channel(range_channels * p_rc);

void command_scb(range_channels* p_rc1, range_channels* p_rc2, int res_no, int on);
void command_gcb(range_channels* p_rc, int on);
void command_pcb(range_channels* p_rc, int on);
void command_do(range_channels* p_rc, int on);
void command_ao(range_channels* p_rc, double voltage);

int parse_data(char* str, int len, int start, char* buff);

double parse_double(char* str, int start, int end);
int parse_int(char* str, int start, int end);

int compare(char* buffer1, char* buffer2, int len);
enum commands distinct_cmd(char* cmd, int start, int end);
void make_lowercase(char* str, int len);
int is_whitechar(char c);
int remove_duplication(char* cmd_args, int len);

void get_command(char* cmd_args, int len, int start, int* cmd_start, int* cmd_end);
void get_range(char * cmd_args, int len, int start, int* range_start, int* range_end);
void get_option(char* cmd_args, int len, int start, int* option_start, int* option_end);
void get_value(char* cmd_args, int len, int start, int* value_start, int* value_end);

uint16_t to_uint16(char* buf, int len);
void get_range_channels(char* cmd_args, int range_start, int range_end, range_channels* p_rc);

#endif /* COMMAND_H_ */

/*
 * procedure.h
 *
 *  Created on: 2019. 11. 11.
 *      Author: sjhong
 */

#ifndef PROCEDURE_H_
#define PROCEDURE_H_

typedef enum {
    PORT_TYPE_TCP,
    PORT_TYPE_UART,
    PORT_TYPE_USB
}send_port_type;


extern uint8_t common_send_buf[];
extern int common_send_buf_len;

extern Semaphore_Handle sem_dsp_data;
extern Semaphore_Handle sem_dsp;
extern Semaphore_Handle sem_host_send;

void main_proc_thread();
void save_ip_config();

void send_to_port(send_port_type port_type, void* handle);

void process_command(uint8_t * buffer, int len);

void dsp_data_thread();
void dsp_thread();

#endif /* PROCEDURE_H_ */

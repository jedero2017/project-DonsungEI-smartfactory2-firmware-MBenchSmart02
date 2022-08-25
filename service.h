/*
 * service.hh
 *
 *  Created on: 2022. 1. 9.
 *      Author: sjhong
 */

#ifndef SERVICE_H_
#define SERVICE_H_

#include <ti/drivers/Timer.h>
#include <ti/sysbios/knl/Semaphore.h>

//extern Semaphore_Handle sem_service_send;
extern Semaphore_Handle sem_data_send_timer;
extern SOCKET service_socket;

void service_init();

bool service_check_packet(uint8_t * buffer, int len);

void service_process(uint8_t * buffer, int len);

void service_data_send_timer_callback(Timer_Handle h_timer);
void service_data_send_thread();\

bool service_get_temperature(float* arr_tem, int len);
bool service_get_powermeter(float* arr_pow, int len);
bool service_get_vibration(float* arr_vib, int len);
bool service_get_speed(float* arr_speed, int len);

float read_big_endian_float(uint8_t* bytes);
double read_big_endian_double(uint8_t* bytes);


#endif /* SERVICE_H_ */

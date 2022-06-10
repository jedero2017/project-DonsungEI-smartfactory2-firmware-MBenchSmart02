/*
 * vse153.h
 *
 *  Created on: 2022. 1. 9.
 *      Author: sjhong
 */

#ifndef VSE153_H_
#define VSE153_H_

extern bool vse153_connected;

void vse153_init();
bool vse153_connect(uint32_t ip, int port);
bool vse153_get_data(float* data, int len);





#endif /* VSE153_H_ */

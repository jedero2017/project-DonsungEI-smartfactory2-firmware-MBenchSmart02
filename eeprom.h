/*
 * eeprom.h
 *
 *  Created on: 2019. 11. 12.
 *      Author: sjhong
 */

#ifndef EEPROM_H_
#define EEPROM_H_


void eeprom_init();
int eeprom_write(uint32_t * data, uint32_t address, uint32_t len);
void eeprom_read(uint32_t * buffer, uint32_t address, uint32_t len);



#endif /* EEPROM_H_ */

/*
 * eeprom.c
 *
 *  Created on: 2019. 11. 12.
 *      Author: sjhong
 */


#include <stdint.h>

#include <ti/devices/msp432e4/driverlib/driverlib.h>
//#include <ti/display/Display.h>

//extern Display_Handle display;


void eeprom_init()
{
    uint32_t ret;
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);

    while(!(SysCtlPeripheralReady(SYSCTL_PERIPH_EEPROM0)))
    {
    }

    ret = MAP_EEPROMInit();

    if(ret != EEPROM_INIT_OK)
    {
//        Display_printf(display, 0, 0, "Error Initializing EEPROM\n");
        return;
    }
}


int eeprom_write(uint32_t * data, uint32_t address, uint32_t len)
{
    return MAP_EEPROMProgram(data, address, len);
}

void eeprom_read(uint32_t * buffer, uint32_t address, uint32_t len)
{
    MAP_EEPROMRead(buffer, address, len);
}


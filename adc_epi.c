/*
 * adc_epi.c
 *
 *  Created on: 2019. 11. 21.
 *      Author: sjhong
 */



#include <ti/devices/msp432e4/driverlib/driverlib.h>
#include <ti/drivers/GPIO.h>

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ti_drivers_config.h"


adc_epi_init()
{

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);


    MAP_GPIOPinTypeEPI(GPIO_PORTA_BASE, (GPIO_PIN_7 | GPIO_PIN_6));
    MAP_GPIOPinTypeEPI(GPIO_PORTB_BASE, (GPIO_PIN_3));
    MAP_GPIOPinTypeEPI(GPIO_PORTC_BASE, (GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 |
                                         GPIO_PIN_4));
    MAP_GPIOPinTypeEPI(GPIO_PORTG_BASE, (GPIO_PIN_1 | GPIO_PIN_0));
    MAP_GPIOPinTypeEPI(GPIO_PORTH_BASE, (GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 |
                                         GPIO_PIN_0));
//    MAP_GPIOPinTypeEPI(GPIO_PORTK_BASE, (GPIO_PIN_5));
//    MAP_GPIOPinTypeEPI(GPIO_PORTL_BASE, (GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 |
//                                         GPIO_PIN_0));
    MAP_GPIOPinTypeEPI(GPIO_PORTL_BASE, (GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0));
    MAP_GPIOPinTypeEPI(GPIO_PORTM_BASE, (GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 |
                                         GPIO_PIN_0));
//    MAP_GPIOPinTypeEPI(GPIO_PORTN_BASE, (GPIO_PIN_3 | GPIO_PIN_2));

    MAP_GPIOPinConfigure(GPIO_PH0_EPI0S0);
    MAP_GPIOPinConfigure(GPIO_PH1_EPI0S1);
    MAP_GPIOPinConfigure(GPIO_PH2_EPI0S2);
    MAP_GPIOPinConfigure(GPIO_PH3_EPI0S3);
    MAP_GPIOPinConfigure(GPIO_PC7_EPI0S4);
    MAP_GPIOPinConfigure(GPIO_PC6_EPI0S5);
    MAP_GPIOPinConfigure(GPIO_PC5_EPI0S6);
    MAP_GPIOPinConfigure(GPIO_PC4_EPI0S7);
    MAP_GPIOPinConfigure(GPIO_PA6_EPI0S8);
    MAP_GPIOPinConfigure(GPIO_PA7_EPI0S9);
    MAP_GPIOPinConfigure(GPIO_PG1_EPI0S10);
    MAP_GPIOPinConfigure(GPIO_PG0_EPI0S11);
    MAP_GPIOPinConfigure(GPIO_PM3_EPI0S12);
    MAP_GPIOPinConfigure(GPIO_PM2_EPI0S13);
    MAP_GPIOPinConfigure(GPIO_PM1_EPI0S14);
    MAP_GPIOPinConfigure(GPIO_PM0_EPI0S15);
    MAP_GPIOPinConfigure(GPIO_PL0_EPI0S16);
    MAP_GPIOPinConfigure(GPIO_PL1_EPI0S17);
    MAP_GPIOPinConfigure(GPIO_PL2_EPI0S18);
//    MAP_GPIOPinConfigure(GPIO_PL3_EPI0S19);
    MAP_GPIOPinConfigure(GPIO_PB3_EPI0S28);
//    MAP_GPIOPinConfigure(GPIO_PN2_EPI0S29);
//    MAP_GPIOPinConfigure(GPIO_PN3_EPI0S30);
//    MAP_GPIOPinConfigure(GPIO_PK5_EPI0S31);


    /* Enable the clock to the EPI and wait for it to be ready */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_EPI0);
    while(!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_EPI0)))
    {
    }

    MAP_EPIDividerSet(EPI0_BASE, 4);

    MAP_EPIModeSet(EPI0_BASE, EPI_MODE_HB16);

    MAP_EPIConfigHB16Set(EPI0_BASE, EPI_HB16_MODE_ADDEMUX | EPI_HB16_RDWAIT_3 , 0);

    MAP_EPIAddressMapSet(EPI0_BASE, EPI_ADDR_RAM_SIZE_256MB | EPI_ADDR_RAM_BASE_6);

    while(EPI0->STAT & EPI_STAT_INITSEQ)
    {
    }

    //adc reset
    GPIO_write(CONFIG_GPIO_ADCRESET, 1);
    GPIO_write(CONFIG_GPIO_ADCRESET, 0);


}

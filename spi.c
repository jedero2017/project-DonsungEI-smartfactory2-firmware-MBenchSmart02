/*
 * spi.c
 *
 *  Created on: 2019. 11. 12.
 *      Author: sjhong
 */


//#include <ti/display/Display.h>
#include <ti/drivers/SPI.h>
#include "ti_drivers_config.h"

//extern Display_Handle display;

SPI_Handle      h_spi;


void spi_init()
{
    SPI_Params      spiParams;

    SPI_init();

    //SPI 2 : board control spi
    SPI_Params_init(&spiParams);
    spiParams.frameFormat = SPI_POL0_PHA0;
    spiParams.bitRate = 2000000;
    spiParams.dataSize = 8;

//    h_spi = SPI_open(CONFIG_SPI_0, &spiParams);
    if (h_spi == NULL) {
 //       Display_printf(display, 0, 0, "Error initializing master SPI\n");
        while (1);
    }
    else {
//        Display_printf(display, 0, 0, "Master SPI initialized\n");
    }

}

void spi_change_mode(SPI_Handle h_spi, SPI_FrameFormat ff)
{
    SPI_Params      spiParams;


    SPI_close(h_spi);

    SPI_Params_init(&spiParams);
    spiParams.frameFormat = ff;
    spiParams.bitRate = 2000000;
    spiParams.dataSize = 8;

//    h_spi = SPI_open(CONFIG_SPI_0, &spiParams);
//    if (h_spi == NULL) {
//        Display_printf(display, 0, 0, "Error initializing master SPI\n");
//        while (1);
//    }
//    else {
//        Display_printf(display, 0, 0, "Master SPI initialized\n");
//    }
}

void spi_transfer(SPI_Handle h_spi, uint8_t * tx_buf, uint8_t * rx_buf, int count)
{
    SPI_Transaction transaction;
    bool            transferOK;

    transaction.count = count;
    transaction.txBuf = (void*)tx_buf;
    transaction.rxBuf = (void*)rx_buf;

    transferOK = SPI_transfer(h_spi, &transaction);
    if (!transferOK)
    {
//        Display_printf(display, 0, 0, "Unsuccessful master SPI transfer");
    }
}

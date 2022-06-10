/*
 * spi.h
 *
 *  Created on: 2019. 11. 12.
 *      Author: sjhong
 */

#ifndef SPI_H_
#define SPI_H_

extern SPI_Handle h_spi;

void spi_init();
void spi_change_mode(SPI_Handle h_spi, SPI_FrameFormat ff);
void spi_transfer(SPI_Handle h_spi, uint8_t * tx_buf, uint8_t * rx_buf, int count);



#endif /* SPI_H_ */

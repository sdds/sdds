#ifndef SPI_H_INCLUDED
#define SPI_H_INCLUDED

#include <stdint.h>

/*
 * the serial peripheral interface
 * -------------------------------
 *
 * a very primitive abstraction from using i/o registers
 * directly
 *
 * use spi_init first, then you can communicate with the other
 * functions
 */

/* the last set status register value */
extern uint8_t g_spsr;

/*
 * initialize the spi module
 */
void spi_init(void);

/*
 * set the control register with a specific byte
 *
 * @param byte the byte which will be put into the control register
 */
void spi_set_control(uint8_t byte);

/*
 * get the value of the control register
 *
 * @return the value of the control register
 */
uint8_t spi_get_control(void);

/*
 * set the data register with a specific byte
 *
 * @param byte the byte which will be put into the data register
 */
void spi_set_data(uint8_t byte);

/*
 * get the value of the data register
 *
 * @return the value of the data register
 */
uint8_t spi_get_data(void);

/*
 * wait for the spi command to finish
 */
void spi_wait(void);

#endif

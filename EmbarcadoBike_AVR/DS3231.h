#ifndef DS3231_H
#define DS3231_H

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <stdint.h>
#include <avr/io.h>
#include <util/twi.h> 
#include <avr/interrupt.h>

/**
 * Biblioteca para realizar a comunicacao
 * com o RTC DS3231 usando I2C com interrupcao
*/

#define ADDR_DS3231 0xD0
#define WRITE 1
#define READ 0

void start_bit_i2c(uint8_t pullup)
{

}

void stop_bit_i2c()
{

}

#endif
#ifndef DS3231_H
#define DS3231_H

/**
 * Biblioteca para realizar a comunicacao
 * com o RTC DS3231 usando I2C com interrupcao
 */

#include <stdint.h>

#include "TWI.h"

#define ADDR_DS3231 0xD0
#define WRITE 0
#define READ 1

#define SECONDS 0x00
#define MINUTES 0x01
#define HOURS 0x02
#define DAY 0x03
#define DATE 0x04
#define MONTH 0x05
#define YEAR 0x06

/**
 * @brief Funcao para pegar somente os dados do
 * relogio
 * 
 * @note Utilizado na interrupção do TWI
*/
void get_clock()
{


    TWCR |= (1 << TWINT);
}

/**
 * @brief Funcao para pegar somente os dados da
 * data (dia/mes/ano)
 * 
 * @note Utilizado na interrupção do TWI
*/
void get_date()
{

    TWCR |= (1 << TWINT);
}

#endif
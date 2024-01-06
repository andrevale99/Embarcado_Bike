#ifndef DS3231_H
#define DS3231_H

/**
 * Biblioteca para realizar a comunicacao
 * com o RTC DS3231 usando I2C com interrupcao
*/

/**
 * CORRIGIR A FUNÇÂO DE LEITURA DO CLOCK
 * DEVE REALIZAR UMA OPERACAO DE ESCRITA
 * PARA DEPOIS REALIZAR A LEITURA
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

volatile uint8_t contador_de_bytes = 0;

volatile struct DS3231_data
{
    volatile uint8_t clock[3];
    volatile uint8_t data[3];
}ds3231_data;

uint8_t situacion()
{
    return contador_de_bytes;
}

/**
 * @brief Funcao para pegar somente os dados do
 * relogio
 * 
 * @note Utilizado na interrupção do TWI
*/
void get_clock()
{
    switch (TWSR & TW_STATUS_MASK)
    {
    case TW_START:
        TWCR &= ~(1<<TWSTA);
        TWDR = ADDR_DS3231 | READ;
        break;
    
    case TW_MR_SLA_ACK:
        if(contador_de_bytes == DAY)
        {
            i2c_stop_bit();
            contador_de_bytes = 0;
        }
        contador_de_bytes = 1 + contador_de_bytes;
        break;

    case TW_MR_DATA_ACK:
        ds3231_data.clock[contador_de_bytes] = TWDR;
        break;

    case TW_SR_STOP:
        //condicao para mostrar que recebeu todos
        //os dados
        break;

    default:
        //Condicao para dizer que houve algum erro
        break;
    }

    TWCR |= (1 << TWINT);
}

#endif
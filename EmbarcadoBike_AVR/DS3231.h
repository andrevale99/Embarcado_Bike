#ifndef DS3231_H
#define DS3231_H

/**
 * Biblioteca para realizar a comunicacao
 * com o RTC DS3231 usando I2C com interrupcao
*/

/**
 * Organizar código para uma melhor legibilidade
 * e tratamento de erros
 * 
 * Como utiliza-se do snprintf para gravar no LCD
 * ha um BUG quando reseta os segundos (CORRIGIR)
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

volatile uint8_t idx_data = 0;
volatile uint8_t read_flag = 0;

volatile struct DS3231_data
{
    volatile uint8_t clock[3];
    volatile uint8_t data[3];
}ds3231_data;

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
        TWDR = ADDR_DS3231 | WRITE;
        TWCR &= ~(1<<TWSTA);
        break;

    case TW_MT_SLA_ACK:
        TWDR = SECONDS;
        break;
    
    case TW_MT_DATA_ACK:
        i2c_start_bit();
        break;
    
    case TW_REP_START:
        TWDR = ADDR_DS3231 | READ;
        TWCR &= ~(1<<TWSTA);
        break;

    case TW_MR_SLA_ACK:
        //Confirmacao que mandou o endereco do RTC
        //no modo READ
        break;

    case TW_MR_DATA_NACK:
        ds3231_data.clock[0] = TWDR;
        i2c_stop_bit();
        PORTB |= (1<<PB5);
        break; 

    case TW_SR_STOP:
        
        break;

    default:
        //Condicao para dizer que houve algum erro
        break;
    }

    TWCR |= (1 << TWINT);
}

#endif
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

#define I2C_DDR DDRC
#define I2C_PORT PORTC

#define SCL PC5
#define SDA PC4


/**
 * @brief Inicializa o protocolo I2C
 * 
 * @note NAO ATIVA os pullups do SCL e SDA
*/
void init_i2c()
{
    I2C_DDR  &= ~((1<<SCL) | (1<<SDA));

    //Ajuste da frequência de trabalho - SCL = F_CPU/(16+2.TWBR.Prescaler)
    TWBR = 18;
    TWSR |= 0x01;//prescaler = 4;
    TWCR |= (1<<TWINT) | (1<<TWEN) | (1<<TWIE); //habilita o TWI com interrupcao
}

/**
 * @brief Inicializa o protocolo I2C
 * 
 * @note ATIVA os pullups do SCL e SDA
*/
void init_i2c_pullup()
{
    I2C_PORT |= (1<<SCL) | (1<<SDA);
    init_i2c();
}

/**
 * @brief Star bit do I2C
*/
void i2c_start_bit()
{
    TWCR |= (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
}

/**
 * @brief Stop bit do I2C
*/
void i2c_stop_bit()
{
    TWCR |= ((1<<TWINT) | (1<<TWSTO) | (1<<TWEN)) & ~(1<<TWSTA);
}


#endif
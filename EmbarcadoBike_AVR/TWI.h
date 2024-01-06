#ifndef TWI_H
#define TWI_H

#include <avr/io.h>
#include <util/twi.h> 

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
 * 
 * @note Tambem utilizado para mandar
 * o REPEATED START
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
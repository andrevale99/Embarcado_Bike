#ifndef DS3231_H
#define DS3231_H

/**
 * Biblioteca para realizar a comunicacao
 * com o RTC DS3231 usando I2C com interrupcao
 */

/**
 * @implements Organizar código para uma melhor legibilidade
 * e tratamento de erros
 */

#include <stdint.h>

#include "TWI.h"

#define DS3231_ADDR 0xD0

#define SECONDS 0x00
#define MINUTES 0x01
#define HOURS 0x02
#define DAY 0x03
#define DATE 0x04
#define MONTH 0x05
#define YEAR 0x06

#define IDLE_STATE 0x00
#define READ_BYTE_STATE 0x01
#define WRITE_BYTE_STATE 0x2
#define GET_CLOCK_STATE 0x03

volatile uint8_t MASK_DS3231 = IDLE_STATE; // Estados o qual esta (LEITURA, ESCRITA OU LIBERADO)
volatile uint8_t pointer = 0;              // Ponteiro usado como indice de um vetor
volatile uint8_t write_value = 0;          // Valor que sera escrito na memoria do RTC

// Contador para escrita, pois precisa primeiro atualizar o
// ponteiro na memoria no RTC, para depois escrever o valor que
// deseja na memoria, ou seja, para gravar dado precisar realizar
// duas escritas.
volatile uint8_t write_counter = 0;

volatile uint8_t ds3231_data[7];

/**
 * @brief Le um byte do RTC
 *
 * @param addr_ptr o endereco que deseja ler
 */
void read_byte_DS3231(uint8_t addr_ptr)
{
    MASK_DS3231 = READ_BYTE_STATE;
    pointer = addr_ptr;
    i2c_start_bit();
}

/**
 * @brief Funcao que escreve 1 byte no RTC
 * 
 * @param addr_ptr Endereco que deseja escrever
 * @param value O valor que deseja escrever
*/
void write_byte_DS3231(uint8_t addr_ptr, uint8_t value)
{
    MASK_DS3231 = WRITE_BYTE_STATE;
    pointer = addr_ptr;
    write_value = value;
    i2c_start_bit();
}

/**
 * @brief Funcao para ler os dados das horas 
 * do RTC
 * 
 * @note Essa condicao (o "if") serve para realizar
 * a operacao somente 1 vez, ou seja, so ira realizar
 * a operacao quando a anterior estiver terminado
*/
void get_clock()
{
    if (MASK_DS3231 == IDLE_STATE)
    {
        MASK_DS3231 = GET_CLOCK_STATE;
        i2c_start_bit();
    }
}

/**
 * @brief Retorna algum dado lido do DS3231
 *
 * @param WHAT qual valor deseja ler
 *
 * @note Os valores de WHAT são os enderecos
 * dos dados da memoria do RTC. Ha defines ja
 * para auxiliar
 */
uint8_t get_DS3231_data(uint8_t WHAT)
{
    if (WHAT < 0 || WHAT > YEAR)
        return 255;

    return ds3231_data[WHAT];
}

/**
 * @brief Funcao para pegar somente os dados do
 * relogio
 *
 * @note Utilizado na interrupção do TWI
 */
void DS3231_rotine()
{
    if (MASK_DS3231 == READ_BYTE_STATE)
    {
        switch (TWSR & TW_STATUS_MASK)
        {
        case TW_START:
            TWDR = DS3231_ADDR | TW_WRITE;
            TWCR &= ~(1 << TWSTA);
            break;

        case TW_MT_SLA_ACK:
            TWDR = pointer;
            break;

        case TW_MT_DATA_ACK:
            i2c_start_bit();
            break;

        case TW_REP_START:
            TWDR = DS3231_ADDR | TW_READ;
            TWCR &= ~(1 << TWSTA);
            break;

        case TW_MR_SLA_ACK:
            // Confirmacao que mandou o endereco do RTC
            // no modo READ e recebeu um ACK
            break;

        case TW_MR_DATA_NACK:
            ds3231_data[pointer] = TWDR;
            MASK_DS3231 = IDLE_STATE;
            i2c_stop_bit();
            break;

        default:
            // Condicao para dizer que houve algum erro
            break;
        }
    }

    else if (MASK_DS3231 == WRITE_BYTE_STATE)
    {
        switch (TWSR & TW_STATUS_MASK)
        {
        case TW_START:
            TWDR = DS3231_ADDR | TW_WRITE;
            TWCR &= ~(1 << TWSTA);
            break;

        case TW_MT_SLA_ACK:
            TWDR = pointer;
            break;

        case TW_MT_DATA_ACK:
            if (write_counter == 1)
            {
                i2c_stop_bit();
                write_counter = 0;
            }
            TWDR = write_value;
            write_counter += 1;
            break;

        default:
            // Condicao para dizer que houve algum erro
            break;
        }
    }

    else if (MASK_DS3231 == GET_CLOCK_STATE)
    {
        switch (TWSR & TW_STATUS_MASK)
        {
        case TW_START:
            TWDR = DS3231_ADDR | TW_WRITE;
            TWCR &= ~(1 << TWSTA);
            break;

        case TW_MT_SLA_ACK:
            TWDR = pointer;
            break;

        case TW_MT_DATA_ACK:
            i2c_start_bit();
            break;

        case TW_REP_START:
            TWDR = DS3231_ADDR | TW_READ;
            TWCR &= ~(1 << TWSTA);
            break;

        case TW_MR_SLA_ACK:
            // Recebe o ACK do RTC
            // Troca o estado para transmitir os ACKS
            TWCR |= (1 << TWEA);
            break;

        case TW_MR_DATA_ACK:
            if (pointer == DAY)
                TWCR &= ~(1<<TWEA); // Troca o estado
            else
            {
                ds3231_data[pointer] = TWDR;
                pointer += 1;
            }
            break;

        case TW_MR_DATA_NACK:
            MASK_DS3231 = IDLE_STATE;
            pointer = 0;
            i2c_stop_bit();
            break;

        default:
            break;
        }
    }

    TWCR |= (1 << TWINT);
}

#endif
/**
 * @author Andre Menezes de Freitas Vale
 * @version 1.0.0
 *
 * @brief Um embarcado simples para colocar na bicicleta.
 * Fazendo para aperfeicoar a programacao em baixo nivel
 *
 * @implements Criar uma logica para nao utilizar os delay
 * para obter os dados do DS3231 (RTC)
 */

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Biblioteca para usar o 'snprintf()'
#include <stdio.h>

#include "LCD.h"
#include "DS3231.h"

//======================================
//  VARIAVEIS
//======================================
const char *battery_display = "BAT%:";
const char *velocity_display = "m/s:";

struct Buffer
{
  char str[16];
  volatile uint8_t size;
} buffer;

volatile uint8_t vel = 255;
volatile uint32_t bat = 0;
//======================================
//  PROTOTIPOS
//======================================
void setup();

/**
 * @brief Funcao para cnfiguracao do ADC
 */
void ADC_setup();

/**
 * @brief Funcao para cnfiguracao do TIMER0
 */
void TIMER0_setup();

/**
 * @brief Funcao para atualizar
 * os dados necessarios
 */
void refresh_data();

/**
 * @brief Funcoes de interrupcoes
 *
 * @param ADC_vect Interrupcao do ADC. No programa esta
 * sendo utilizado no modo AUTO TRIGGER
 * pela comparacao com o TIMER0 e o OCR0A
 *
 * @param TWI_vect Intterupcao do I2C (TWI). Usado para pegar
 * os dados do relogio
 */
ISR(ADC_vect);
ISR(TWI_vect);
//======================================
//  MAIN
//======================================
int main()
{

  setup();

  // Criar funcao para escrever a interface
  // do LCD, as frases que nao precisao ser
  // atualizadas
  writeLCD(battery_display, 5);

  sei();

  while (1)
  {
    read_byte(HOURS);
    _delay_ms(10);
    read_byte(MINUTES);
    _delay_ms(10);
    read_byte(SECONDS);
    _delay_ms(10);

    refresh_data();

    _delay_ms(250);
  }

  return 0;
}
//======================================
//  FUNCOES
//======================================
void setup()
{
  // Desativa os RX r TX do MCU para
  // utilizar os pinos PD0 (RX) e PD1 (TX)
  // do LCD, se n fica bugando
  UCSR0B = 0x00;

  ADC_setup();
  TIMER0_setup();
  init_4bitsLCD();
  init_i2c();
}

void ADC_setup()
{
  ADMUX = ((1 << REFS0));                                  // Referencia no AVCC
  ADCSRA = ((1 << ADIE) | (1 << ADATE) | (0x07 << ADPS0)); // Prescale de 128 e auto trigger ativado
  ADCSRB |= (1 << ADTS1) | (1 << ADTS0);                   // Auto trigger do TIMER0 com COMPA

  DIDR0 = 0x3E; // Somente o A0 está ativo

  ADCSRA |= (1 << ADEN);
}

void TIMER0_setup()
{
  TCCR0A = (1 << WGM01);                      // Pinos OC0x Desativados e na conf. CTC
  TCCR0B |= (1 << CS02) | (1 << (1 << CS00)); // Prescale de 256

  OCR0A = 25;
}

void refresh_data()
{
  buffer.size = snprintf(buffer.str, 16, "%d ", bat);
  LCD_cmd(SET_DDRAM | 5, CMD);
  writeLCD(&buffer.str[0], buffer.size);

  LCD_cmd(RETURN_HOME, CMD);
  LCD_cmd(SECOND_LINE, CMD);

  buffer.size = snprintf(buffer.str, 16, "%x:", get_DS3231_data(HOURS));
  writeLCD(&buffer.str[0], buffer.size);
  buffer.size = snprintf(buffer.str, 16, "%x:", get_DS3231_data(MINUTES));
  writeLCD(&buffer.str[0], buffer.size);
  buffer.size = snprintf(buffer.str, 16, "%x ", get_DS3231_data(SECONDS));
  writeLCD(&buffer.str[0], buffer.size);

  LCD_cmd(RETURN_HOME, CMD);
}

ISR(ADC_vect)
{
  volatile uint8_t sreg = SREG;

  static uint8_t adc_LSB;
  static uint8_t adc_MSB;

  adc_LSB = ADCL;
  adc_MSB = ADCH;

  bat = (adc_MSB << 8) | adc_LSB;

  bat = (bat * 100) / 1023;

  // Desativa o bit do trigger do TIMER0
  // necessário para realizar o proximo
  // disparo (pag. 207 - 23.3 Start Conventions)
  TIFR0 |= (1 << OCF0A);

  SREG = sreg;
}

ISR(TWI_vect)
{
  volatile uint8_t sreg = SREG;

  DS3231_rotine();

  SREG = sreg;
}
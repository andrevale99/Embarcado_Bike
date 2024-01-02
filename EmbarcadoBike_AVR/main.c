#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//Biblioteca para usar o 'snprintf()'
#include <stdio.h>

#include "LCD.h"

//======================================
//  VARIAVEIS
//======================================
const char *battery = "BAT%:";
const char *velocity = "m/s:";

volatile char buffer[16];

volatile uint8_t  vel = 0;
volatile uint32_t  bat = 0;
//======================================
//  PROTOTIPOS
//======================================
void setup();

/**
 * @brief Funcao para cnfiguracao do ADC
*/
void ADC_setup();

/**
 * @brief Funcao para cnfiguracao do TIME0
*/
void TIMER0_setup();

/**
 * @brief Interrupcao do ADC
 * 
 * @note ativada quando concluir a conversão ADC,
 * para iniciar a conversão escrever 1 no bit
 * ADSC do registrador ADCSRA ou configurar o ADC
 * no modo free running (olhar datasheet)
*/
ISR(ADC_vect);
//======================================
//  MAIN
//======================================
int main()
{
  
  setup();

  sei();

  while(1)
  {
    writeLCD(battery, 5);
    snprintf(buffer, 16, "%d ", TCNT0);
    writeLCD(&buffer[0], 4);
    LCD_cmd(RETURN_HOME, CMD);
  }

  return 0;
}
//======================================
//  FUNCOES
//======================================
void setup()
{
  ADC_setup();
  TIMER1_setup();
  UCSR0B = 0x00; //Desativa os RX r TX do MCU
  init_4bitsLCD();
}

void ADC_setup()
{
  ADMUX = ((1<<REFS0));
  ADCSRA = ((1<<ADIE) | (1<<ADATE)| (0x07 << ADPS0));
  ADCSRB |= (1 << ADTS1) | (1 << ADTS0);

  DIDR0 = 0x3E; //Somente o A0 está ativo

  ADCSRA |= (1<<ADEN);
}

void TIMER0_setup()
{
  TCCR0A = (1<<WGM01); //Pinos OC0x Desativados e na conf. CTC
  TCCR0B |= (1<<CS02) | (1<<(1<<CS00)); //Prescale de 256

  OCR0A = 156;
}

ISR(ADC_vect)
{
  static uint8_t adc_LSB;
  static uint8_t adc_MSB;

  adc_LSB = ADCL;
  adc_MSB = ADCH;

  bat = (adc_MSB << 8) | adc_LSB;

  bat = (bat * 100) / 1023;
  snprintf(buffer, 16, "%d ", bat);

  TIFR0 |= (1<<OCF0A);
}
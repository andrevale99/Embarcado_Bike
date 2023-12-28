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
char *battery = "BAT%:";
char buffer[10];
char *velocity = "m/s:";

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
    ADCSRA |= (1<<ADSC);

    writeLCD(battery, 5);

    bat = (bat * 100) / 1023;
    snprintf(buffer, 10, "%d", bat);
    writeLCD(&buffer[0], 3);

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
  UCSR0B = 0x00;
  init_4bitsLCD();
}

void ADC_setup()
{
  ADMUX = ((1<<REFS0));
  ADCSRA = ((1<<ADEN) | 0x07 | (1<<ADIE));

  DIDR0 = 0x3E; //Somente o A0 está ativo
}

ISR(ADC_vect)
{
  static uint8_t adc_LSB;
  static uint8_t adc_MSB;

  adc_LSB = ADCL;
  adc_MSB = ADCH;

  bat = (adc_MSB << 8) | adc_LSB;
}
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//Biblioteca para usar o 'snprintf()'
#include <stdio.h>

#include "LCD.h"
#include "DS3231.h"

//======================================
//  VARIAVEIS
//======================================
const char *battery_display = "BAT%:";
const char *velocity_display = "m/s:";

volatile struct buffer
{
  volatile char str[16];
  volatile uint8_t size;
}buffer;

volatile uint8_t  vel = 255;
volatile uint32_t  bat = 255;
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

  //Criar funcao para escrever a interface 
  //do LCD, as frases que nao precisao ser
  //atualizadas
  writeLCD(battery_display,5);

  while(1)
  {
    buffer.size = snprintf(buffer.str, 16, "%d", bat);
    LCD_cmd(SET_DDRAM | 5, CMD);
    writeLCD(&buffer.str[0], buffer.size);

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
  TIMER0_setup();
  UCSR0B = 0x00; //Desativa os RX r TX do MCU
  init_4bitsLCD();
}

void ADC_setup()
{
  ADMUX = ((1<<REFS0)); //Referencia no AVCC
  ADCSRA = ((1<<ADIE) | (1<<ADATE)| (0x07 << ADPS0)); //Prescale de 128 e auto trigger ativado
  ADCSRB |= (1 << ADTS1) | (1 << ADTS0); //Auto trigger do TIMER0 com COMPA

  DIDR0 = 0x3E; //Somente o A0 está ativo

  ADCSRA |= (1<<ADEN);
}

void TIMER0_setup()
{
  TCCR0A = (1<<WGM01); //Pinos OC0x Desativados e na conf. CTC
  TCCR0B |= (1<<CS02) | (1<<(1<<CS00)); //Prescale de 256

  OCR0A = 25;
}

ISR(ADC_vect)
{
  static uint8_t adc_LSB;
  static uint8_t adc_MSB;

  adc_LSB = ADCL;
  adc_MSB = ADCH;

  bat = (adc_MSB << 8) | adc_LSB;

  bat = (bat * 100) / 1023;

  //Desativa o bit do trigger do TIMER0
  //necessário para realizar o proximo
  //disparo (pag. 207 - 23.3 Start Conventions)
  TIFR0 |= (1<<OCF0A);
}
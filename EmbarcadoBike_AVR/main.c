#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "LCD.h"
//======================================
//  VARIAVEIS
//======================================

//======================================
//  PROTOTIPOS
//======================================
void ADC_setup();

ISR(ADC_vect);
//======================================
//  MAIN
//======================================
int main()
{
  init_4bitsLCD();
  ADC_setup();

  sei();
 
  while(1)
  {

  }

  return 0;
}
//======================================
//  FUNCOES
//======================================
void ADC_setup()
{
  ADMUX = ((1<<REFS0));
  ADCSRA = ((1<<ADEN) | 0x07 | (1<<ADIE));

  DIDR0 = 0x3E; //Somente o A0 está ativo
}

ISR(ADC_vect)
{
}
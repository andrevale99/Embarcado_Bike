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
/**
 * @brief Funcao para cnfiguracao do ADC
*/
void ADC_setup();

/**
 * @brief Interrupcao do ADC
 * 
 * @note ativada quando concluir a conversão ADC,
 * para iniciar a conversão escrever 1 no bit
 * ADSC do registrador ADCSRA
*/
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
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "LCD.h"


int main()
{
  init_4bitsLCD();
  
  char *msg = "andre\0";
  for( ; *msg!='\0'; msg++)
  {
    LCD_cmd(*msg, WRITE);
    _delay_ms(250);
  }

  while(1)
  {

  }

  return 0;
}
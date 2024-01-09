/**
 * @author Andre Menezes de Freitas Vale
 * @version 1.1.0
 *
 * @brief Um embarcado simples para colocar na bicicleta.
 * Fazendo para aperfeicoar a programacao em baixo nivel
 *
 * @implements Ha um problema de ruido no butao
 * colocar alguma logica no programa ou componente
 * no butao para filtrar o ruido
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

#define BAT_OFFSET_LCD 5
#define VEL_OFFSET_LCD 13
#define CLOCK_OFFSET_LCD 4

#define BUTTONS_DDRx DDRD
#define BUTTONS_PORTx PORTD
#define BUTTON_CONF PD0
#define BUTTON_OK PD1
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

void (*f)();
//======================================
//  PROTOTIPOS
//======================================
/**
 * @brief Funcao que chama as outras funcoes
 * de setup
 *
 * @note Serve mais para organizacao
 */
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
 * @brief Configuracoes dos GPIOS
 */
void GPIOx_setup();

/**
 * @brief Escreve as coisas que nao precisam
 * ficar atualizandos tipo as strings
 * battery_diplay e velocity_display
 */
void escrever_interface();

/**
 * @brief Funcao para atualizar
 * os dados necessarios
 */
void refresh_data();

/**
 * @brief Rotina Inicial, mostra os dados
 * de relogio, bateria e velocidade
 */
void rotina_principal();

/**
 * @brief Para escrever/corrigir as horas
 * do relogio
 */
void rotina_ecrita_relogio();

/**
 * @brief Funcoes de interrupcoes
 *
 * @param ADC_vect Interrupcao do ADC. No programa esta
 * sendo utilizado no modo AUTO TRIGGER
 * pela comparacao com o TIMER0 e o OCR0A
 *
 * @param TWI_vect Intterupcao do I2C (TWI). Usado para pegar
 * os dados do relogio
 *
 * @param PCINT2_vect Interrupcao na mudanca de estado
 * dos pinos PCINT23...16 (PD7...PD0)
 */
ISR(ADC_vect);
ISR(TWI_vect);
ISR(PCINT2_vect);
//======================================
//  MAIN
//======================================
int main()
{

  setup();

  _delay_ms(200);

  f = &rotina_principal;

  escrever_interface();

  sei();

  while (1)
  {
    f();
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
  GPIOx_setup();
  init_4bitsLCD();
  init_i2c();
}

void ADC_setup()
{
  ADMUX = ((1 << REFS0));                                  // Referencia no AVCC e ADC0 aberto
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

void GPIOx_setup()
{
  // Coloca os butoes como inputs e com o
  // pull-ups ativados
  BUTTONS_DDRx &= ~((1 << BUTTON_CONF) | (1 << BUTTON_OK));
  BUTTONS_PORTx |= ((1 << BUTTON_CONF) | (1 << BUTTON_OK));

  PCICR |= (1 << PCIE2);
  PCMSK2 |= (1 << PCINT16) | (1 << PCINT17);
}

void escrever_interface()
{
  LCD_cmd(RETURN_HOME, CMD);
  LCD_cmd(CLEAR_DISPLAY, CMD);
  writeLCD(battery_display, 5);
  LCD_cmd(SET_DDRAM | 9, CMD);
  writeLCD(velocity_display, 4);
}

void refresh_data()
{
  buffer.size = snprintf(buffer.str, 16, "%d ", bat);
  LCD_cmd(SET_DDRAM | BAT_OFFSET_LCD, CMD);
  writeLCD(&buffer.str[0], buffer.size);

  buffer.size = snprintf(buffer.str, 16, "%d  ", vel);
  LCD_cmd(SET_DDRAM | VEL_OFFSET_LCD, CMD);
  writeLCD(&buffer.str[0], buffer.size);

  LCD_cmd(RETURN_HOME, CMD);
  LCD_cmd(SET_DDRAM | SECOND_LINE | CLOCK_OFFSET_LCD, CMD);

  buffer.size = snprintf(buffer.str, 16, "%x:", get_DS3231_data(HOURS));
  writeLCD(&buffer.str[0], buffer.size);
  buffer.size = snprintf(buffer.str, 16, "%x:", get_DS3231_data(MINUTES));
  writeLCD(&buffer.str[0], buffer.size);
  buffer.size = snprintf(buffer.str, 16, "%x   ", get_DS3231_data(SECONDS));
  writeLCD(&buffer.str[0], buffer.size);

  LCD_cmd(RETURN_HOME, CMD);
}

void rotina_principal()
{
  get_clock();
  refresh_data();
}

void rotina_ecrita_relogio()
{
  LCD_cmd(CLEAR_DISPLAY, CMD);
  LCD_cmd(RETURN_HOME, CMD);

  writeLCD("ESCRITA RELOGIO", 15);
  LCD_cmd(SECOND_LINE, CMD);

  do
  {
    writeLCD("HOR", 3);
    LCD_cmd(SECOND_LINE, CMD);
    //Ler potenciometro para ajustar as horas
  } while (PIND & (1 << PIND1));

  while (!(PIND & (1 << PIND1)))
    ;

  do
  {
    writeLCD("MIN", 3);
    LCD_cmd(SECOND_LINE, CMD);
    //Ler potenciometro para ajustar os minutos
  } while (PIND & (1 << PIND1));

  while (!(PIND & (1 << PIND1)))
    ;

  do
  {
    writeLCD("SEC", 3);
    LCD_cmd(SECOND_LINE, CMD);
    //Ler potenciometro para ajustar os segundos
  } while (PIND & (1 << PIND1));

  while (!(PIND & (1 << PIND1)))
    ;

  _delay_ms(2000);

  escrever_interface();

  f = &rotina_principal;
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

ISR(PCINT2_vect)
{
  volatile uint8_t sreg = SREG;

  if (!(PIND & (1 << PIND0)))
    f = &rotina_ecrita_relogio;

  SREG = sreg;
}
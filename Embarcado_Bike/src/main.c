#include <stm32f411xe.h>
#include "GPIO_Setup.h"
#include "TIMER_Setup.h"
//=======================================================
//  VARIAVEIS
//=======================================================

//=======================================================
//  PROTOTIPOS
//=======================================================
void Setup(void);                 

extern void EXTI15_10_IRQHandler(void);

//=======================================================
//  MAIN
//=======================================================
int main(void)
{

    Setup();

    while(1)
    {
    }

    return 0;
}
//=======================================================
//  FUNCOES
//=======================================================
/**
 * @brief Setup dos Perifericos
*/
void Setup(void)
{
    GPIO_setup();
    EXTI_setup();

    TIMER_Setup();
}

/**
 * @brief Funcao de Interrupcao externa dos pinos
 * 10 ao 15
*/
void EXTI15_10_IRQHandler(void)
{
    //Verifica se foi o pino PB12 o acionado
    if((EXTI->PR & EXTI_PR_PR12))
    {
        //Desativa o bit de espera
        EXTI->PR |= EXTI_PR_PR12;
        TIM2->CCER ^= TIM_CCER_CC2E;
        TIM2->CR1 ^= TIM_CR1_CEN; 
    }
    //NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
}
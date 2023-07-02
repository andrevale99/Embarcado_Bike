#include "TIMER_Setup.h"

/**
 * @brief Configuracao do TIMER2 como PWM edge-aligned
*/
void TIMER_Setup(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->PSC =  1024; //Prescale
    TIM2->ARR = 15625; //Autorreload
    TIM2->CCR2 = (uint16_t)15625 / 10; //Comparador

    //Ativa o TIMER como PWM 1 e ativa o preload
    TIM2->CCMR1 |= (0x6UL << TIM_CCMR1_OC2M_Pos) | TIM_CCMR1_OC2PE;

    //Ativa o Pino (fica assincrono) e Inverte a polaridade de ativacao
    TIM2->CCER |= TIM_CCER_CC2E | TIM_CCER_CC2P;

    //Gera o evento ao ocorrer a comparacao
    TIM2->EGR |= TIM_EGR_UG;

    //Ativa o autorreload e o timer
    TIM2->CR1 |= TIM_CR1_ARPE | TIM_CR1_CEN;
}
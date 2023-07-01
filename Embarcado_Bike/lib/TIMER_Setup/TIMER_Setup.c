#include "TIMER_Setup.h"

void TIMER_Setup(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->PSC =  1024;
    TIM2->ARR = 15625;
    TIM2->CCR2 = (uint16_t)15625 / 2;

    TIM2->CCMR1 |= (0x6UL << TIM_CCMR1_OC2M_Pos) | TIM_CCMR1_OC2PE;

    TIM2->CCER |= TIM_CCER_CC2E;

    TIM2->EGR |= TIM_EGR_UG;

    TIM2->CR1 |= TIM_CR1_ARPE | TIM_CR1_CEN;
}
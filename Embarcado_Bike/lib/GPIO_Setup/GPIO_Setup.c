#include "GPIO_Setup.h"

/**
 * @brief Configura os Pinos do MCU
 * 
 * @note PC13 como output (LED)
 * @note PB12 como Input com PULL-UP
*/
void GPIO_setup(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOBEN;


    //LED PC13
    GPIOC->BSRR |= GPIO_BSRR_BS13;
    GPIOC->MODER |= GPIO_MODER_MODER13_0;
    GPIOC->OTYPER |= 0;
    GPIOC->OSPEEDR |= 0;
    GPIOC->PUPDR |= 0;

    //Pino B12, Buttom
    GPIOB->MODER |= 0;
    GPIOB->OTYPER |= 0;
    GPIOB->OSPEEDR |= 0;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPD12_0;

}

/**
 * @brief Configuracao para interrupcao externas
 * nos pinos
 * 
 * @note EXTI do PB12 com Borda de Descida
*/
void EXTI_setup(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI12_PB;
    EXTI->IMR |= EXTI_IMR_MR12;
    EXTI->FTSR |= EXTI_FTSR_TR12;

    NVIC_SetPriority(EXTI15_10_IRQn, 5);
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/**
 * @brief Funcao de Interrupcao externa dos pinos
 * 10 ao 15
*/
void EXTI15_10_IRQHandler(void)
{
    //Verifica se foi o pino PB12 o acionado
    if(EXTI->PR & EXTI_PR_PR12)
    {
        //Desativa o bit de espera
        EXTI->PR |= EXTI_PR_PR12; 
    }
    //NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
}
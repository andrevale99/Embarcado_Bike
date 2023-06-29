#ifndef GPIO_SETUP_H
#define GPIO_SETUP_H

#include <stm32f411xe.h>

void GPIO_setup(void);

void EXTI15_10_IRQHandler(void);

#endif
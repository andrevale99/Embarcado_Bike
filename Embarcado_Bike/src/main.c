#include <stm32f411xe.h>
#include "GPIO_Setup.h"

extern volatile uint32_t flag;

void Setup(void);

int main(void)
{

    Setup();

    while(1)
    {

    }

    return 0;
}

void Setup(void)
{
    GPIO_setup();
    EXTI_setup();
}
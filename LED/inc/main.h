#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

const uint16_t LEDS = GPIO_Pin_7 | GPIO_Pin_14 | GPIO_Pin_0;
const uint16_t LED[3] = {GPIO_Pin_7, GPIO_Pin_14, GPIO_Pin_0};

void init();
void loop();

void delay();

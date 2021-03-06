#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"

#define RCC_FLAG_MASK  ((uint8_t)0x1FU)
#define RCC_GET_FLAG(__FLAG__) (((((((__FLAG__) >> 5U) == 1U)? RCC->CR :((((__FLAG__) >> 5U) == 2U) ? RCC->BDCR :((((__FLAG__) >> 5U) == 3U)? RCC->CSR :RCC->CIR))) & (1U << ((__FLAG__) & RCC_FLAG_MASK)))!= 0U)? 1U : 0U)

#define DELAY_TIM_FREQUENCY_MS 1000			/* = 1kHZ -> timer runs in milliseconds */

const uint16_t LEDS = GPIO_Pin_7 | GPIO_Pin_14 | GPIO_Pin_0;
const uint16_t LED[3] = {GPIO_Pin_7, GPIO_Pin_14, GPIO_Pin_0};

void init();
void loop();

void setClock(uint32_t _PLLM, uint32_t _PLLN, uint32_t _PLLP);
uint32_t getClock(uint32_t _PLLM, uint32_t _PLLN, uint32_t _PLLP);

void init_ms();
void stop_timer();
void delay_ms(uint32_t ms);

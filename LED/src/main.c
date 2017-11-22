#include "main.h"

int main() {
    init();

    do {
        loop();
    } while (1);
}

void init() {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef gpio;
    GPIO_StructInit(&gpio);
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_Pin = LEDS;
    GPIO_Init(GPIOB, &gpio);

    GPIO_SetBits(GPIOB, LEDS);
}

void loop() {
    static uint32_t counter = 0;

    ++counter;

    GPIO_ResetBits(GPIOB, LEDS);
    GPIO_SetBits(GPIOB, LED[counter % 3]);

    delay(10);
}

void delay(uint32_t ms) {
    ms *= 3360;
    while(ms--) {
        __NOP();
    }
}

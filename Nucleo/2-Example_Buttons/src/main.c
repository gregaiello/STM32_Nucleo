#include "main.h"

static uint8_t lastButtonStatus = RESET;

int main() {
    init();

    do {
        loop();
    } while (1);
}

void init() {
    initLeds();
    initButton();
}

void loop() {
    static uint32_t counter = 0;

    uint8_t currentButtonStatus = GPIO_ReadInputDataBit(GPIOC, USER_BUTTON);

    if (lastButtonStatus != currentButtonStatus && currentButtonStatus != RESET) {
        ++counter;
        GPIO_ResetBits(GPIOB, LEDS);
        GPIO_SetBits(GPIOB, LED[counter % 4]);
    }
    lastButtonStatus = currentButtonStatus;
}

void initLeds() {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef gpio;
    GPIO_StructInit(&gpio);
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_Pin = LEDS;
    GPIO_Init(GPIOB, &gpio);
}

void initButton() {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef gpio;
    GPIO_StructInit(&gpio);
    gpio.GPIO_Mode = GPIO_Mode_IN;
    gpio.GPIO_Pin = USER_BUTTON;
    GPIO_Init(GPIOC, &gpio);
}

void delay(uint32_t ms) {
    ms *= 3360;
    while(ms--) {
        __NOP();
    }
}

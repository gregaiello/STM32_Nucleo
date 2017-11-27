#include "main.h"

uint32_t counter = 0;
uint32_t HSI_CLOCK = 16000000;
uint32_t _PLLM =  8;
uint32_t _PLLP =  2; 
uint32_t _PLLN =  100; 

int main() {
	setClock(_PLLM, _PLLN, _PLLP);
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
    	++counter;
    	GPIO_ResetBits(GPIOB, LEDS);
	GPIO_ToggleBits(GPIOB, LED[counter % 3]);
    	//GPIO_SetBits(GPIOB, LED[counter % 3]);
	delay_ms(1000);
}

void init_ms() {
	// Enable clock for TIM2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	// Time base configuration
	TIM_TimeBaseInitTypeDef TIM;
	TIM_TimeBaseStructInit(&TIM);
	TIM.TIM_Prescaler = (SystemCoreClock/DELAY_TIM_FREQUENCY_MS)-1;
	TIM.TIM_Period = UINT16_MAX;
	TIM.TIM_ClockDivision = 0;
	TIM.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2,&TIM);
	// Enable counter for TIM2
	TIM_Cmd(TIM2,ENABLE);
}

void stop_timer() {
	TIM_Cmd(TIM2,DISABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,DISABLE);
}

void delay_ms(uint32_t ms) {
	// Init and start timer
	init_ms();
	// Dummy loop with 16 bit count wrap around
	volatile uint32_t start = TIM2->CNT;
	while((TIM2->CNT-start) <= ms);
	// Stop timer
	stop_timer();
}

void setClock(uint32_t _PLLM, uint32_t _PLLN, uint32_t _PLLP)
{ 
        /*Things to setup:
		1-	Enable power supply: APB1EN bit 28 set to 0b1
		2-	Set power supply: PWR_CR bit 14-15 set to 0b11 (<100MHz)
		3-	Set HSI as system clock: RCC_CFGR bit 1-2 set to 0b00 
		4-	Enable HSI: RCC_CR bit 1 set to 0b1
		a-	Wait until HSI is ready: RCC_CR bit 2 set by hardware to 1 when HSI is ready
		5-	Disable PLL: RCC_CR bit 24 set to 0b0
		b-	Wait until PLL is ready: RCC_CR bit 25 set by hardware to 0 when PLL is ready
		6-	HSI as PLL source: RCC_PLLCFGR bit 22 set to 0b0
		7-	PLLM = 8: RCC_PLLCFGR bit 0-5 set to 0b010000
		8-	PLLN = 100: RCC_PLLCFGR bit 6-14 set to 0b001100100
		9-	PLLP = 2: RCC_PLLCFGR bit 16-17 set to 0b00
		10-	Enable PLL: RCC_CR bit 24 set to 0b1
		c-	Wait until PLL is ready: RCC_CR bit 25 set by hardware to 1 when PLL ready	
	*/
	
	RCC->APB1ENR |= RCC_APB1ENR_PWREN; // Power interface clock enable
	
	uint32_t tmp;
	tmp = (PWR->CR & (~((uint32_t) 0xC000))); // Mask to reset bits we want to change (0b11 << 14)
	PWR->CR = (tmp | ((uint32_t) PWR_CR_VOS)); // Regulator voltage scaling output selection (scale 1)

	RCC->CR |= RCC_CR_HSION; // Enable HSI, RCC_CR bit 1 set to 1
	
	while(RCC_GET_FLAG(RCC_FLAG_HSIRDY) == RESET) // wait until HSI is ready and running
	{
	}
	
        RCC->CFGR = 0x0;       		//no APB high-speed prescaler
	                                //no AHB prescaler
	                                //HSI set as system clock 

	(RCC->CR) &= (~(RCC_CR_PLLON)); // Disable PLL, RCC_CR bit 24 set to 0

	while(RCC_GET_FLAG(RCC_FLAG_PLLRDY) != RESET) // wait until PLL is ready
	{
	}

	(RCC->PLLCFGR) &= (~(RCC_PLLCFGR_PLLSRC)); // Set HSI as PLL input, bit 22 set to 0

	uint32_t tmp1;
        tmp1 = (RCC->CFGR & (~(RCC_CFGR_PPRE1))); // Create mask to set 101 (division by 4) for APB low speed clk
        RCC->CFGR = (tmp1 | ((uint32_t) RCC_CFGR_PPRE1_DIV4)); // Prescaler APB low speed = 4  
	
	uint32_t tmp2;
        tmp2 = RCC->PLLCFGR & (~((uint32_t) RCC_PLLCFGR_PLLM)); // Create mask to set PLLM (~0b111111) 
        RCC->PLLCFGR = (tmp2 | (((uint32_t) _PLLM))); // PLLM set (bit 0:5)  
	
	uint32_t tmp3;
        tmp3 = RCC->PLLCFGR & ~((uint32_t) RCC_PLLCFGR_PLLN); // Create mask to set PLLN (~0b111111111000000) 
        RCC->PLLCFGR = (tmp3 | (((uint32_t) _PLLN) << 6)); // PLLN set (bit 6:14)  
	
        uint32_t tmp4;
        tmp4 = RCC->PLLCFGR & (~((uint32_t) RCC_PLLCFGR_PLLP)); // Create mask to set PLLP (~0b110000000000000000) 
        RCC->PLLCFGR = (tmp4 | (((uint32_t) _PLLP) << 16)); // PLLP set (bit 16:17) 

	RCC->CR |= ((uint32_t) RCC_CR_PLLON); // Enable PLL (everything has to be already set)
	
        while(RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET) // wait until PLL is ready
	{
	}

	uint32_t tmp5;
        tmp5 = RCC->CFGR & (~((uint32_t) RCC_CFGR_SW)); // Create mask to set PLL as clock input (~0b11) 
        RCC->CFGR = (tmp5 | ((uint32_t) RCC_CFGR_SW_PLL)); // PLL as system clock input

	while(RCC_GET_FLAG(RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL) // wait until system clock is ready
	{
	}
}

uint32_t getClock(uint32_t _PLLM, uint32_t _PLLN, uint32_t _PLLP)
{
	return (((HSI_CLOCK/_PLLM)*_PLLN)/_PLLP);

}

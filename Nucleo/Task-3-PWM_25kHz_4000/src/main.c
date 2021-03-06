#include "main.h"

uint32_t HSI_CLOCK = 16000000;
uint32_t _PLLM =  8;
uint32_t _PLLP =  0; // it means PLLP = 2 
uint32_t _PLLN =  100; 

int main() 
{
	setClock(_PLLM, _PLLN, _PLLP);
    	init_Button();
    	init_LEDS();
	init_PWM();
	do {
        	loop();
		for(int i = 0; i<1000; i++);
    	} while (1);
}

void loop() {
	switch(counter)
	{
		case 4000: 

				delta = -1;
				break;
		case 1: 
				delta = 1;
				break;
	}	
	counter += delta;
	TIM_SetCompare2(TIM4, counter);	

	uint8_t currentButtonStatus = GPIO_ReadInputDataBit(GPIOC, USER_BUTTON);
	if (lastButtonStatus != currentButtonStatus && currentButtonStatus != RESET) 
	{
		brigthLed2 *= 2;
		if (brigthLed2 >= 4000 ) 
		{
		        brigthLed2 = 2500;
		}
		TIM_SetCompare3(TIM3, brigthLed2);
		TIM_SetCompare1(TIM12, brigthLed2);

	}
	lastButtonStatus = currentButtonStatus;
}

void init_Button() 
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_Pin = USER_BUTTON;
	GPIO_Init(GPIOC, &gpio);
}

void init_LEDS() 
{
	/*  Configure the TIM pins by configuring the corresponding GPIO pins */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure_AF;
	GPIO_InitStructure_AF.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_0 | GPIO_Pin_14;
	GPIO_InitStructure_AF.GPIO_Mode = GPIO_Mode_AF;
	//GPIO_InitStructure_AF.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure_AF.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure_AF.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure_AF);
}

void init_PWM()
{
/* Registers to setup:
		1. Enable TIM clock using RCC_APBxPeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE) function
                2. Fill the TIM_TimeBaseInitStruct with the desired parameters including:
          		- TIM_Period = 1000
          		- TIM_Prescaler = PrescalerValue (to be understood in detail)
          		- TIM_ClockDivision = 0 (to be understood in detail)
          		- TIM_CounterMode = TIM_CounterMode_Up
       		3. Call TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStruct)
       		4. Fill the TIM_OCInitStruct with the desired parameters including:
          		- The TIM Output Compare mode: TIM_OCMode_PWM1
          		- TIM Output State: TIM_OutputState_Enable
          		- TIM Pulse value: TIM_Pulse = 0
          		- TIM Output Compare Polarity : TIM_OCPolarity_High
       		5. Call TIM_OCxInit(TIMx, &TIM_OCInitStruct)
		6. Call TIM_OCxPreloadConfig(TIMx, TIM_OCPreload_ENABLE)
                7. Call the TIM_Cmd(ENABLE) function to enable the TIM counter.
*/

	/* TIM4 clockenable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); // APB1 runs at 50MHz or 100MHz? (not clear in DS)
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // APB1 runs at 50MHz or 100MHz? (not clear in DS)
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE); // APB1 runs at 50MHz or 100MHz? (not clear in DS)

	/* Compute the prescaler value */
	uint16_t PrescalerValue = 0; //(uint32_t) (SystemCoreClock / 10000000) - 1; // 20MHz f_cnt (in the library is added a +1 take it into account)
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = (uint16_t) 3999; // f_pwm = 10MHz/10000 = 1 kHz
	TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) PrescalerValue;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);

	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_TIM12);

	/* PWM1 Mode configuration: Channel 1, 3 and 2 */	
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_OC1Init(TIM12, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM12, TIM_OCPreload_Enable);

	/* PWM Enable */
	TIM_Cmd(TIM4, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	TIM_Cmd(TIM12, ENABLE);
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
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	/**Configure the main internal regulator output voltage */
	RCC_PWR_CLK_ENABLE();
	PWR_VOLTAGESCALING_CONFIG(PWR_CR_VOS);

	/**Initializes the CPU, AHB and APB busses clocks */
	RCC_OscInitStruct.OscillatorType = 0x2;
	RCC_OscInitStruct.HSIState = (uint8_t) 0x1;
	RCC_OscInitStruct.HSICalibrationValue = 16;
	RCC_OscInitStruct.PLL.PLLState = (uint8_t) 0x2;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLCFGR_PLLSRC_HSI;
	RCC_OscInitStruct.PLL.PLLM = _PLLM;
	RCC_OscInitStruct.PLL.PLLN = _PLLN;
	RCC_OscInitStruct.PLL.PLLP = _PLLP;
	
	PLL_Disable();
	
	(RCC->PLLCFGR) &= (uint32_t) (~(RCC_PLLCFGR_PLLSRC)); // Set HSI as PLL input, bit 22 set to 0
	
	uint32_t tmp1;
        tmp1 = (RCC->CFGR & (uint32_t)(~(RCC_CFGR_PPRE1)));
        RCC->CFGR = (tmp1 | ((uint32_t) RCC_CFGR_PPRE1_DIV2)); // Prescaler APB1 low speed = 2 (50MHz)  
	
	uint32_t tmp2;
        tmp2 = RCC->PLLCFGR & (uint32_t)(~((uint32_t) RCC_PLLCFGR_PLLM)); // Create mask to set PLLM (~0b111111) 
        RCC->PLLCFGR = (tmp2 | (((uint32_t) _PLLM))); // PLLM set (bit 0:5)  
	
	uint32_t tmp3;
        tmp3 = RCC->PLLCFGR & (uint32_t)(~((uint32_t) RCC_PLLCFGR_PLLN)); // Create mask to set PLLN (~0b111111111000000) 
        RCC->PLLCFGR = (tmp3 | (((uint32_t) _PLLN) << 6)); // PLLN set (bit 6:14)  

	uint32_t tmp4;
        tmp4 = RCC->PLLCFGR & (uint32_t)(~((uint32_t) RCC_PLLCFGR_PLLP)); // Create mask to set PLLP (~0b110000000000000000) 
        RCC->PLLCFGR = (tmp4 | (((uint32_t) _PLLP) << 16)); // PLLP set (bit 16:17)

	PLL_Enable();	
	
	/**Initializes the CPU, AHB and APB busses clocks */
	RCC_ClkInitStruct.ClockType = 0xF;
	RCC_ClkInitStruct.SYSCLKSource = RCC_CFGR_SW_PLL;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_CFGR_HPRE_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_CFGR_PPRE1_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_CFGR_PPRE1_DIV1;
	
	/* set new latency for flash (I am running faster) */
	FLASH_SET_LATENCY(FLASH_Latency_3);

	/* HCLK: Set the highest APBx dividers in order to ensure that we do not go through
	a non-spec phase whatever we decrease or increase HCLK. */
	if(((RCC_ClkInitStruct.ClockType) & 0x04) == 0x04)
	{
		MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV16);
	}
	if(((RCC_ClkInitStruct.ClockType) & 0x08) == 0x08)
	{
		MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, (RCC_CFGR_PPRE1_DIV16 << 3));
	}
	MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_ClkInitStruct.AHBCLKDivider);	
	
	/* SYSCLK: */
	RCC_SYSCLK_CONFIG(RCC_ClkInitStruct.SYSCLKSource);
	while(RCC_GET_SYSCLK_SOURCE() != RCC_CFGR_SWS_PLL);

	/*PCLK1: */
	MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_ClkInitStruct.APB1CLKDivider);
	MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, ((RCC_ClkInitStruct.APB2CLKDivider) << 3U));

	SystemCoreClockUpdate();
}

void PLL_Disable()
{
	(RCC->CR) &= (uint32_t) (~((uint32_t) RCC_CR_PLLON)); // Disable PLL, RCC_CR bit 24 set to 0	
	while(RCC_GET_FLAG(RCC_FLAG_PLLRDY) != RESET);	
}

void PLL_Enable()
{
	(RCC->CR) |= (uint32_t) ((uint32_t) RCC_CR_PLLON); // Enable PLL, RCC_CR bit 24 set to 1
	while(RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET);	
}

uint32_t getClock(uint32_t _PLLM, uint32_t _PLLN, uint32_t _PLLP)
{
	return (((HSI_CLOCK/_PLLM)*_PLLN)/_PLLP);

}

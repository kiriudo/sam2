
#include "stm32f10x_lib.h"            /* STM32F10x Library Definitions       */

/*----------------------------------------------------------------------------
*        Main: Initialize 
 *---------------------------------------------------------------------------*/
void my_dma(void);
void clock_TP_init(void);
void NVIC_init(void);
void GPIO_TP_init(void);
void printled(void);
void Timer_init(void);
void EXTI_init (void);
u16 data_memory[256];

u16 CCR_Value = 599;
u16 ARR_Value = 999;
u16 PSC_Vlue =  399;
u16 val = 0;

int main (void) {	/* program execution starts here       */
	clock_TP_init();
	GPIO_TP_init();
	NVIC_init();
	Timer_init();
	EXTI_init();
	TIM_SetCompare3(TIM2, 0);
	while(1)
	{
		if(GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_8))
		{
			TIM_SetCompare3(TIM2,val);
		}
	}
}


void clock_TP_init(void){
	ErrorStatus HSEStartUpStatus;
	RCC_HSEConfig(RCC_HSE_ON);
	HSEStartUpStatus = RCC_WaitForHSEStartUp(); /* Wait till HSE is ready */
	if(HSEStartUpStatus == SUCCESS) {
		FLASH_SetLatency(FLASH_Latency_2);
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
		RCC_PLLCmd(ENABLE);
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div2);
		RCC_PCLK2Config(RCC_HCLK_Div1);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_SRAM | RCC_AHBPeriph_FLITF |RCC_AHBPeriph_DMA1, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE );
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM2, ENABLE);
	}
}

void Timer_init(void) {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	u16 TIM_period = 999;
	u16 TIM_prescaler = 7199;
	
	TIM_CtrlPWMOutputs(TIM2,ENABLE);
	
	/* Time base configuration */
	TIM_InternalClockConfig(TIM2);
	TIM_TimeBaseStructure.TIM_Period = TIM_period;
	TIM_TimeBaseStructure.TIM_Prescaler = TIM_prescaler; /* Fpwm = ????? */
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	//TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_UpdateDisableConfig(TIM2, DISABLE);
	TIM_UpdateRequestConfig ( TIM2, TIM_UpdateSource_Regular);
	TIM_Cmd ( TIM2, ENABLE );
	
	TIM_InternalClockConfig(TIM3);
	TIM_TimeBaseStructure.TIM_Period = TIM_period;
	TIM_TimeBaseStructure.TIM_Prescaler = TIM_prescaler; /* Fpwm = ????? */
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_Cmd ( TIM3, ENABLE ); 
}

void TIM3_IRQHandler(void){
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	GPIO_WriteBit(GPIOB, GPIO_Pin_8, (BitAction)(1^GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_8)));
}

void EXTI_init (void) {
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13); /* Selects PC.13 as EXTI */
	EXTI_InitStructure.EXTI_Line = EXTI_Line13; /* Line 13 */
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 
}

void NVIC_init(void) {
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); /* Configure 2 bits for preemption priority */
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQChannel | TIM2_IRQChannel | EXTI15_10_IRQChannel; /* Enable the timer Interrupt */
	
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure); 
}

void GPIO_TP_init(void){
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	  
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
}


void DMAChannel1_IRQHandler(void)
{
	DMA_ClearITPendingBit(DMA1_IT_TC1);
	printled();
}

void printled(void){
	int i = 0;
	for(i=0;i<256;i++)
	{
		val+=data_memory[i];
	}
	val = val/256;
}


void EXTI15_10_IRQHandler(void) {
	TIM_SetCompare3(TIM2,val);
  EXTI_ClearITPendingBit(EXTI_Line13); // Acquitement
}





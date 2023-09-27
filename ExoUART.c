
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
u8 mess = 'E';
u16 CCR_Value = 599;
u16 ARR_Value = 999;
u16 PSC_Vlue =  399;
u16 val = 0;
u8 rmess;

int main (void) {	/* program execution starts here*/
	clock_TP_init();
	GPIO_TP_init();
	NVIC_init();
	Timer_init();
	EXTI_init();
	while(1);
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
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC | RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE );
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM2, ENABLE);
	}
}

void USART1_IRQHandler(void){
		USART_SendData(USART1, mess);
}

void EXTI_init (void) {
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13); /* Selects PC.13 as EXTI */
	EXTI_InitStructure.EXTI_Line = EXTI_Line13;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 
}

void USART_init() {
USART_InitTypeDef  USART_InitStructure;
USART_InitStructure.USART_BaudRate = 9600;
USART_InitStructure.USART_WordLength = USART_WordLength_8b;
USART_InitStructure.USART_StopBits = USART_StopBits_2;
USART_InitStructure.USART_Parity = USART_Parity_Odd;
USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
USART_Init(USART1, &USART_InitStructure); 		/* Configure the USART1 */

USART_ITConfig(USART1, USART_IT_TXE, ENABLE);  /* Enable the USART Transmit interrupt */  
USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); /* Enable the USART Receive interrupt */

USART_Cmd(USART1, ENABLE); 			/* Enable USART1 */
}

void NVIC_init(void) {
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); /* Configure 2 bits for preemption priority */
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQChannel | TIM2_IRQChannel | EXTI15_10_IRQChannel | USART1_IRQChannel; /* Enable the timer Interrupt */
	
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure); 
}

void GPIO_TP_init(void){
		GPIO_InitTypeDef GPIO_InitStructure;
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	  
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	
}


void DMAChannel1_IRQHandler(void)
{
	DMA_ClearITPendingBit(DMA1_IT_TC1);
	printled();
}





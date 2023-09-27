
#include "stm32f10x_lib.h"            /* STM32F10x Library Definitions       */

/*----------------------------------------------------------------------------
*        Main: Initialize 
 *---------------------------------------------------------------------------*/
void my_dma(void);
void clock_TP_init(void);
void NVIC_init(void);
void GPIO_TP_init(void);
void printled(void);
void initADC();
u16 data_memory[256];

int main (void) {	/* program execution starts here       */
	clock_TP_init();
	NVIC_init();
	initADC();
	my_dma();
	while(1);
}

void printled(void){
	int i = 0;
	u16 val = 0;
	for(i=0;i<256;i++)
	{
		val+=data_memory[i];
	}
	val = val/256;
}

void my_dma(void)
{
	DMA_InitTypeDef DMA_InitStruct;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;
	DMA_InitStruct.DMA_MemoryBaseAddr = (u32)&data_memory;
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStruct.DMA_BufferSize = 256;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
	DMA_Init( DMA1_Channel1, &DMA_InitStruct );
	DMA_Cmd(DMA1_Channel1, ENABLE );
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC|DMA_IT_HT, ENABLE);
}

void clock_TP_init(void)
{
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
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_SRAM|RCC_AHBPeriph_FLITF |RCC_AHBPeriph_DMA1, ENABLE);
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOB, ENABLE );		
	}
}

void NVIC_init(void) {
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); /* Configure 2 bits for preemption priority */
		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQChannel; /* Enable the EXTI1 Interrupt */
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure); 
}

void GPIO_TP_init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
}


void initADC(void){
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None ;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Left;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init( ADC1, &ADC_InitStructure);
	ADC_RegularChannelConfig( ADC1, ADC_Channel_1, 1, ADC_SampleTime_1Cycles5);
	ADC_DMACmd(ADC1, ENABLE); // ? chaque fin de conversion, demande de transfert DMA.
	ADC_Cmd(ADC1, ENABLE );
	ADC_ResetCalibration(ADC1); /* Enable ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1)); /* Check the end of ADC1 reset calibration register */
	ADC_StartCalibration(ADC1); /* Start ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1)); /* Check the end of ADC1 calibration */
	ADC_SoftwareStartConvCmd( ADC1, ENABLE ); 
}



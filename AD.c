#include "stm32f10x.h"                  // Device header

uint16_t AD_Value[4];

void AD_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);                                           //配置ADC分频器6分频12MHz，选择2;4;6;8分频
	
	GPIO_InitTypeDef GPIO_InitStructure;                                        //配置GPIO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;                               //模拟输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_55Cycles5);
	//指定ADC、指定通道、指定规则组序列器中的次序1~16、指定通道采样时间(ADCCLK周期数) 

	ADC_InitTypeDef ADC_InitStructure;                                          //配置ADC
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                          //ADC工作模式，独立模式
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                      //数据对齐，指定右对齐
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;         //外部触发转换选择，不使用外部触发(软件触发使用)
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                         //是否开启连续转换
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;                               //扫描模式
	ADC_InitStructure.ADC_NbrOfChannel = 4;                                     //指定规则组转换列表中通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);
//中断和模拟看门狗
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;				//外设站点的起始地址、数据宽度、是否自增
	DMA_InitStructure.DMA_PeripheralDataSize =DMA_PeripheralDataSize_HalfWord;	//字节uint16_t
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//外设地址不自增，始终转运同一个地址的数据
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Value;					//存储器站点的起始地址、数据宽度、是否自增
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;							//传送方向，指定外设时源端(还是目的地)
	DMA_InitStructure.DMA_BufferSize = 4 ;										//缓存区大小(传输计数器)
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;								//传输模式，自动重装(循环模式),自动重装和软件出发不能同时使用
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;								//不使用软件触发，ADC硬件触发
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;						//优先级
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	DMA_Cmd(DMA1_Channel1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);													//开启ADC到DMA通路
	ADC_Cmd(ADC1, ENABLE);                                                      //给ADC上电
	
	ADC_ResetCalibration(ADC1);													//复位校准
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);							//等待复位校准完成
	ADC_StartCalibration(ADC1);													//启动校准
	while (ADC_GetCalibrationStatus(ADC1) == SET);								//等待校准完成
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);										//连续转换初始化时直接运行
}

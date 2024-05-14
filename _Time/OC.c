#include "stm32f10x.h"                  // Device header

void PWM_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);						//引脚重映射，开启AFIO时钟
//	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);						//部分重映射模式1
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);					//解除PA15原来映射的SWJ调试功能

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;								//复用推挽输出，断开输出数据寄存器，连接片上外设
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;		//GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	TIM_InternalClockConfig(TIM2);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;			//ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;		//PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);										//给结构体赋予初始值，防止定时器更改出错
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;							//选择输出比较模式PWM1
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;					//选定输出比较极性，电平不翻转，有效电平为高电平
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;				//输出状态，使能
	TIM_OCInitStructure.TIM_Pulse = 0;						//CCR
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	/*多通道PWM使用*/
	// TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	// TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	// TIM_OC4Init(TIM2, &TIM_OCInitStructure);


	// TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
	// TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	// TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
	// TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);

	TIM_Cmd(TIM2, ENABLE);
}

void PWM_SetCompare1(uint16_t Compare)
{
	TIM_SetCompare1(TIM2, Compare);												//单独更改通道一CCR的值
}


void PWM_SetPrescaler(uint16_t Prescaler)
{
	TIM_PrescalerConfig(TIM2, Prescaler, TIM_PSCReloadMode_Immediate);			//修改特定定时器预分频器的值以及重装模式，此立即重装
}


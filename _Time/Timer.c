#include "stm32f10x.h"                  // Device header

void Timer_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);                             //通用定时器在APB1外设
//	TIM_InternalClockConfig(TIM2);                                                  //为TIM2选择内部时钟，可不写，定时器上电默认内部时钟
    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPU;									//上拉输入，微弱信号浮空输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	TIM_ETRClockMode2Config(TIM2,TIM_ExtTRGPSC_OFF,TIM_ExtTRGPolarity_NonInverted,0x00);
	//选择外部时钟源模式2；在此之前应先配置GPIO。定时器，外部预分频值，极性(上升、下降沿)，滤波预分频值
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;                              //定时器初始化结构体
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;                       //滤波器分频一分频，多次采样相同电平则输入
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;                   //上升计数
	TIM_TimeBaseInitStructure.TIM_Period=10000-1;                                   //周期，ARR自动重装器的值0~65535,下同
	TIM_TimeBaseInitStructure.TIM_Prescaler=7200-1;                                 //PSC预分频器，72MHz，合计定时一秒
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;                              //重复计数器，高级计数器才有
    //无CNT计数器的值，参考其他函数设置TIM_SetCounter/TIM_GetCounter
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);                              //初始化。会立即重新装载预分频器和计数器的值，手动产生一个更新事件
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);                                            //将初始化产生的更新中断标志位清除，从0计数
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);                                        //使能TIM2(更新)中断到NVIC

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStructure);
	

	TIM_Cmd(TIM2,ENABLE);                                                           //使能定时器二

}


void TIM2_IRQHandler(void)                                                          //定时器2的中断函数
{
	if (TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{
        //此处执行中断函数
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);                                  //别忘了清除标志位
	}
		
}

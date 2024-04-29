#include "stm32f10x.h"                  // Device header

void COUNTERSENSOR_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);								//AFIO时钟使能，NVIC为内核外设，不需要开启时钟
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource14);                     //EXTI引脚选择
    
	EXTI_InitTypeDef EXTI_InitStructure;                                            //EXTI初始化，
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;                               //中断响应,也可事件响应，引向其他IO
	EXTI_InitStructure.EXTI_Line=EXTI_Line14;
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;							//触发方式下降沿
	EXTI_Init(&EXTI_InitStructure);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);									//配置优先级分组，先占、从占优先级，整个芯片只能执行一次
	NVIC_InitTypeDef NVIC_InitStructure;                                            //NVIC初始化
	NVIC_InitStructure.NVIC_IRQChannel=EXTI15_10_IRQn;                              //选择中断来源9~5 15~10在一个NVIC通道，需要标志位区分
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;							//设置抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;								//设置响应优先级
	NVIC_Init(&NVIC_InitStructure);
	
	
}


void EXTI15_10_IRQHandler(void)														//外部中断函数，不需要声明，自动调用
{
	if (EXTI_GetITStatus(EXTI_Line14)==SET)											//确认标志位
	{
		//此处执行中断程序
		EXTI_ClearITPendingBit(EXTI_Line14);										//清除标志位，每次执行完中断程序后都清除
	}
	
}




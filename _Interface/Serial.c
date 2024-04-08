#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

#define	GPIO	GPIOA
#define TX		GPIO_Pin_9
#define RX		GPIO_Pin_10

uint8_t Serial_RxData;
uint8_t Serial_RxFlag;

void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);                              //开启时钟，USART1在APB2外设，其他(2/3)在APB1外设
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                                     //选择复用推挽输出
	GPIO_InitStructure.GPIO_Pin = TX;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = RX;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO, &GPIO_InitStructure);

	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;                                          //波特率,常用9600/115200
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;     //硬件流控制(设备间协调传输速度使用)。不使用流控
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;                     //串口模式。Tx|Rx
	USART_InitStructure.USART_Parity = USART_Parity_No;                                 //校验位;奇校验(校验位补1至奇数个)或者偶校验。此处无校验
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                              //停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                         //字长,不校验选择8位
	USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);

	USART_Cmd(USART1, ENABLE);
}


void Serial_SendByte(uint8_t Byte)                                      //发送函数
{
	USART_SendData(USART1, Byte);                                       //发送byte,低位先行
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);       //获取发送寄存器空标志位状态，等待发送完成
}

void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}
}

void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

int fputc(int ch, FILE *f)                                      //printf重新映射到串口
{
	Serial_SendByte(ch);
	return ch;
}

void Serial_Printf(char *format, ...)                                      //printf重新映射到串口
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}

uint8_t Serial_GetRxFlag(void)
{
	if (Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}

uint8_t Serial_GetRxData(void)
{
	return Serial_RxData;
}

/*
uint8_t RxData;
while (1)                                           //放到主函数main中,通过不断查询Rx标志位来得知是否接收到数据
	{
		if (Serial_GetRxFlag() == 1)
		{
			RxData = Serial_GetRxData();            //转存获取到的数据
			Serial_SendByte(RxData);

		}
	}
*/

void USART1_IRQHandler(void)                                    //中断函数
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		Serial_RxData = USART_ReceiveData(USART1);
		Serial_RxFlag = 1;
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);         //清除标志位
	}
}

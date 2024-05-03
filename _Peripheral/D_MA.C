#include "stm32f10x.h"                  // Device header

uint16_t D_MA_Size;

void D_MA_Init(uint32_t AddrA, uint32_t AddrB, uint16_t Size)
{
	D_MA_Size = Size;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);							//AHB总线开启时钟
	
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = AddrA;							//外设站点的起始地址、数据宽度、是否自增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		//字节uint8_t
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
	DMA_InitStructure.DMA_MemoryBaseAddr = AddrB;								//存储器站点的起始地址、数据宽度、是否自增
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;							//传送方向，指定外设时源端(还是目的地)
	DMA_InitStructure.DMA_BufferSize = Size;									//缓存区大小(传输计数器)
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;								//传输模式，是否自动重装,自动重装和软件出发不能同时使用
	DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;									//选择是否存储器到存储器，既软件触发
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;						//优先级
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	DMA_Cmd(DMA1_Channel1, DISABLE);
}

void D_MA_Transfer(void)
{
	DMA_Cmd(DMA1_Channel1, DISABLE);											//赋值前先给DMA失能
	DMA_SetCurrDataCounter(DMA1_Channel1, D_MA_Size);							//给传输计数器赋值
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	while (DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);							//获取转运标志位，共四种，此处为转运完成标志位
	DMA_ClearFlag(DMA1_FLAG_TC1);
}

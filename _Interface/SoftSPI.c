#include "stm32f10x.h"                  // Device header

#define	SPI_Port	GPIOA
#define	RCC_APB		RCC_APB2Periph_GPIOA
#define SCK			GPIO_Pin_5	//Serial Clock
#define MOSI		GPIO_Pin_7	//Master Output Slave Input
#define MISO		GPIO_Pin_6	//Master Input Slave Output
#define SS			GPIO_Pin_4	//Slave Select

void _SPI_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                    	//MOSI SCK SS为推挽输出
	GPIO_InitStructure.GPIO_Pin = SCK | MOSI | SS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;                       	//MISO为上拉输入
	GPIO_InitStructure.GPIO_Pin = MISO;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_Port, &GPIO_InitStructure);

	GPIO_WriteBit(SPI_Port, SS, (BitAction)1);								//从机选择置高
	GPIO_WriteBit(SPI_Port, SCK, (BitAction)0);								//模式0.CPOL=0:空闲状态时,SCK为低电平.
}

void _SPI_Start(void)
{
	GPIO_WriteBit(SPI_Port, SS, (BitAction)0);								//SPI起始将选中的从机SS置低
}

void _SPI_Stop(void)
{
	GPIO_WriteBit(SPI_Port, SS, (BitAction)1);								//SPI结束将选中的从机SS置高
}

uint8_t _SPI_SwapByte(uint8_t ByteSend)										//SPI交换一个字节,高位先行
{
	uint8_t i, ByteReceive = 0x00;

	for (i = 0; i < 8; i ++)
	{
		GPIO_WriteBit(SPI_Port, MOSI,(BitAction)(ByteSend & (0x80 >> i)));				//模式0.SS下降沿和SCK下降沿移出数据
		GPIO_WriteBit(SPI_Port, SCK, (BitAction)1);
		if (GPIO_ReadInputDataBit(SPI_Port, MISO) == 1){ByteReceive |= (0x80 >> i);}	//CPHA=0:SCK第一个边沿移入数据,第二个边沿移出数据
		GPIO_WriteBit(SPI_Port, SCK, (BitAction)0);										//如需模式1，将所有时钟上移一行即可
	}

	return ByteReceive;
}

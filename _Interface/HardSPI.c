#include "stm32f10x.h"                  // Device header

#define	SPI_Port	GPIOB
#define SPIx    	SPI2
#define SCK			GPIO_Pin_13	//Serial Clock
#define MOSI		GPIO_Pin_15	//Master Output Slave Input
#define MISO		GPIO_Pin_14	//Master Input Slave Output
#define SS			GPIO_Pin_12	//Slave Select
/*
#define	SPI_Port	GPIOA
#define SPIx    	SPI1
#define SCK			GPIO_Pin_5	//Serial Clock
#define MOSI		GPIO_Pin_7	//Master Output Slave Input
#define MISO		GPIO_Pin_6	//Master Input Slave Output
#define SS			GPIO_Pin_4	//Slave Select
*/

void _SPI_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);        /*换引脚记得改*/

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;            //通用推挽输出
	GPIO_InitStructure.GPIO_Pin = SS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;             //硬件复用推挽输出
	GPIO_InitStructure.GPIO_Pin = SCK | MOSI;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;               //上拉输入
	GPIO_InitStructure.GPIO_Pin = MISO;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_Port, &GPIO_InitStructure);

	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                           //决定当前设备是主机
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;      //选择双线全双工模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                       //八位数据帧
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                      //高位先行
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;    	//分频系数，越大越慢
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                              //CPOL=0,时钟极性默认低电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                            //CPHA=0,第一个边沿开始采样。选择SPI模式0
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                               //软件NSS模式，SS引脚通过软件模拟
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPIx, &SPI_InitStructure);

	SPI_Cmd(SPIx, ENABLE);

	GPIO_WriteBit(SPI_Port, SS, (BitAction)1);
}

void _SPI_Start(void)
{
	GPIO_WriteBit(SPI_Port, SS, (BitAction)0);
}

void _SPI_Stop(void)
{
	GPIO_WriteBit(SPI_Port, SS, (BitAction)1);
}

uint8_t _SPI_SwapByte(uint8_t ByteSend)
{
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) != SET);       //获取TXE标志位,等待TXE(发送寄存器空标志位)为1

	SPI_I2S_SendData(SPIx, ByteSend);                                   //写DR寄存器

	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) != SET);      //获取RXNE标志位,等待RXNE(接收寄存器非空标志位)为1

	return SPI_I2S_ReceiveData(SPIx);                                   //读DR寄存器
}

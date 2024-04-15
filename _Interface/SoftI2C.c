#include "stm32f10x.h"                  // Device header
#include "Delay.h"

#define	GPIO	GPIOB					//引脚定义
#define	RCC_APB	RCC_APB2Periph_GPIOB
#define SCL		GPIO_Pin_10
#define SDA		GPIO_Pin_11

void _I2C_Init(void)												//初始化软件I2C
{
	RCC_APB2PeriphClockCmd(RCC_APB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;				//开漏输出,开漏输出仍然可输入，直接读取寄存器即可
	GPIO_InitStructure.GPIO_Pin = SCL | SDA;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO, &GPIO_InitStructure);

	GPIO_SetBits(GPIO, SCL | SDA);					//统一置为高电平
}

void SoftI2C_W_SCL(uint8_t BitValue)
{
	GPIO_WriteBit(GPIO, SCL, (BitAction)BitValue);
	Delay_us(10);		//I2C为开漏输出，电平变化慢。延时待信号稳定
}

void SoftI2C_W_SDA(uint8_t BitValue)
{
	GPIO_WriteBit(GPIO, SDA, (BitAction)BitValue);
	Delay_us(10);
}

uint8_t SoftI2C_R_SDA(void)
{
	uint8_t BitValue;
	BitValue = GPIO_ReadInputDataBit(GPIO, SDA);
	Delay_us(10);
	return BitValue;
}

void SoftI2C_Start(void)//SCL高电平期间，SDA从高电平切换到低电平
{
	SoftI2C_W_SDA(1);
	SoftI2C_W_SCL(1);
	SoftI2C_W_SDA(0);
	SoftI2C_W_SCL(0);
}

void SoftI2C_Stop(void)//SCL高电平期间，SDA从低电平切换到高电平
{
	SoftI2C_W_SDA(0);
	SoftI2C_W_SCL(1);
	SoftI2C_W_SDA(1);
}

void SoftI2C_SendByte(uint8_t Byte)//发送字节数据,高位先行
{
	uint8_t i;
	for (i = 0; i < 8; i ++)
	{
		SoftI2C_W_SDA(Byte & (0x80 >> i));
		SoftI2C_W_SCL(1);
		SoftI2C_W_SCL(0);
	}
}

uint8_t SoftI2C_ReceiveByte(void)//接受字节数据
{
	uint8_t i, Byte = 0x00;
	SoftI2C_W_SDA(1);
	for (i = 0; i < 8; i ++)
	{
		SoftI2C_W_SCL(1);
		if (SoftI2C_R_SDA() == 1){Byte |= (0x80 >> i);}
		SoftI2C_W_SCL(0);
	}
	return Byte;
}

void SoftI2C_SendAck(uint8_t AckBit)							//发送应答
{
	SoftI2C_W_SDA(AckBit);
	SoftI2C_W_SCL(1);
	SoftI2C_W_SCL(0);
}

uint8_t SoftI2C_ReceiveAck(void)								//接收应答
{
	uint8_t AckBit;
	SoftI2C_W_SDA(1);
	SoftI2C_W_SCL(1);
	AckBit = SoftI2C_R_SDA();
	SoftI2C_W_SCL(0);
	return AckBit;
}

void _I2C_WriteReg(uint8_t HardAddress,uint8_t RegAddress, uint8_t Data)//指定地址写
{
	SoftI2C_Start();
	SoftI2C_SendByte(HardAddress);
	SoftI2C_ReceiveAck();
	SoftI2C_SendByte(RegAddress);
	SoftI2C_ReceiveAck();
	SoftI2C_SendByte(Data);
	SoftI2C_ReceiveAck();
	SoftI2C_Stop();
}

uint8_t _I2C_ReadReg(uint8_t HardAddress,uint8_t RegAddress)			//指定地址读
{
	uint8_t Data;

	SoftI2C_Start();
	SoftI2C_SendByte(HardAddress);
	SoftI2C_ReceiveAck();
	SoftI2C_SendByte(RegAddress);
	SoftI2C_ReceiveAck();

	SoftI2C_Start();										//重复起始
	SoftI2C_SendByte(HardAddress | 0x01);					//改变最后一位，读取
	SoftI2C_ReceiveAck();
	Data = SoftI2C_ReceiveByte();
	SoftI2C_SendAck(1);										//发送应答1，停止读取，发送应答0，继续读取
	SoftI2C_Stop();

	return Data;
}

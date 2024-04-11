#include "stm32f10x.h"                  // Device header
#include "Delay.h"

#define	GPIO	GPIOB					//引脚定义
#define	I2Cx	I2C2
#define SCL		GPIO_Pin_10
#define SDA		GPIO_Pin_11


void _I2C_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);                        //I2C时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);                       //引脚时钟

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;                             //复用(外设操作管脚而不是CPU)开漏模式
	GPIO_InitStructure.GPIO_Pin = SCL | SDA;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;                                  //I2C模式
	I2C_InitStructure.I2C_ClockSpeed = 100000;                                  //SCL时钟频率,400K以下
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;                          //时钟占空比(低:高)，100KHz以下标准频率下，固定1:1,100k~400kHz可多低电平，方便数据翻转
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;                                 //应答位配置,确定接收一个字节后是否给从机应答
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;   //指定stm32作为从机时可以响应地址位数
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;                                   //指定stm32作为从机地址
	I2C_Init(I2Cx, &I2C_InitStructure);
	I2C_Cmd(I2Cx, ENABLE);
}

void _I2C_WaitEvent(I2C_TypeDef* IICX, uint32_t I2C_EVENT)
{
	uint32_t Timeout;
	Timeout = 10000;
	while (I2C_CheckEvent(IICX, I2C_EVENT) != SUCCESS)
	{
		Timeout --;
		if (Timeout == 0)
		{
			break;
		}
	}
}

void _I2C_WriteReg(uint8_t HardAddress,uint8_t RegAddress, uint8_t Data)
{
//  while( I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) );                            //等待总线空闲
	I2C_GenerateSTART(I2Cx, ENABLE);                                            //生成起始条件
	_I2C_WaitEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT);                         //检查EV5事件(主机模式选择)

	I2C_Send7bitAddress(I2Cx, HardAddress, I2C_Direction_Transmitter);          //发送从机地址、写入指令
	_I2C_WaitEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);           //检查EV6事件(发送模式已选择)

	I2C_SendData(I2Cx, RegAddress);                                             //发送数据(寄存器地址)
	_I2C_WaitEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING);                   //检查EV8事件(字节正在发送(数据已经转入移位寄存器))

	I2C_SendData(I2Cx, Data);
	_I2C_WaitEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED);                    //检查EV8_2事件(字节发送完成)

	I2C_GenerateSTOP(I2Cx, ENABLE);                                             //产生结束指令
}

uint8_t _I2C_ReadReg(uint8_t HardAddress,uint8_t RegAddress)
{
	uint8_t Data;
//  while( I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) );                            //等待总线空闲
	I2C_GenerateSTART(I2Cx, ENABLE);                                            //切换地址
	_I2C_WaitEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT);
	I2C_Send7bitAddress(I2Cx, HardAddress, I2C_Direction_Transmitter);
	_I2C_WaitEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	I2C_SendData(I2Cx, RegAddress);
	_I2C_WaitEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED);                    //检查EV8_2事件，等待地址发送完成


	I2C_GenerateSTART(I2Cx, ENABLE);                                            //重新产生起始条件
	_I2C_WaitEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT);

	I2C_Send7bitAddress(I2Cx, HardAddress, I2C_Direction_Receiver);             //发送从机地址并改为读取模式
	_I2C_WaitEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);

	I2C_AcknowledgeConfig(I2Cx, DISABLE);                                       //停止应答，结束读取
	I2C_GenerateSTOP(I2Cx, ENABLE);                                             //数据接收完成之前，提前产生结束指令

	_I2C_WaitEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED);                       //等待数据接收完成(接收多字节数据时，循环执行后两行，待最后一个字节执行前两行)
	Data = I2C_ReceiveData(I2Cx);
	I2C_AcknowledgeConfig(I2Cx, ENABLE);                                        //ACK置一

	return Data;
}

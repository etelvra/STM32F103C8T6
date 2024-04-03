#include "stm32f10x.h"                  // Device header
#include "NRF24L01_Ins.h"
#include "Delay.h"
#include "math.h"
#include "HardSPI.h"

/**********  NRF24L01  ***********/
#define IRQ_Port	GPIOA
#define CE_Port		GPIOA
#define IRQ_Pin		GPIO_Pin_9
#define CE			GPIO_Pin_8
/*
#define IRQ_Port	GPIOB
#define CE_Port		GPIOB
#define IRQ_Pin		GPIO_Pin_1
#define CE			GPIO_Pin_0
*/
#define TX_ADR_WIDTH    5
#define RX_ADR_WIDTH    5
#define TX_PLOAD_WIDTH  32
#define RX_PLOAD_WIDTH  32

const uint8_t TX_ADDRESS[TX_ADR_WIDTH]={0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const uint8_t RX_ADDRESS[RX_ADR_WIDTH]={0xFF, 0xFF, 0xFF, 0xFF, 0xFF};


uint8_t NRF24L01_Write_Reg(uint8_t Reg, uint8_t Value)
{
	uint8_t Status;

	_SPI_Start();
  	Status = _SPI_SwapByte(Reg);
	_SPI_SwapByte(Value);
	_SPI_Stop();

	return Status;
}

uint8_t NRF24L01_Read_Reg(uint8_t Reg)
{
 	uint8_t Value;

	_SPI_Start();
  	_SPI_SwapByte(Reg);
	Value = _SPI_SwapByte(NOP);
	_SPI_Stop();

	return Value;
}

uint8_t NRF24L01_Write_Buf(uint8_t Reg, uint8_t *Buf, uint8_t Len)
{
	uint8_t Status, i;
	_SPI_Start();
	Status = _SPI_SwapByte(Reg);
	for(i = 0; i < Len; i ++)
	{
		_SPI_SwapByte(*Buf ++);
	}
	_SPI_Stop();
	return Status;
}

uint8_t NRF24L01_Read_Buf(uint8_t Reg, uint8_t *Buf, uint8_t Len)
{
	uint8_t Status, i;
	_SPI_Start();
	Status =_SPI_SwapByte(Reg);
 	for(i = 0; i < Len; i ++)
	{
		Buf[i] = _SPI_SwapByte(NOP);
	}
	_SPI_Stop();
	return Status;
}

uint8_t NRF24L01_Check(void)//检测24L01是否存在
{
	uint8_t check_in_buf[5] = {0x11 ,0x22, 0x33, 0x44, 0x55};
	uint8_t check_out_buf[5] = {0x00};

	NRF24L01_Write_Buf(nRF_WRITE_REG + TX_ADDR, check_in_buf, 5);
	NRF24L01_Read_Buf(nRF_READ_REG + TX_ADDR, check_out_buf, 5);
	if((check_out_buf[0] == 0x11) && (check_out_buf[1] == 0x22) && (check_out_buf[2] == 0x33) && (check_out_buf[3] == 0x44) && (check_out_buf[4] == 0x55))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void NRF24L01_RT_Init(void)
{
  	NRF24L01_Write_Reg(nRF_WRITE_REG+RX_PW_P0, RX_PLOAD_WIDTH);
	NRF24L01_Write_Reg(FLUSH_RX, NOP);
  	NRF24L01_Write_Buf(nRF_WRITE_REG + TX_ADDR, (uint8_t*)TX_ADDRESS, TX_ADR_WIDTH);//写TX节点地址
  	NRF24L01_Write_Buf(nRF_WRITE_REG + RX_ADDR_P0, (uint8_t*)RX_ADDRESS, RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK
	NRF24L01_Write_Reg(nRF_WRITE_REG + CONFIG, 0x0F);		//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式
  	NRF24L01_Write_Reg(nRF_WRITE_REG + EN_AA, 0x01);		//使能通道0的自动应答
  	NRF24L01_Write_Reg(nRF_WRITE_REG + EN_RXADDR, 0x01);	//使能通道0的接收地址
  	NRF24L01_Write_Reg(nRF_WRITE_REG + SETUP_RETR, 0x1A);	//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
  	NRF24L01_Write_Reg(nRF_WRITE_REG + RF_CH, 100);
  	NRF24L01_Write_Reg(nRF_WRITE_REG + RF_SETUP, 0x0F);  	//设置TX发射参数,0db增益,2Mbps,低噪声增益开启
}

void NRF24L01_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);        /*换引脚记得改*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = CE;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CE_Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IRQ_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IRQ_Port, &GPIO_InitStructure);
	_SPI_Init();
	GPIO_WriteBit(CE_Port, CE, (BitAction)0);
	while(NRF24L01_Check());
	NRF24L01_RT_Init();
	GPIO_WriteBit(CE_Port, CE, (BitAction)1);
}

void NRF24L01_Tx(uint8_t *Buf)
{
	uint8_t State;
	GPIO_WriteBit(CE_Port, CE, (BitAction)0);
	NRF24L01_Write_Reg(nRF_WRITE_REG + CONFIG, 0x0E);			//转为发射模式
  	NRF24L01_Write_Buf(WR_TX_PLOAD, Buf, TX_PLOAD_WIDTH);		//写入发送数据
 	GPIO_WriteBit(CE_Port, CE, (BitAction)1);

	while(GPIO_ReadInputDataBit(IRQ_Port, IRQ_Pin) == 1);
	State = NRF24L01_Read_Reg(STATUS);
	NRF24L01_Write_Reg(nRF_WRITE_REG + STATUS, State);
	if(State&MAX_RT)
	{
		NRF24L01_Write_Reg(FLUSH_TX, NOP);
	}

	GPIO_WriteBit(CE_Port, CE, (BitAction)0);
	NRF24L01_Write_Reg(nRF_WRITE_REG + CONFIG, 0x0F);			//换回接收模式
	GPIO_WriteBit(CE_Port, CE, (BitAction)1);
}

uint8_t NRF24L01_Get_Flag(void)
{
	return (!GPIO_ReadInputDataBit(IRQ_Port, IRQ_Pin));
}

uint8_t NRF24L01_Rx(uint8_t *Buf)
{
	uint8_t State;

	State = NRF24L01_Read_Reg(STATUS);
	NRF24L01_Write_Reg(nRF_WRITE_REG + STATUS, State);
	if(State & RX_OK)
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD, Buf, RX_PLOAD_WIDTH);
		NRF24L01_Write_Reg(FLUSH_RX, NOP);
		Delay_us(150);
		return 0;
	}
	return 1;
}

void NRF24L01_SendNum(uint32_t Num)
{
	uint8_t SendBuf[10] = {0};
	for (uint8_t i = 0; i < 10; i ++)
	{
		SendBuf[i] = Num / (uint32_t)pow(10, 9 - i) % 10;
	}

	NRF24L01_Tx(SendBuf);
}


uint32_t NRF24L01_GetNum(void)
{
	uint32_t Num = 0;
	uint8_t Buf[10] = {0};
	NRF24L01_Rx(Buf);
	for (uint8_t i = 0; i < 10; i ++)
	{
		Num += Buf[i] * pow(10, 9 - i);
	}

	return Num;
}


void NRF24L01_SendString(char *String)
{
	uint8_t Psign[32] = {0};
	uint8_t SendBuf[33] = {0};
	uint8_t TSign = 0;
	for (uint8_t i = 0; i < 32; i ++)
	{
		if (String[i] != 0)
		{
			Psign[TSign] = i;
			TSign ++;
		}
	}
	if (TSign > 0)
	{
		TSign --;
	}
	SendBuf[0] = Psign[TSign] + 1;
	for (uint8_t i = 0; i < SendBuf[0]; i ++)
	{
		SendBuf[i + 1] = String[i];
	}
	NRF24L01_Tx(SendBuf);
}

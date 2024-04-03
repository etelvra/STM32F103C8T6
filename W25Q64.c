#include "stm32f10x.h"                  // Device header
#include "W25Q64_Ins.h"
#include "HardSPI.h"

void W25Q64_Init(void)
{
	_SPI_Init();
}

void W25Q64_ReadID(uint8_t *MID, uint16_t *DID)
{
	_SPI_Start();
	_SPI_SwapByte(W25Q64_JEDEC_ID);
	*MID = _SPI_SwapByte(W25Q64_DUMMY_BYTE);
	*DID = _SPI_SwapByte(W25Q64_DUMMY_BYTE);
	*DID <<= 8;
	*DID |= _SPI_SwapByte(W25Q64_DUMMY_BYTE);
	_SPI_Stop();
}

void W25Q64_WriteEnable(void)											//使能写函数
{
	_SPI_Start();
	_SPI_SwapByte(W25Q64_WRITE_ENABLE);
	_SPI_Stop();
}

void W25Q64_WaitBusy(void)												//等待忙函数，超时退出
{
	uint32_t Timeout;
	_SPI_Start();
	_SPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1);
	Timeout = 100000;
	while ((_SPI_SwapByte(W25Q64_DUMMY_BYTE) & 0x01) == 0x01)
	{
		Timeout --;
		if (Timeout == 0)
		{
			break;
		}
	}
	_SPI_Stop();
}

void W25Q64_PageProgram(uint32_t Address, uint8_t *DataArray, uint16_t Count)	//数据发送函数写页，数据Count不能超过256，否则从头覆盖
{
	uint16_t i;
	
	W25Q64_WriteEnable();
	
	_SPI_Start();
	_SPI_SwapByte(W25Q64_PAGE_PROGRAM);
	_SPI_SwapByte(Address >> 16);				//地址为三个字节，先发送最高位
	_SPI_SwapByte(Address >> 8);
	_SPI_SwapByte(Address);
	for (i = 0; i < Count; i ++)
	{
		_SPI_SwapByte(DataArray[i]);
	}
	_SPI_Stop();
	
	W25Q64_WaitBusy();
}

void W25Q64_SectorErase(uint32_t Address)			//扇区擦除
{
	W25Q64_WriteEnable();
	
	_SPI_Start();
	_SPI_SwapByte(W25Q64_SECTOR_ERASE_4KB);
	_SPI_SwapByte(Address >> 16);
	_SPI_SwapByte(Address >> 8);
	_SPI_SwapByte(Address);
	_SPI_Stop();
	
	W25Q64_WaitBusy();
}

void W25Q64_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count)	//接收数据
{
	uint32_t i;
	_SPI_Start();
	_SPI_SwapByte(W25Q64_READ_DATA);
	_SPI_SwapByte(Address >> 16);
	_SPI_SwapByte(Address >> 8);
	_SPI_SwapByte(Address);
	for (i = 0; i < Count; i ++)
	{
		DataArray[i] = _SPI_SwapByte(W25Q64_DUMMY_BYTE);
	}
	_SPI_Stop();
}

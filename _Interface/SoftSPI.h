#ifndef __SOFTSPI_H
#define __SOFTSPI_H

void _SPI_Init(void);
void _SPI_Start(void);
void _SPI_Stop(void);
uint8_t _SPI_SwapByte(uint8_t ByteSend);

#endif

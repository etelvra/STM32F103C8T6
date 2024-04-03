#ifndef __NRF24L01_H
#define __NRF24L01_H

void NRF24L01_Init(void);
uint8_t NRF24L01_Get_Flag(void);

void NRF24L01_SendNum(uint32_t Num);
uint32_t NRF24L01_GetNum(void);
void NRF24L01_SendString(char *String);
#endif

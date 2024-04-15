#ifndef __SOFTI2C_H
#define __SOFTI2C_H

void _I2C_Init(void);
void _I2C_WriteReg(uint8_t HardAddress,uint8_t RegAddress, uint8_t Data);
uint8_t _I2C_ReadReg(uint8_t HardAddress,uint8_t RegAddress);

#endif

/*void SoftI2C_Init(void);
void SoftI2C_Start(void);
void SoftI2C_Stop(void);
void SoftI2C_SendByte(uint8_t Byte);
uint8_t SoftI2C_ReceiveByte(void);
void SoftI2C_SendAck(uint8_t AckBit);
uint8_t SoftI2C_ReceiveAck(void);
*/

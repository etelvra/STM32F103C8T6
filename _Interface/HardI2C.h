#ifndef __HARDI2C_H
#define __HARDI2C_H

void _I2C_Init(void);
void _I2C_WriteReg(uint8_t HardAddress,uint8_t RegAddress, uint8_t Data);
uint8_t _I2C_ReadReg(uint8_t HardAddress,uint8_t RegAddress);

#endif

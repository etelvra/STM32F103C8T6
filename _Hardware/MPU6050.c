#include "stm32f10x.h"                  // Device header
#include "HardI2C.h"
#include "MPU6050_Reg.h"


void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)			//指定地址写
{
	_I2C_WriteReg(MPU6050_ADDRESS,RegAddress,Data);
}

uint8_t MPU6050_ReadReg(uint8_t RegAddress)					//指定地址读
{
	return _I2C_ReadReg(MPU6050_ADDRESS,RegAddress);
}

void MPU6050_Init(void)
{
	_I2C_Init();
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);				//配置电源管理寄存器1,解除睡眠，选择陀螺仪时钟，使能0失能
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);				//配置电源管理寄存器2
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);				//采样率分频，十分频
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);
}

uint8_t MPU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}

void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ,
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
	uint8_t DataH, DataL;

	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
	*AccX = (DataH << 8) | DataL;

	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
	*AccY = (DataH << 8) | DataL;

	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
	*AccZ = (DataH << 8) | DataL;

	DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
	*GyroX = (DataH << 8) | DataL;

	DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
	*GyroY = (DataH << 8) | DataL;

	DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
	*GyroZ = (DataH << 8) | DataL;
}

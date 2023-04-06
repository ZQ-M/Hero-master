#ifndef __IMU_MPU_6050_H_
#define __IMU_MPU_6050_H_

#include "spi5.h"
#include "mpu6500_reg.h"
#include "stdio.h"
#include "cmsis_os.h"
#include "usart3.h"
#include "string.h"

typedef struct
{
	int16_t ax;
	int16_t ay;
	int16_t az;

	int16_t mx;
	int16_t my;
	int16_t mz;

	int16_t temp;

	int16_t gx;
	int16_t gy;
	int16_t gz;
	
	int16_t ax_offset;
	int16_t ay_offset;
	int16_t az_offset;

	int16_t gx_offset;
	int16_t gy_offset;
	int16_t gz_offset;
} Mpu_Data_t;

typedef struct
{
	int16_t ax;
	int16_t ay;
	int16_t az;

	int16_t mx;
	int16_t my;
	int16_t mz;

	float temp;

	float wx; /*!< omiga, +- 2000dps => +-32768  so gx/16.384/57.3 =	rad/s */
	float wy;
	float wz;

	float vx;
	float vy;
	float vz;

	float rol;
	float pit;
	float yaw;
} Imu_t;

#define BOARD_DOWN (1)  

uint8_t Mpu_Set_Gyro_FSR(uint8_t fsr);
uint8_t Mpu_Set_Accel_FSR(uint8_t fsr);

void Mpu6050_Get_Data(void);
void Mpu_Offset_Call(void);
uint8_t Mpu_Device_Init(void);

//外部调取IMU数据
Imu_t *Get_Imu_Date_Now(void);

#endif //__IMU_MPU_6050_H_

#include "imu_mpu6050.h"

#define MPU_DELAY(ms) osDelay(ms);

uint8_t mpu_buff[14];
Mpu_Data_t mpu_data;
Imu_t imu = {0};

void Mpu6050_Get_Data()
{
    SPI5_Read_Bytes(MPU6500_ACCEL_XOUT_H, mpu_buff, 14);

    mpu_data.ax = mpu_buff[0] << 8 | mpu_buff[1];
    mpu_data.ay = mpu_buff[2] << 8 | mpu_buff[3];
    mpu_data.az = mpu_buff[4] << 8 | mpu_buff[5];
    mpu_data.temp = mpu_buff[6] << 8 | mpu_buff[7];

    mpu_data.gx = ((mpu_buff[8] << 8 | mpu_buff[9]) - mpu_data.gx_offset);
    mpu_data.gy = ((mpu_buff[10] << 8 | mpu_buff[11]) - mpu_data.gy_offset);
    mpu_data.gz = ((mpu_buff[12] << 8 | mpu_buff[13]) - mpu_data.gz_offset);

    // ist8310_get_data(ist_buff);
    // memcpy(&mpu_data.mx, ist_buff, 6);

    memcpy(&imu.ax, &mpu_data.ax, 6 * sizeof(int16_t));

    imu.temp = 21 + mpu_data.temp / 333.87f;
    /* 2000dps -> rad/s */
    imu.wx = mpu_data.gx / 16.384f / 57.3f;
    imu.wy = mpu_data.gy / 16.384f / 57.3f;
    imu.wz = mpu_data.gz / 16.384f / 57.3f;
}

uint8_t Mpu_Set_Gyro_FSR(uint8_t fsr)
{
    return SPI5_Write_Byte(MPU6500_GYRO_CONFIG, fsr << 3);
}

uint8_t Mpu_Set_Accel_FSR(uint8_t fsr)
{
    return SPI5_Write_Byte(MPU6500_ACCEL_CONFIG, fsr << 3);
}

void Mpu_Offset_Call(void)
{
    int i;
    for (i = 0; i < 300; i++)
    {
        SPI5_Read_Bytes(MPU6500_ACCEL_XOUT_H, mpu_buff, 14);

        mpu_data.ax_offset += mpu_buff[0] << 8 | mpu_buff[1];
        mpu_data.ay_offset += mpu_buff[2] << 8 | mpu_buff[3];
        mpu_data.az_offset += mpu_buff[4] << 8 | mpu_buff[5];

        mpu_data.gx_offset += mpu_buff[8] << 8 | mpu_buff[9];
        mpu_data.gy_offset += mpu_buff[10] << 8 | mpu_buff[11];
        mpu_data.gz_offset += mpu_buff[12] << 8 | mpu_buff[13];

        MPU_DELAY(5);
    }
    mpu_data.ax_offset = mpu_data.ax_offset / 300;
    mpu_data.ay_offset = mpu_data.ay_offset / 300;
    mpu_data.az_offset = mpu_data.az_offset / 300;
    mpu_data.gx_offset = mpu_data.gx_offset / 300;
    mpu_data.gy_offset = mpu_data.gy_offset / 300;
    mpu_data.gz_offset = mpu_data.gz_offset / 300;
}

uint8_t Mpu_Device_Init(void)
{
    uint8_t id;
    MPU_DELAY(100);

    id = SPI5_Read_Byte(MPU6500_WHO_AM_I);
    LL_USART_TransmitData8(USART3, id);
    uint8_t i = 0;
    uint8_t MPU6500_Init_Data[10][2] = {
        {MPU6500_PWR_MGMT_1, 0x80},     /* Reset Device */
        {MPU6500_PWR_MGMT_1, 0x03},     /* Clock Source - Gyro-Z */
        {MPU6500_PWR_MGMT_2, 0x00},     /* Enable Acc & Gyro */
        {MPU6500_CONFIG, 0x04},         /* LPF 41Hz */
        {MPU6500_GYRO_CONFIG, 0x18},    /* +-2000dps */
        {MPU6500_ACCEL_CONFIG, 0x10},   /* +-8G */
        {MPU6500_ACCEL_CONFIG_2, 0x02}, /* enable LowPassFilter  Set Acc LPF */
        {MPU6500_USER_CTRL, 0x20},
    }; /* Enable AUX */
    for (i = 0; i < 10; i++)
    {
        SPI5_Write_Byte(MPU6500_Init_Data[i][0], MPU6500_Init_Data[i][1]);
        MPU_DELAY(1);
    }

    Mpu_Set_Gyro_FSR(3);
    Mpu_Set_Accel_FSR(2);

    // ist8310_init();
    Mpu_Offset_Call();
    return 0;
}
Imu_t *Get_Imu_Date_Now(void)
{
    return &imu;
}

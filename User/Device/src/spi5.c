#include "spi5.h"

#define SPI5_NSS_LOW HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, GPIO_PIN_RESET)
#define SPI5_NSS_HIGH HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, GPIO_PIN_SET)

static uint8_t tx, rx;
static uint8_t tx_buff[14] = {0xff};

uint8_t SPI5_Write_Byte(uint8_t const reg, uint8_t const data)
{
    SPI5_NSS_LOW;
    tx = reg & 0x7F;
    HAL_SPI_TransmitReceive(&hspi5, &tx, &rx, 1, 55);
    tx = data;
    HAL_SPI_TransmitReceive(&hspi5, &tx, &rx, 1, 55);
    SPI5_NSS_HIGH;
    return 0;
}

#define MPU_HSPI hspi5
#define MPU_NSS_LOW HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, GPIO_PIN_RESET)
#define MPU_NSS_HIGH HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, GPIO_PIN_SET)

uint8_t SPI5_Read_Byte(uint8_t const reg)
{
    MPU_NSS_LOW;
    tx = reg | 0x80;
    HAL_SPI_TransmitReceive(&MPU_HSPI, &tx, &rx, 1, 55);
    HAL_SPI_TransmitReceive(&MPU_HSPI, &tx, &rx, 1, 55);
    MPU_NSS_HIGH;
    return rx;
}

uint8_t SPI5_Read_Bytes(uint8_t const regAddr, uint8_t *pData, uint8_t len)
{
    SPI5_NSS_LOW;
    tx = regAddr | 0x80;
    tx_buff[0] = tx;
    HAL_SPI_TransmitReceive(&hspi5, &tx, &rx, 1, 55);
    HAL_SPI_TransmitReceive(&hspi5, tx_buff, pData, len, 55);
    SPI5_NSS_HIGH;
    return 0;
}



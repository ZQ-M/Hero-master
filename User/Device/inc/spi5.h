#ifndef __SPI5_H_
#define __SPI5_H_

#include "spi.h"

uint8_t SPI5_Write_Byte(uint8_t const reg, uint8_t const data);
uint8_t SPI5_Read_Byte(uint8_t const reg);
uint8_t SPI5_Read_Bytes(uint8_t const regAddr, uint8_t *pData, uint8_t len);

#endif //__SPI5_H_

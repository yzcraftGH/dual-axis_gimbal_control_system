/**
  *****************************************************************************
  * @file		bsp_i2c.c
  * @brief		封装 HAL 的 I2C 读写,处理 HAL 错误码,检查设备是否在线
  * @author		yzcraft
  * @date		2026-10-06
  * @attention
  *****************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp_i2c.h"
#include "main.h"

/* Exported functions --------------------------------------------------------*/

uint8_t bsp_i2c_write_reg(uint8_t dev_addr, uint8_t reg, uint8_t data)
{
	if ( HAL_I2C_Mem_Write(&hi2c1, dev_addr, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 100) != HAL_OK )
	{
		return 1;
	}
	return 0;
}

uint8_t bsp_i2c_read_reg(uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
	if (buf == 0 || len == 0) {return 1;}

	if ( HAL_I2C_Mem_Read(&hi2c1, dev_addr, reg, I2C_MEMADD_SIZE_8BIT, buf, len, 100) != HAL_OK )
	{
		return 1;
	}
	return 0;
}

uint8_t bsp_i2c_is_device_ready(uint8_t dev_addr)
{
	if (HAL_I2C_IsDeviceReady(&hi2c1, dev_addr, 3, 100) != HAL_OK) {
		return 1;
	}
	return 0;
}









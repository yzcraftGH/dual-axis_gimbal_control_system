/**
  *****************************************************************************
  * @file		bsp_i2c.h
  * @brief		封装 HAL 的 I2C 读写,处理 HAL 错误码,检查设备是否在线
  * @author		yzcraft
  * @date		2026-10-06
  * @attention
  *****************************************************************************
  */
#ifndef __BSP_I2C_H
#define __BSP_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported functions prototypes ---------------------------------------------*/

/**
 * @brief	向指定设备的寄存器写入一个字节
 * @param	dev_addr	设备的8位地址(已左移一位)
 * @param	reg			寄存器地址
 * @param	data		写入的字节
 * @retval	0	成功		1	失败
 */
uint8_t bsp_i2c_write_reg(uint8_t dev_addr, uint8_t reg, uint8_t data);

/**
 * @brief		从指定设备的寄存器中连续读取多个字节
 * @param		dev_addr	设备的8位地址(已左移一位)
 * @param		reg			寄存器地址
 * @param[out]	buf			接收缓冲区
 * @param		len			读取长度
 * @retval		0	成功		1	失败
 */
uint8_t bsp_i2c_read_reg(uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint16_t len);

/**
 * @brief	检查设备是否在线
 * @param	dev_addr	设备的8位地址(已左移一位)
 * @retval	0	在线		1	不在线
 */
uint8_t bsp_i2c_is_device_ready(uint8_t dev_addr);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_I2C_H */





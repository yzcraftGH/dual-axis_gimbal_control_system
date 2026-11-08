/**
  *****************************************************************************
  * @file		dev_mpu6050.h
  * @brief		驱动 MPU6050 芯片:初始化,读原始数据,零位校准
  * @author		yzcraft
  * @date		2026-10-06
  * @attention
  *****************************************************************************
  */

#ifndef __DEV_MPU6050_H
#define __DEV_MPU6050_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
/* Exported types ------------------------------------------------------------*/

/**
 * @brief	mpu6050 原始数据结构体
 */
typedef struct{
	int16_t ax, ay, az; 	/**< 加速度原始值 */
	int16_t gx, gy, gz;		/**< 角速度原始值 */
}dev_mpu6050_raw_t;

/* Exported functions prototypes ---------------------------------------------*/

/**
 * @brief	初始化 mpu6050
 * @retval	0-成功	1-失败
 * @note	内部包含复位延迟 和 WHO_AM_I 校验
 */
uint8_t dev_mpu6050_init(void);

/**
 * @brief		读取原始数据
 * @param[out]	raw 原始数据输出缓冲区
 * @retval		0-成功	1-失败
 */
uint8_t dev_mpu6050_read_raw(dev_mpu6050_raw_t *raw);

/**
 * @brief  零位校准,采集偏置
 * @note   调用时传感器必须静止,内部采集 100 次求平均
 * @retval		0-成功	1-失败
 */
uint8_t dev_mpu6050_calibrate(void);

/**
 * @brief  		获取零位偏置
 * @param[out]  off  输出缓冲区
 */
void dev_mpu6050_get_offset(dev_mpu6050_raw_t *off);

#ifdef __cplusplus
}
#endif

#endif /* __DEV_MPU6050_H */





















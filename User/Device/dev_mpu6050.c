/**
  *****************************************************************************
  * @file		dev_mpu6050.c
  * @brief		驱动 MPU6050 芯片:初始化,读原始数据,零位校准
  * @author		yzcraft
  * @date		2026-10-06
  * @attention
  *****************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "dev_mpu6050.h"
#include "bsp_i2c.h"
#include "main.h"
/* Private define ------------------------------------------------------------*/
#define MPU6050_ADDR        (0x68 << 1)   /**< 8 位地址 0xD0 */
/* 寄存器地址表 */
#define MPU6050_WHO_AM_I     0x75
#define MPU6050_PWR_MGMT_1   0x6B
#define MPU6050_GYRO_CONFIG  0x1B
#define MPU6050_ACCEL_CONFIG 0x1C
#define MPU6050_CONFIG       0x1A
#define MPU6050_ACCEL_XOUT_H 0x3B

/* Private variables ---------------------------------------------------------*/
static dev_mpu6050_raw_t offset;	/**< 零位偏置 */
static uint8_t is_inited = 0;			/**< 初始化标志 1-已初始化 0-未初始化 */

/* Exported functions --------------------------------------------------------*/

uint8_t dev_mpu6050_init(void)
{
	/* 验证设备是否在线 */
	if ( bsp_i2c_is_device_ready(MPU6050_ADDR) != 0  )
	{
		return 1;
	}
	/* 读whoami */
	uint8_t who;
	if (bsp_i2c_read_reg(MPU6050_ADDR, MPU6050_WHO_AM_I, &who, 1) != 0){return 1;}
	if (who != 0x68)
	{
		return 1;
	}
	/* 写0x80复位 */
	if (bsp_i2c_write_reg(MPU6050_ADDR, MPU6050_PWR_MGMT_1, 0x80) != 0){return 1;}

	HAL_Delay(100);
	/* 0x00唤醒 */
	if (bsp_i2c_write_reg(MPU6050_ADDR, MPU6050_PWR_MGMT_1, 0x00) != 0){return 1;}
	/* 配置陀螺仪量程 0x18(+-2000度/s) */
	if (bsp_i2c_write_reg(MPU6050_ADDR, MPU6050_GYRO_CONFIG, 0x18) != 0){return 1;}
	/* 配置加速度计量程 0x00(+- 2g)*/
	if (bsp_i2c_write_reg(MPU6050_ADDR, MPU6050_ACCEL_CONFIG, 0x00) != 0){return 1;}
	/* 配置数字低通滤波器(DLPF) 0x04(DLPF 21Hz) */
	if (bsp_i2c_write_reg(MPU6050_ADDR, MPU6050_CONFIG, 0x04)!= 0) {return 1;}
	/* 更新初始化标志 */
	is_inited = 1;

	return 0;
}

uint8_t dev_mpu6050_read_raw(dev_mpu6050_raw_t *raw)
{
	if (raw == 0 || is_inited == 0) {return 1;}

	/* 读加速度计原始值 */
	uint8_t buf[14];
	if (bsp_i2c_read_reg(MPU6050_ADDR, MPU6050_ACCEL_XOUT_H, buf, 14) != 0) {
		return 1;
	}
	/* 高低位拼接成完整数据 */
	raw->ax = (int16_t)(buf[0] << 8 | buf[1]);
	raw->ay = (int16_t)(buf[2] << 8 | buf[3]);
	raw->az = (int16_t)(buf[4] << 8 | buf[5]);
    /* buf[6,7] 温度,跳过 */
	raw->gx = (int16_t)(buf[8] << 8 | buf[9]);
	raw->gy = (int16_t)(buf[10] << 8 | buf[11]);
	raw->gz = (int16_t)(buf[12] << 8 | buf[13]);

	return 0;
}

uint8_t dev_mpu6050_calibrate(void)
{
	if (is_inited == 0) {return 1;}

	int32_t sum_ax = 0 , sum_ay = 0, sum_az = 0;
	int32_t sum_gx = 0 , sum_gy = 0, sum_gz = 0;
	dev_mpu6050_raw_t raw;
	/* 取100次求平均 */
	uint8_t count = 0;
	for (uint8_t i = 0; i < 100; i++)
	{
		if( dev_mpu6050_read_raw(&raw) == 0 )
		{
			sum_ax += raw.ax; sum_ay += raw.ay; sum_az += raw.az;
			sum_gx += raw.gx; sum_gy += raw.gy; sum_gz += raw.gz;
			count++;
		}
		HAL_Delay(5);
	}
	if (count == 0){return 1;}

	offset.ax = sum_ax / count;
	offset.ay = sum_ay / count;
	/* 防止可能的计算溢出边界 */
	int32_t az_avg = sum_az / count;
	offset.az = (int16_t)(az_avg - 16384);

	offset.gx = sum_gx / count;
	offset.gy = sum_gy / count;
	offset.gz = sum_gz / count;

	return 0;
}

void dev_mpu6050_get_offset(dev_mpu6050_raw_t *off)
{
	if (off == 0){return ;}

	*off = offset;
}









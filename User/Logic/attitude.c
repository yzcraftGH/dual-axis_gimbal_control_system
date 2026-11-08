/**
  *****************************************************************************
  * @file		attitude.c
  * @brief		计算并输出姿态角
  * @author		yzcraft
  * @date		2026-10-06
  * @attention
  *****************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "attitude.h"
#include "filter.h"
#include "dev_mpu6050.h"
#include "main.h"
#include "led.h"
#include <math.h>
/* Private define ------------------------------------------------------------*/
#define ATTITUDE_COMP_COEF 0.60f  /**< 互补滤波系数:越大越平滑但越慢,越小越快但越抖 */
/*
 * 你的胆子真是肥嘟嘟的
 * 说互补滤波最优参数是0.98
 * 实测下来都想填0,
 * 还是填个0.1意思一下,不然就浪费表情了
 * 毕竟下面还有一个一阶低通
 *
 * 回上面 ^
 * 我的胆子真是肥嘟嘟的
 * 以后不敢乱改参数了
 *
 * 回上面 ^
 * 我的胆子就是肥嘟嘟的
 * 我的脸也肥嘟嘟的,肚子也肥嘟嘟的
 * 由于陀螺仪滞后带来的问题
 * 将系数降低到0.60
 */
/* Private variables ---------------------------------------------------------*/
static filter_comp_t filter_pitch;
static filter_comp_t filter_roll;
static dev_mpu6050_raw_t offset;
static uint8_t attitude_ok = 0;
/* 失败次数计数标志 用于检测并尝试恢复	mpu6050 */
static uint8_t fail_count = 0;

/* 最新角速度 (度/s):供云台做前馈,让目标角"跟着手"而非"追上滤波结果" */
static float attitude_pitch_rate = 0.0f;
static float attitude_roll_rate  = 0.0f;

/* Exported functions --------------------------------------------------------*/

void logic_attitude_init(void)
{
	if (dev_mpu6050_init() != 0)
	{
		attitude_ok = 0;
		logic_led_set_error(LED_ERR_MPU6050);
		return ;
	}
	/* 等待芯片稳定 */
	HAL_Delay(500);
	/* 采集并设置偏置 */
	if (dev_mpu6050_calibrate() != 0) {
	    attitude_ok = 0;
	    logic_led_set_error(LED_ERR_MPU6050);
	    return;
	}
	dev_mpu6050_get_offset(&offset);

	/* 初始化互补滤波器 */
	filter_comp_init(&filter_roll, ATTITUDE_COMP_COEF);
	filter_comp_init(&filter_pitch, ATTITUDE_COMP_COEF);

	attitude_ok = 1;
	fail_count = 0;

	logic_led_clear_error(LED_ERR_MPU6050);
}

void logic_attitude_task(void)
{

	if (attitude_ok == 0) {return ;}

	dev_mpu6050_raw_t raw;
	if (dev_mpu6050_read_raw(&raw) != 0 )
	{
		if (++fail_count >= 10)
		{
			attitude_ok = 0;
			logic_led_set_error(LED_ERR_MPU6050);
		}
		return ;
	}
	fail_count =0;

	/* 减去偏置 + 换算单位 */
	float ax = (raw.ax - offset.ax) / 16384.0f;
	float ay = (raw.ay - offset.ay) / 16384.0f;
	float az = (raw.az - offset.az) / 16384.0f;
	float gx = (raw.gx - offset.gx) / 16.4f;    /* +/-2000度/s 量程 */
	float gy = (raw.gy - offset.gy) / 16.4f;


	/* 保存角速度供云台前馈使用 */
	attitude_pitch_rate = gx;
	attitude_roll_rate  = gy;


	/* 加速度计解算角度 */
	float acc_pitch = atan2f(ay, az) * 57.2958f;
	/* 横滚先转,分量干净,分母用模,避免被俯仰污染 */
	/* 此处用模分母减少奇异点 */
	float acc_roll  = atan2f(ax, sqrtf(ay*ay + az*az)) * 57.2958f;

	/* 互补滤波融合 */
	uint32_t now = HAL_GetTick();
	filter_comp_update(&filter_pitch, acc_pitch, gx, now);
	filter_comp_update(&filter_roll,  acc_roll,  gy, now);
}

uint8_t logic_attitude_calibrate(void)
{

	if ( attitude_ok == 0 ) {return 1;}
	if (dev_mpu6050_calibrate() != 0 ){return 1;}
	dev_mpu6050_get_offset(&offset);
	fail_count = 0;
	return 0;
}

float logic_attitude_get_pitch(void)
{
	return filter_pitch.angle;
}

float logic_attitude_get_roll(void)
{
	return filter_roll.angle;
}


float logic_attitude_get_pitch_rate(void)
{
	return attitude_pitch_rate;
}

float logic_attitude_get_roll_rate(void)
{
	return attitude_roll_rate;
}

uint8_t logic_attitude_is_ok(void)
{
	return attitude_ok;
}


void logic_attitude_reset(void)
{
	filter_comp_reset(&filter_pitch);
	filter_comp_reset(&filter_roll);
}
















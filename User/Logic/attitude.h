/**
  *****************************************************************************
  * @file		attitude.h
  * @brief		计算并输出姿态角
  * @author		yzcraft
  * @date		2026-10-06
  * @attention
  *****************************************************************************
  */

#ifndef __ATTITUDE_H
#define __ATTITUDE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
/* Exported functions --------------------------------------------------------*/

/**
 * @brief	初始化,上电自动校准
 */
void logic_attitude_init(void);

/**
 * @brief	姿态计算任务,
 * @note	需每20ms 调用一次
 */
void logic_attitude_task(void);

/**
 * @brief	执行零位校准
 * @note	CAL 指令调用
 * @retval	0 = 成功 1 = 失败
 */
uint8_t logic_attitude_calibrate(void);

/**
 * @brief	获取俯仰角
 * @retval	俯仰角 单位 度
 */
float logic_attitude_get_pitch(void);

/**
 * @brief	获取滚转角
 * @retval	滚转角 单位 度
 */
float logic_attitude_get_roll(void);

/**
 * @brief	获取俯仰角速度
 * @retval	俯仰角速度 单位 度/s
 */
float logic_attitude_get_pitch_rate(void);

/**
 * @brief	获取横滚角速度
 * @retval	横滚角速度 单位 度/s
 */
float logic_attitude_get_roll_rate(void);

/**
 * @brief	检查姿态是否正常
 * @retval	1=正常,0=异常
 */
uint8_t logic_attitude_is_ok(void);

/**
 * @brief	重置姿态角
 */
void logic_attitude_reset(void);


#ifdef __cplusplus
}
#endif

#endif /* __ATTITUDE_H */





















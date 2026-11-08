/**
  *****************************************************************************
  * @file		gimbal.h
  * @brief		维护两轴目标角,提供回中位和设置角度接口
  * @author		yzcraft
  * @date		2026-10-04
  * @attention
  *****************************************************************************
  */
#ifndef __GIMBAL_H
#define __GIMBAL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/

/**
 * @brief	舵机轴枚举
 */
typedef enum{
	LOGIC_GIMBAL_YAW = 0,
	LOGIC_GIMBAL_PITCH
}logic_gimbal_axis_t;

/* Exported functions prototypes ---------------------------------------------*/

/**
 * @brief	初始化两轴
 */
void logic_gimbal_init(void);

/**
 * @brief	设置某轴的角度
 * @param	axis	LOGIC_GIMBAL_YAW, LOGIC_GIMBAL_PITCH
 * @param	angle	取值范围[0,180]
 */
void logic_gimbal_set_angle(logic_gimbal_axis_t axis, uint8_t angle);

/**
 * @brief	把两轴设为90度
 */
void logic_gimbal_center(void);

/**
 * @brief	获取目标轴的角度
 * @param	axis	LOGIC_GIMBAL_YAW, LOGIC_GIMBAL_PITCH
 * @retval	角度 [0,180]
 */
uint8_t logic_gimbal_get_target_angle(logic_gimbal_axis_t axis);

/**
 * @brief  云台周期任务,模式二下执行姿态随动
 * @note   每 20ms 调用一次
 */
void logic_gimbal_task(void);

#ifdef __cplusplus
}
#endif

#endif /* __GIMBAL_H */












/**
  *****************************************************************************
  * @file		dev_servo.h
  * @brief		角度 到 脉宽的换算,调用 bsp_pwm
  * @author		yzcraft
  * @date		2026-10-04
  * @attention
  *****************************************************************************
  */
#ifndef __DEV_SERVO_H
#define __DEV_SERVO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>

/* Exported types ------------------------------------------------------------*/

/**
 * @brief	枚举轴
 */
typedef enum{
	DEV_SERVO_YAW = 0,
	DEV_SERVO_PITCH
}dev_servo_axis_t;

/* Exported functions prototypes ---------------------------------------------*/

/**
 * @brief	初始化舵机,双轴回中位90度
 * @note	调用前须先调用 bsp_pwm_init() 启动 PWM 输出
 */
void dev_servo_init(void);

/**
 * @brief	设置舵机角度
 * @param	axis 选择舵机:YAW , PITCH
 * @param	angle 设置角度 取值范围 [0 , 180]
 */
void dev_servo_set_angle(dev_servo_axis_t axis , uint8_t angle);

#ifdef __cplusplus
}
#endif

#endif /* __DEV_SERVO_H */

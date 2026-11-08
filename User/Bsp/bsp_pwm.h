/**
  *****************************************************************************
  * @file		bsp_pwm.h
  * @brief		把 PWM 底层:启动 TIM1 CH1/CH4,提供设置脉宽接口
  * @author		yzcraft
  * @date		2026-10-04
  * @attention
  *****************************************************************************
  */

#ifndef __BSP_PWM_H
#define __BSP_PWM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
/* Exported types ------------------------------------------------------------*/

/**
 * @brief  PWM 通道枚举
 */
typedef enum {
	BSP_PWM_YAW = 0,      /**< 方位轴:TIM1_CH1 (PA8) */
	BSP_PWM_PITCH         /**< 俯仰轴:TIM1_CH4 (PA11) */
} bsp_pwm_channel_t;

/* Exported functions prototypes ---------------------------------------------*/

/**
 * @brief  启动 PWM 输出
 */
void bsp_pwm_init(void);

/**
 * @brief	设置脉宽
 * @param	PWM 通道枚举编号:BSP_PWM_YAW,BSP_PWM_PITCH
 * @param	pulse 脉宽 单位:us 取值范围[500,2500]
 */
void bsp_pwm_set_pulse(bsp_pwm_channel_t ch , uint16_t pulse);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_PWM_H */



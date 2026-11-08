/**
  *****************************************************************************
  * @file		bsp_pwm.c
  * @brief		启动 TIM1 的 CH1,CH4 PWM 输出
  * 			提供"设置脉宽"的接口;处理 PWM 通道的底层操作
  * @author		yzcraft
  * @date		2026-10-04
  * @attention
  *****************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "bsp_pwm.h"
#include "main.h"

 /* Exported functions --------------------------------------------------------*/

void bsp_pwm_init(void)
{
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
}

void bsp_pwm_set_pulse(bsp_pwm_channel_t ch , uint16_t pulse)
{
	if (ch > BSP_PWM_PITCH || pulse < 500 || pulse >2500) {return ;}
	/* PSC=71,ARR=19999,计数频率 1MHz,CCR 直接等于微秒数 */
	if (ch == BSP_PWM_YAW)
	{
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,pulse);
	}
	else if (ch == BSP_PWM_PITCH)
	{
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,pulse);
	}
}













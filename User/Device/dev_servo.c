/**
  *****************************************************************************
  * @file		dev_servo.c
  * @brief		角度 到 脉宽的换算,调用 bsp_pwm
  * @author		yzcraft
  * @date		2026-10-04
  * @attention
  *****************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "dev_servo.h"
#include "bsp_pwm.h"

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief	dev_servo_axis_t 到 bsp_pwm_channel_t 的映射
 */
static bsp_pwm_channel_t servo_axis_to_pwm(dev_servo_axis_t axis);

/* Exported functions --------------------------------------------------------*/

void dev_servo_init(void)
{
	dev_servo_set_angle(DEV_SERVO_YAW, 90);
	dev_servo_set_angle(DEV_SERVO_PITCH, 90);
}

void dev_servo_set_angle(dev_servo_axis_t axis , uint8_t angle)
{
	if(axis > DEV_SERVO_PITCH || angle > 180 ) {return ;}
	 /* 角度 -> 脉宽:0度->500us,180度->2500us */
	uint16_t pulse = 500 + (uint32_t)angle* 2000 /180 ;
	bsp_pwm_set_pulse(servo_axis_to_pwm(axis), pulse);
}

/* Private functions ---------------------------------------------------------*/

static bsp_pwm_channel_t servo_axis_to_pwm(dev_servo_axis_t axis)
{
	if(axis == DEV_SERVO_YAW)
	{
		return BSP_PWM_YAW;
	}
	return BSP_PWM_PITCH;

}




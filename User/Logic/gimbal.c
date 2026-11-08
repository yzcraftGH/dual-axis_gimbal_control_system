/**
  *****************************************************************************
  * @file		gimbal.c
  * @brief		维护两轴目标角,提供回中位和设置角度接口
  * @author		yzcraft
  * @date		2026-10-04
  * @attention
  *****************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "gimbal.h"
#include "dev_servo.h"
#include "attitude.h"
#include "main.h"
#include "app_mode.h"
#include "filter.h"
/* Private define ------------------------------------------------------------*/

/** 前馈系数:角速度 -> 目标角提前量,单位 秒.先试 0.06,范围 0.03~0.12 */
#define GIMBAL_FEEDFORWARD_GAIN   0.06f

/** 舵机输出一阶低通的系数:越大越平滑但越滞后 */
#define GIMBAL_SERVO_LPF_COEF     0.75f

/* Private variables ---------------------------------------------------------*/
static uint8_t target_yaw;
static uint8_t target_pitch;

/* 舵机输出的一阶低通:只用来抹掉"取整到 1度"的台阶感,不承担平滑滞后的职责 */
static filter_comp_t servo_yaw_lpf;
static filter_comp_t servo_pitch_lpf;


/* Private function prototypes -----------------------------------------------*/

/**
 * @brief	将logic_gimbal_axis_t映射到 dev_servo_axis_t
 * @param	axis	LOGIC_GIMBAL_YAW, LOGIC_GIMBAL_PITCH
 */
static dev_servo_axis_t gimbal_logic_to_dev(logic_gimbal_axis_t axis);

/* Exported functions --------------------------------------------------------*/

void logic_gimbal_init(void)
{
	target_yaw = 90;
	target_pitch = 90;

	filter_comp_init(&servo_yaw_lpf,   GIMBAL_SERVO_LPF_COEF);
	filter_comp_init(&servo_pitch_lpf, GIMBAL_SERVO_LPF_COEF);

	dev_servo_init();

}

void logic_gimbal_set_angle(logic_gimbal_axis_t axis, uint8_t angle)
{
	if (axis > LOGIC_GIMBAL_PITCH || angle > 180) {return ;}

	dev_servo_set_angle(gimbal_logic_to_dev(axis), angle);
	if(axis == LOGIC_GIMBAL_YAW)
	{
		target_yaw = angle;
	}
	else
	{
		target_pitch = angle;
	}
}

void logic_gimbal_center(void)
{
	logic_gimbal_set_angle(LOGIC_GIMBAL_YAW, 90);
	logic_gimbal_set_angle(LOGIC_GIMBAL_PITCH, 90);
}

uint8_t logic_gimbal_get_target_angle(logic_gimbal_axis_t axis)
{
	if (axis > LOGIC_GIMBAL_PITCH) {return 90;}

	if(axis == LOGIC_GIMBAL_YAW)
	{
		return target_yaw;
	}
	return target_pitch;
}

void logic_gimbal_task(void)
{
	/* 尝试恢复 MPU6050 :提前,在任何时刻尝试恢复 */
	if (logic_attitude_is_ok() == 0) {
		static uint32_t last_retry = 0;
		if (HAL_GetTick() - last_retry >= 5000) {   /* 每 5 秒重试一次 */
			last_retry = HAL_GetTick();
			logic_attitude_init();
		}
		return ;
	}


    /* 仅模式二随动 */
	if(app_mode_get() != MODE_FOLLOW) {return ;}

	/* 更新姿态角 */
	logic_attitude_task();

	/* 姿态异常,不随动 */
	if (logic_attitude_is_ok() == 0) {return ;}

	/* 读姿态角和角速度 */
	float pitch      = logic_attitude_get_pitch();
	float roll       = logic_attitude_get_roll();
	float pitch_rate = logic_attitude_get_pitch_rate();
	float roll_rate  = logic_attitude_get_roll_rate();

	/* 姿态角 -> 云台目标角,中位 90度
	 * 前馈项:用角速度预测目标位置,抵消互补滤波和舵机的滞后,做出"跟手"的手感 */
	float pitch_target_f = 90.0f + pitch + GIMBAL_FEEDFORWARD_GAIN * pitch_rate;
	float yaw_target_f   = 90.0f - roll  - GIMBAL_FEEDFORWARD_GAIN * roll_rate;

	/*
	 * 轻量一阶低通:
	 *   降低 1° 量化的跳变频率，避免舵机在相邻两度之间来回跳而"嗡嗡"响
	 *     所以每一次跳变都会被舵机执行；目标角高频抖动时会在相邻两度之间来回跳，舵机会"嗡嗡"响。
	 *  (注意)会压住前馈的提前量
	 *    */
	uint32_t now_ms = HAL_GetTick();
	pitch_target_f = filter_comp_update(&servo_pitch_lpf, pitch_target_f, 0.0f, now_ms);
	yaw_target_f   = filter_comp_update(&servo_yaw_lpf,   yaw_target_f,   0.0f, now_ms);

	/* 四舍五入到整度 */
	int16_t pitch_target = (int16_t)(pitch_target_f + 0.5f);
	int16_t yaw_target   = (int16_t)(yaw_target_f   + 0.5f);

	/* 饱和钳位到 0~180 */
	if (yaw_target < 0)     { yaw_target = 0; }
	if (yaw_target > 180)   { yaw_target = 180; }
	if (pitch_target < 0)   { pitch_target = 0; }
	if (pitch_target > 180) { pitch_target = 180; }

	logic_gimbal_set_angle(LOGIC_GIMBAL_PITCH, (uint8_t)pitch_target);
	logic_gimbal_set_angle(LOGIC_GIMBAL_YAW, (uint8_t)yaw_target);
}


/* Private functions ---------------------------------------------------------*/
static dev_servo_axis_t gimbal_logic_to_dev(logic_gimbal_axis_t axis)
{
	if(axis == LOGIC_GIMBAL_YAW)
	{
		return DEV_SERVO_YAW;
	}
	return DEV_SERVO_PITCH;
}









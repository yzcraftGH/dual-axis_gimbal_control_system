/**
  ******************************************************************************
  * @file    app_mode.c
  * @brief   三种模式切换功能的实现
  * @author  yzcraft
  * @date	 2026-09-29
  * @attention	本文件只管理模式状态,不直接操作硬件.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "app_mode.h"

#include "gimbal.h"
#include "attitude.h"
#include "track.h"

/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static app_mode_t app_mode_current;


/* Exported functions --------------------------------------------------------*/
void app_mode_init(void)
{
	app_mode_current = MODE_CMD;
}


void app_mode_set(app_mode_t mode)
{
	if((mode > MODE_TRACK) || mode < MODE_CMD) {return;}
	app_mode_current = mode;

	if (mode == MODE_CMD)
	{
		logic_gimbal_center();
	}
	else if (mode == MODE_FOLLOW)
	{
		logic_attitude_reset();
	}
	else if (mode == MODE_TRACK)
	{
		logic_track_reset();
	}
}

app_mode_t app_mode_get(void)
{
	return app_mode_current;
}

app_mode_t app_mode_next(app_mode_t mode_now)
{
	switch (mode_now)
	{
	case MODE_CMD:
		return MODE_FOLLOW;
	case MODE_FOLLOW:
		return MODE_TRACK;
	case MODE_TRACK:
		return MODE_CMD;
	default:
		return MODE_CMD;
	}
}

app_mode_t app_mode_prev(app_mode_t mode_now)
{
	switch (mode_now)
	{
	case MODE_CMD:
		return MODE_TRACK;
	case MODE_FOLLOW:
		return MODE_CMD;
	case MODE_TRACK:
		return MODE_FOLLOW;
	default:
		return MODE_CMD;
	}
}

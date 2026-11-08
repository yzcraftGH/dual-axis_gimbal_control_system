/**
  *****************************************************************************
  * @file		filter.c
  * @brief		通用滤波算法
  * @author		yzcraft
  * @date		2026-10-05
  * @attention	时间戳由调用者传入,保持纯净
  *****************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "filter.h"
#include <string.h>

/* Private variables ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
void filter_comp_init(filter_comp_t *f, float coef)
{
	if (f == 0 || coef < 0 || coef > 1) {return ;}

	f->angle = 0.0f;
	f->coef = coef;
	f->is_updated = 0;
	f->last_tick = 0;
}

float filter_comp_update(filter_comp_t *f, float acc_angle , float gyro_rate, uint32_t now)
{
	if (f == 0) {return 0.0f;}

	if(f->is_updated == 0)
	{
		f->angle = acc_angle;
		f->last_tick = now;
		f->is_updated = 1;
		return f->angle;
	}

	/* 计算dt 单位: s */
	float dt = (float)(now - f->last_tick) / 1000.0f;
	/* 更新时间戳 */
	f->last_tick = now;

	/* 防止 dt 异常(如首次运行,定时器溢出) */
	if (dt <= 0.0f || dt > 0.1f) {
		dt = 0.02f;   /* 默认 20ms */
	}

	/* 互补滤波 */
	f->angle = f->coef*(f->angle + gyro_rate*dt) + (1.0f - f->coef)*acc_angle;

	return f->angle ;
}

void filter_comp_reset(filter_comp_t *f)
{
	if (f == 0) {
		return;
	}
	f->is_updated = 0;
}


/*======================滑动平均滤波器====================*/

void filter_ma_init(filter_ma_t *f, uint8_t win)
{
	if (f == 0) {return ;}
	if (win == 0) {win = 1;}
	if (win > FILTER_MA_MAX_WIN) {win = FILTER_MA_MAX_WIN;}

	memset(f , 0 , sizeof(filter_ma_t));
	f->win_len = win;
}

void filter_ma_reset(filter_ma_t *f)
{
	uint8_t win_l;
	if (f == 0) {return ;}

	win_l = f->win_len;
	memset(f , 0 , sizeof(filter_ma_t));
	f->win_len = win_l;
}

float filter_ma_update(filter_ma_t *f, float sample)
{
	if (f == 0 || f->win_len == 0) {return sample;}

	/* 更新总和,并减去被覆盖的旧数据 */
	f->sum += sample - f->buf[f->write_pos];
	f->buf[f->write_pos] = sample;

	f->write_pos ++;
	if (f->write_pos >= f->win_len)
	{
		f->write_pos = 0;
	}

	if (f->is_full == 0)
	{
		f->count ++;
		if (f->count >= f->win_len)
		{
			f->count = f->win_len;
			f->is_full = 1;
		}
	}

	/*处理均值*/
	if(f->sum == 0.0f || f->count == 0)
	{
		f->mean = 0.0f;
	}
	else
	{
		f->mean = f->sum / (float)f->count;
	}

	return f->mean;
}

float filter_ma_get(filter_ma_t *f)
{
	if (f == 0) {return 0.0f;}
	return f->mean;
}






















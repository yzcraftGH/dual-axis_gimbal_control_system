/**
  ******************************************************************************
  * @file    delay.c
  * @brief   非阻塞一次性延时
  * @author  yzcraft
  * @date    2026-09-30
  * @note    基于 HAL_GetTick(),不阻塞主循环.
  * @attention
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "delay.h"
#include "main.h"

/* Exported functions --------------------------------------------------------*/

void delay_start(delay_t *d, uint32_t interval_ms)
{
	if (d == 0 || interval_ms == 0) {return;}
	d->start_tick = HAL_GetTick();
	d->interval_ms = interval_ms;
	d->running = 1;
}

uint8_t delay_is_expired(delay_t *d)
{
	if(d == 0 || d->running == 0) {return 0;}
	uint32_t now =HAL_GetTick();
	if ((now - d->start_tick) >= (d->interval_ms))
	{
		d->running = 0;  /*一次性 到期即停止*/
		d->start_tick = 0;
		return 1;
	}
	return 0;
}

uint8_t delay_is_running(delay_t *d)
{
	if (d == 0) {
		return 0;
	}
	return d->running;
}

void delay_cancel(delay_t *d)
{
	if (d == 0) {
		return;
	}
	d->start_tick = 0;
    /* interval_ms 保留,方便查看原设定值 */
	d->running = 0;
}




/**
  ******************************************************************************
  * @file    key.c
  * @brief   按键逻辑的实现
  * @author  yzcraft
  * @date	 2026-09-28
  * @attention
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "key.h"
#include "main.h"
#include "bsp_gpio.h"

/* Private define ------------------------------------------------------------*/
#define KEY_SHORT_MIN_MS 50U
#define KEY_LONG_MIN_MS 1000U /* 可以 改成800以消除边界误差 */
#define KEY_DEBOUNCE_MS 20U

/* Private variables ---------------------------------------------------------*/
static  key_event_t key_event_temp;
static  bsp_key_state_t key_state_now;
static  uint32_t press_tick;      /* 真实按下时刻,用于长短按计时 */
/* 消抖相关 */
static  bsp_key_state_t last_raw_level;
static  uint32_t raw_change_tick;
/* 防止按键跳动把按下时刻滞后 */
static  uint8_t  press_locked;


/* Exported functions --------------------------------------------------------*/
void logic_key_init(void)
{
	key_event_temp = KEY_EVENT_NONE;
	key_state_now = BSP_KEY_RELEASED;
	last_raw_level = BSP_KEY_RELEASED;
	raw_change_tick = HAL_GetTick();
	press_tick = HAL_GetTick();
	press_locked = 0;
}

void logic_key_task(void)
{


	bsp_key_state_t now_state = bsp_key_read();

	/* ----------- 消抖 ----------- */
	uint32_t now = HAL_GetTick();

	if (now_state != last_raw_level)
	{
		/* 原始电平变化的瞬间记录按下时刻,作为长短按计时基准;
		   按下锁一次,抖动回弹不会把时刻往后推 */
		if ((now_state == BSP_KEY_PRESSED) && (key_state_now == BSP_KEY_RELEASED)
			&& (press_locked == 0))
		{
			press_tick = now;
			press_locked = 1;
		}
		last_raw_level = now_state;
		raw_change_tick = now; 					/* 记录变化时刻,松手也会被更新一次 */
	}
	if ((now - raw_change_tick) < KEY_DEBOUNCE_MS) /* <20ms视为抖动 */
		/* 此处会在松手后延迟20ms处理事件 */
	{
		return;                           		/* 还没稳定,等下一轮 */
	}

	/* 松开后解锁,准备下一次按下 */
	if ((now_state == BSP_KEY_RELEASED) && (key_state_now == BSP_KEY_RELEASED))
	{
		press_locked = 0;
	}


	if ((now_state == BSP_KEY_PRESSED) && (	key_state_now == BSP_KEY_RELEASED) )
	{
		key_state_now = BSP_KEY_PRESSED;			/* 此处不再计时 */
	}

	if ((now_state == BSP_KEY_RELEASED)&&(key_state_now == BSP_KEY_PRESSED))
	{
		/* 用 真实按下时刻 -> 松手时刻 判定 */
		uint32_t hold_ms = raw_change_tick - press_tick;
		if (hold_ms < KEY_SHORT_MIN_MS)
		{
			key_event_temp = KEY_EVENT_NONE;		/* <=50ms 抖动,不触发事件 */
		}
		else if(hold_ms >= KEY_LONG_MIN_MS)
		{
			key_event_temp = KEY_EVENT_LONG;		/* >=1.0s 长按 */
		}
		else
		{
			key_event_temp = KEY_EVENT_SHORT;		/* 50ms~1.0s 短按 */
		}
		key_state_now = BSP_KEY_RELEASED;
	}


}

key_event_t logic_key_get_event(void)
{
	key_event_t key_event_ret = key_event_temp;
	key_event_temp = KEY_EVENT_NONE;
	return key_event_ret;
}

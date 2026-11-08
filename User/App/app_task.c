/**
  ******************************************************************************
  * @file    app_task.c
  * @brief   任务调度:集中调用各模块 task 函数
  * @author  yzcraft
  * @date    2026-10-01
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
/* 任务一加入 */
#include "app_task.h"
#include "main.h"
#include "app_mode.h"
#include "key.h"
#include "led.h"
/* 任务二加入 */
#include "protocol.h"
#include "uart_event.h"
#include "oled_ui.h"
/* 任务三加入 */
#include "gimbal.h"
/* 任务五加入 */
#include "track.h"
/* Exported types ------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/*-----------更新由app_task统一管理------------*/
/* 任务一加入 */
#define KEY_TASK_INTERVAL_MS	10U     /**< 按键扫描周期 10ms */
#define LED_TASK_INTERVAL_MS	50U     /**< LED 更新周期 50ms */
/* 任务二加入 */
#define OLED_TASK_INTERVAL_MS	100U    /**< OLED 刷新周期 100ms */
/* 任务三加入 */
#define GIMBAL_TASK_INTERVAL_MS	20U		/**< GIMBAL更新周期 20ms */
/* 任务五加入 */
#define TRACK_TASK_INTERVAL_MS 	10U		/**< TRACK更新周期 10ms */

/* Private variables ---------------------------------------------------------*/
/* 任务一加入 */
static uint32_t last_tick_key;
static uint32_t	last_tick_led;
/* 任务二加入 */
static uint32_t last_tick_oled;
/* 任务三加入 */
static uint32_t last_tick_gimbal;
/* 任务五加入 */
static uint32_t last_tick_track;

/* Exported functions --------------------------------------------------------*/

void app_task_init(void)
{
	last_tick_key = HAL_GetTick();
	last_tick_led = HAL_GetTick();
	last_tick_oled = HAL_GetTick();
	last_tick_gimbal = HAL_GetTick();
	last_tick_track = HAL_GetTick();
}

void app_task_run(void)
{
	uint32_t now = HAL_GetTick();
	/* 每10ms扫描一次按键 */
	if(now - last_tick_key >= KEY_TASK_INTERVAL_MS)
	{
		last_tick_key = now;
		logic_key_task();

		key_event_t key_event_now = logic_key_get_event();
		switch (key_event_now)
		{
			case KEY_EVENT_SHORT:
				logic_uart_event_send_key_short();
				logic_oled_ui_last_key("SHORT");
				app_mode_set(app_mode_next(app_mode_get()));
				logic_uart_event_send_mode(app_mode_get());
				logic_led_task();
				break;
			case KEY_EVENT_LONG:
				logic_uart_event_send_key_long();
				logic_oled_ui_last_key("LONG");
				app_mode_set(app_mode_prev(app_mode_get()));
				logic_uart_event_send_mode(app_mode_get());
				logic_led_task();
				break;
			default:
				break;

		}
	}
	/* -------- 串口协议:每次循环调用,内部非阻塞 -------- */
	logic_protocol_task();

	/* -------- 同步最近有效指令给 oled_ui -------- */
	logic_oled_ui_last_cmd(logic_protocol_get_last_cmd());

	/* 每100ms刷新一次OLED*/
	if (now - last_tick_oled >= OLED_TASK_INTERVAL_MS)
	{
		last_tick_oled = now;
		logic_oled_ui_task();
	}
	/* 每50ms更新一次LED */
	if (now - last_tick_led >= LED_TASK_INTERVAL_MS)
	{
		last_tick_led = now;
		logic_led_task();
	}
	/* 每20ms更新一次GIMBAL */
	if (now - last_tick_gimbal >= GIMBAL_TASK_INTERVAL_MS)
	{
		last_tick_gimbal = now;
		logic_gimbal_task();
	}

	/* 每20ms更新一次TRACK */
	if (now - last_tick_track >= TRACK_TASK_INTERVAL_MS)
	{
		last_tick_track = now;
		logic_track_task();
	}


}




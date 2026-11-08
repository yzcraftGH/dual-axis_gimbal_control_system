/**
  ******************************************************************************
  * @file    led.c
  * @brief   LED的逻辑:运行状态 模式指示,
  * @author  yzcraft
  * @date	 2026-09-30
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "led.h"
#include "main.h"
#include "app_mode.h"
#include "bsp_gpio.h"
/* Exported types ------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define LED_BLINK_DELAY_MS 500U /**< 闪烁间隔500ms即为1Hz闪烁 */
#define SELF_TEST_ROUNDS 3U	    /**< 自检循环轮数 */
/* Private variables ---------------------------------------------------------*/
static uint8_t error_flag;      /**< 错误源位掩码,非 0 即 LED1 1Hz 闪烁 */
static uint32_t led1_last_toggle;
static uint32_t led2_last_toggle;

/* Exported functions --------------------------------------------------------*/

void logic_led_init(void)
{
	bsp_led_off(BSP_LED_1);
	bsp_led_off(BSP_LED_2);
	bsp_led_off(BSP_LED_3);
	error_flag = 0;
	led1_last_toggle = HAL_GetTick();
	led2_last_toggle = HAL_GetTick();
}

void logic_led_set_error(uint8_t err_mask)
{
	error_flag |= err_mask;         /* 只置位指定源,不动其它位 */
}

void logic_led_clear_error(uint8_t err_mask)
{
	error_flag &= (uint8_t)(~err_mask);   /* 只清除指定源,不动其它位 */
}

void logic_led_task(void)
{
	 uint32_t now = HAL_GetTick();
	/*LED_1 逻辑*/
	if(error_flag != 0)
	{

		/* 错误即1Hz闪烁 */
		if(now - led1_last_toggle > LED_BLINK_DELAY_MS)
		{
			led1_last_toggle = now;	/* 更新时间戳 */
			bsp_led_toggle(BSP_LED_1);
		}
	}
	else
	{
		bsp_led_on(BSP_LED_1);
		led1_last_toggle = now;		/* 常亮时保持时间戳同步 */
	}

	/*LED_2 逻辑*/
	app_mode_t mode_now = app_mode_get();
	switch (mode_now)
	{
	case MODE_CMD:
		bsp_led_off(BSP_LED_2);
		led2_last_toggle = now;
		break;
	case MODE_FOLLOW:
		bsp_led_on(BSP_LED_2);
		led2_last_toggle = now;
		break;
	case MODE_TRACK:
		if(now - led2_last_toggle > LED_BLINK_DELAY_MS)
		{
			led2_last_toggle = now;
			bsp_led_toggle(BSP_LED_2);
		}
		break;
	default:
		break;
	}

	/* LED_3 逻辑 */
	/* 在任务一中保持熄灭
	 * 任务二开始每收到一帧完整指令翻转一次
	 * 做在 protocol.c 里了
	 */

}

void logic_led_toggle_led3(void)
{
	bsp_led_toggle(BSP_LED_3);
}

void logic_led_powerOn_selfTest(void)
{
	for (uint8_t i = 0; i < SELF_TEST_ROUNDS; i++)
	{
		bsp_led_on(BSP_LED_1);
		bsp_led_off(BSP_LED_2);
		bsp_led_off(BSP_LED_3);
		HAL_Delay(200);
		bsp_led_off(BSP_LED_1);
		bsp_led_on(BSP_LED_2);
		bsp_led_off(BSP_LED_3);
		HAL_Delay(200);
		bsp_led_off(BSP_LED_1);
		bsp_led_off(BSP_LED_2);
		bsp_led_on(BSP_LED_3);
		HAL_Delay(200);
	}
	bsp_led_off(BSP_LED_1);
	bsp_led_off(BSP_LED_2);
	bsp_led_off(BSP_LED_3);
}




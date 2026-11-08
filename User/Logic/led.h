/**
  ******************************************************************************
  * @file    led.h
  * @brief   LED的逻辑:运行状态 模式指示,
  * @author  yzcraft
  * @date	 2026-09-30
  ******************************************************************************
  */
#ifndef __LED_H
#define __LED_H

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdint.h>
/* Exported define -----------------------------------------------------------*/
/* 错误源位掩码,可同时存在多个错误(每一位独立) */
#define LED_ERR_ADC       (1U << 0)   /**< ADC / 光敏采样异常 */
#define LED_ERR_OLED      (1U << 1)   /**< OLED 初始化失败 */
#define LED_ERR_MPU6050   (1U << 2)   /**< MPU6050 初始化失败或读数异常 */
/* Exported functions prototypes ---------------------------------------------*/

/**
 * @brief	初始化LED
 * @note  	默认全部熄灭
 */
void logic_led_init(void);

/**
 * @brief	置位错误标志
 * @param	err_mask	LED_ERR_xxx 的按位或;传 0 无效果
 * @note	LED1 的闪烁由"error_flag 非 0"驱动,因此多个错误源可同时存在
 */
void logic_led_set_error(uint8_t err_mask);

/**
 * @brief	清除指定错误标志
 * @param	err_mask	LED_ERR_xxx 的按位或,清指定源
 * @note	这期神了,回来才发现初始化成功后忘清错误标志了,有点非人类.
 */
void logic_led_clear_error(uint8_t err_mask);

/**
 * @brief	执行LED的逻辑
 * @note	LED1:常亮=运行正常;1Hz闪烁=外设初始化失败或运行异常;
 * 		 	LED2:指示当前模式(模式一熄灭,模式二常亮,模式三1Hz闪烁),每次有效按键后立即更新;
 * 		 	LED3:熄灭(任务二起每收到一帧完整指令翻转一次).
 * 		 	短按,切换到下一模式(1 2 3 1)
 * 		 	长按,退回上一模式(1 3 2 1),
 */
void logic_led_task(void);

/**
 *  @brief	翻转LED3
 */
void logic_led_toggle_led3(void);

/**
 * @brief	上电自检
 * @note	三只LED以200ms间隔自左至右依次点亮,循环3轮
 */
void logic_led_powerOn_selfTest(void);





#ifdef __cplusplus
}
#endif

#endif /*__LED_H*/

/**
  ******************************************************************************
  * @file    bsp_gpio.h
  * @brief   板级 GPIO 驱动:LED,按键的底层封装
  * @author  yzcraft
  * @date	 2026-09-28
  ******************************************************************************
  */
#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

//#include "main.h"
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/

/**
 * @brief  LED 逻辑编号枚举 以便传入
 */
typedef enum{
	BSP_LED_1 =0, /*< LED1 逻辑编号 以此类推*/
	BSP_LED_2,
	BSP_LED_3
}bsp_led_t;

/**
 * @brief  按键状态逻辑编号枚举
 */
typedef enum{
	BSP_KEY_RELEASED = 0 , /**< 按键松开逻辑编号 */
	BSP_KEY_PRESSED		   /**< 按键按下逻辑编号 */
}bsp_key_state_t;

/* Exported functions prototypes ---------------------------------------------*/

/**
 * @brief  BSP GPIO 初始化
 * @note   这里只做上电默认状态设置(LED 全灭)
 */
void bsp_gpio_init (void);
/**
 * @brief 点亮指定LED
 * @param  led LED逻辑编号 可传入
 * 			BSP_LED_1
 * 			BSP_LED_2
 * 			BSP_LED_3
 */
void bsp_led_on (bsp_led_t led);
/**
 * @brief 熄灭指定LED
 * @param  led LED逻辑编号 可传入
 * 			BSP_LED_1
 * 			BSP_LED_2
 * 			BSP_LED_3
 */
void bsp_led_off (bsp_led_t led);
/**
 * @brief 翻转熄灭指定LED
 * @param  led LED逻辑编号 可传入
 * 			BSP_LED_1
 * 			BSP_LED_2
 * 			BSP_LED_3
 */
void bsp_led_toggle (bsp_led_t led);
/**
 * @brief 读取按键状态
 * @retval BSP_KEY_RELEASED  按键松开
 * @retval BSP_KEY_PRESSED   按键按下
 * @note   本函数只返回原始电平,不负责消抖与长短按判定.
 */
bsp_key_state_t bsp_key_read(void);

#ifdef __cplusplus
}
#endif


#endif /* __BSP_GPIO_H */




/**
  ******************************************************************************
  * @file    dev_oled.h
  * @brief   OLED 驱动接口声明:初始化,写命令/数据,清屏
  * @author  yzcraft
  * @date	 2026-10-03
  ******************************************************************************
  */

#ifndef __DEV_OLED_H
#define __DEV_OLED_H

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief	初始化OLED
 * @retval 	0 成功
 * @retval	1 错误
 */
uint8_t dev_oled_init(void);

/**
 * @brief	清屏
 */
void dev_oled_clear(void);

/**
 * @brief 		OLED显示字符串
 * @note		x 和 y 表示字符串在 OLED 屏幕上开始显示的左上角像素坐标
 * @param[in]	uint8_t x	水平方向(列)的像素位置,范围 0 ~ 127
 * @param[in]	uint8_t y 	垂直方向(行)的像素位置,范围 0 ~ 63.
 * @param[in]	const char *str	要显示的字符串
 */
void dev_oled_show_string(uint8_t x, uint8_t y, const char *str);

#ifdef __cplusplus
}
#endif

#endif /* __DEV_OLED_H */



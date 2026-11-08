/**
  ******************************************************************************
  * @file    oled_ui.h
  * @brief   负责组织显示内容,只做"取数据 + 调显示",不做串口,不做按键.
  * @author  yzcraft
  * @date	 2026-10-03
  ******************************************************************************
  */

#ifndef __OLED_UI_H
#define __OLED_UI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

/**
 * @brief	初始化
 * @note	失败时调用 void logic_led_set_error(uint8_t err_mask);
 */
void logic_oled_ui_init(void);

/**
 * @brief	主循环调用 刷新显示
 */
void logic_oled_ui_task(void);

/**
 * @brief		记录最近按键事件
 * @param[in]	传入 "SHORT" "LONG" "NONE"
 */
void logic_oled_ui_last_key(const char *key);

/**
 * @brief		记录最近指令
 * @param[in]	cmd  指令字符串,如 "AT","YAW 90"
 */
void logic_oled_ui_last_cmd(const char *cmd);

#ifdef __cplusplus
}
#endif

#endif /* __OLED_UI_H */

/**
  *****************************************************************************
  * @file		key.h
  * @brief		按键的逻辑:按键消抖,长短按判断
  * @author		yzcraft
  * @date		2026-09-28
  * @attention
  *****************************************************************************
  */

#ifndef __KEY_H
#define __KEY_H

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
/**
 * @brief 按键事件的类型
 */
typedef enum {
	KEY_EVENT_NONE = 0,		/**< 无事件 */
	KEY_EVENT_SHORT,		/**< 短按 */
	KEY_EVENT_LONG,			/**< 长按 */
}key_event_t;

/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief 按键逻辑初始化
 */
void logic_key_init(void);


/**
 * @brief  按键扫描任务,需每 5~10ms 调用一次
 * @note   松手时统一判定短按/长按,<50ms 视为抖动不触发
 * 		   调用前请初始化
 */
void logic_key_task(void);

/**
 * @brief  获取最近一次按键事件 读取后自动清除
 * @retval KEY_EVENT_NONE   无事件
 * @retval KEY_EVENT_SHORT  短按
 * @retval KEY_EVENT_LONG   长按
 * @note   调用前请初始化
 */
key_event_t logic_key_get_event(void);


#ifdef __cplusplus
}
#endif

#endif /*__KEY_H*/



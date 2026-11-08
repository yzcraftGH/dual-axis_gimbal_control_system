/**
  ******************************************************************************
  * @file    delay.h
  * @brief   非阻塞一次性延时
  * @author  yzcraft
  * @date    2026-09-30
  * @note    基于 HAL_GetTick(),不占用硬件定时器,不阻塞主循环.
  *          时间戳用减法比较,自动处理 uint32_t 溢出.
  * @attention	当前计时统一用 HAL_GetTick() 实现,
  * 			此模块保留作为可能的后续扩展的通用工具
  ******************************************************************************
  */
#ifndef __DELAY_H
#define __DELAY_H

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
/* Exported types ------------------------------------------------------------*/

/**
 * @brief  非阻塞延时结构体
 */
typedef struct {
	uint32_t start_tick;    /**< 启动时刻的时间戳 */
	uint32_t interval_ms;   /**< 延时长度(毫秒) */
	uint8_t  running;       /**< 1=计时中,0=已停止/未启动 */
} delay_t;
/* Exported functions prototypes ---------------------------------------------*/

/**
 * @brief  启动一次性非阻塞延时
 * @param  d            延时结构体指针
 * @param  interval_ms  延时长度(ms),必须大于 0
 * @note   如果延时正在运行,重新启动会覆盖之前的计时
 */
void delay_start(delay_t *d, uint32_t interval_ms);

/**
 * @brief  查询延时是否到期
 * @param  d  延时结构体指针
 * @retval 1  已到期(本次查询后自动停止,后续查询返回 0)
 * @retval 0  未到期 / 未启动 / 已停止
 * @note   到期后自动把 running 置 0,保证只触发一次
 */
uint8_t delay_is_expired(delay_t *d);

/**
 * @brief  查询延时是否还在计时中
 * @param  d  延时结构体指针
 * @retval 1  计时中
 * @retval 0  未启动 / 已到期
 */
uint8_t delay_is_running(delay_t *d);

/**
 * @brief  取消延时
 * @param  d  延时结构体指针
 */
void delay_cancel(delay_t *d);



#ifdef __cplusplus
}
#endif

#endif /* __DELAY_H */


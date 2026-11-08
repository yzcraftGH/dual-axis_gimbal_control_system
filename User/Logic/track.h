/**
  *****************************************************************************
  * @file		track.h
  * @brief		对自动追光的逻辑接口设计
  * @author		yzcraft
  * @date		2026-10-09
  * @attention
  *****************************************************************************
  */

#ifndef __TRACK_H
#define __TRACK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
/* Private define ------------------------------------------------------------*/
#define TRACK_SENSOR_NUM    4U /**< 光敏路数 */

/** ADC 结果数组的下标含义 */
#define TRACK_INDEX_LT   1U /**< 左上  A4 / ADC_L / PA2 / ADC1_IN2 */
#define TRACK_INDEX_RT   2U /**< 右上  A1 / ADC_U / PA3 / ADC1_IN3 */
#define TRACK_INDEX_LB   3U /**< 左下  A3 / ADC_D / PA4 / ADC1_IN4 */
#define TRACK_INDEX_RB   0U /**< 右下  A2 / ADC_R / PA1 / ADC1_IN1 */

/* Exported types ------------------------------------------------------------*/

/**
 * @brief	追光状态
 */
typedef enum{
	TRACK_STATE_NO_LIGHT,			/**< 无明显光源,保持静止 */
	TRACK_STATE_SEEKING,			/**< 向光源方向调整 */
	TRACK_STATE_ALIGNED				/**< 已正对光源 */
}track_state_t;
/* Exported functions --------------------------------------------------------*/

/**
 * @brief	追光模块初始化
 * @note	清滤波器,清状态.上电在 bsp_adc_init() 之后调用一次
 */
void logic_track_init(void);

/**
 * @brief	重新进入模式三时调用
 * @note	以当前两轴目标角为起点,避免无谓回摆;同时清滤波器与对齐计数
 */
void logic_track_reset(void);

/**
 * @brief	追光任务
 * @note	每 20ms 调用一次;内部自行判断模式,不是模式三我直接拒绝.
 */
void logic_track_task(void);

/**
 * @brief	获取当前追光状态
 * @retval	TRACK_STATE_NO_LIGHT / TRACK_STATE_SEEKING / TRACK_STATE_ALIGNED
 */
track_state_t logic_track_get_state(void);

/**
 * @brief		取最近一次滤波后的四路读数
 * @param[out]	buf	输出缓冲区,长度 >= TRACK_SENSOR_NUM
 * @note		顺序为 左 右 上 下;
 */
void logic_track_get_level(uint16_t *buf);




#ifdef __cplusplus
}
#endif

#endif /* __TRACK_H */






























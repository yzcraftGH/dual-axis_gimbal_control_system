/**
  *****************************************************************************
  * @file		filter.h
  * @brief		通用滤波算法
  * @author		yzcraft
  * @date		2026-10-05
  * @attention	时间戳由调用者传入,保持纯净
  *****************************************************************************
  */

#ifndef __FILTER_H
#define __FILTER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported define -----------------------------------------------------------*/
#define FILTER_MA_MAX_WIN   16U     /**< 滑动平均最大窗口长度 */

/* Exported types ------------------------------------------------------------*/
/**
 * @brief	互补滤波器结构体
 * @note	融合加速度计(长期稳定)和陀螺仪(短期精确)的优点
 */
typedef struct{
	float angle;		/**< 融合后的角度 (跨调用保持) */
	float coef;			/**< 互补滤波系数 (配置) */
	uint32_t last_tick;	/**< 上次更新时间 (跨调用保持) */
	uint8_t is_updated;	/**< 是否已初始化 : 0->未初始化 ; 1->已初始化 (跨调用保持)*/
}filter_comp_t;

/**
 * @brief	滑动平均滤波器
 * @note	窗口累加和用 sum 维护,每次更新为 O(1),无需遍历整个窗口
 * 			窗口未填满时按"已填样本数 cnt"取平均,开机不会从 0 缓慢爬升
 */
typedef struct{
	float	buf[FILTER_MA_MAX_WIN];	/**< 窗口内历史样本 (跨调用保持) */
	float	sum;					/**< 窗口累加和 (跨调用保持) */
	float	mean;					/**< 最近一次的平均值 (跨调用保持) */
	uint8_t	win_len;				/**< 窗口长度, 1 ~ FILTER_MA_MAX_WIN (配置) */
	uint8_t	write_pos;				/**< 环形写入位置 (跨调用保持) */
	uint8_t	count;					/**< 已填入的样本数 (跨调用保持) */
	uint8_t	is_full;				/**< 窗口是否已填满 (跨调用保持) */
}filter_ma_t;

/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief	初始化滤波器
 * @param  f      滤波器指针
 * @param  coef  互补系数,范围 [0, 1],推荐 0.98
 * @note   初始化后 is_updated = 0,首次 update 时用加速度计角度直接赋值
 */
void filter_comp_init(filter_comp_t *f, float coef);

/**
 * @brief	更新互补滤波器,返回融合后的角度
 * @param	f			滤波器指针
 * @param	acc_angle	加速度计解算的角度 (度)
 * @param	gyro_rate	陀螺仪角速度		(度/秒)
 * @param	now			传入的当前时间戳 (ms)
 * @retval	融合后的角度 (度)
 * @note    首次调用时,直接用 acc_angle 初始化 angle,不融合
 */
float filter_comp_update(filter_comp_t *f, float acc_angle , float gyro_rate, uint32_t now);

/**
 * @brief	重置滤波器
 * @param 	f	滤波器指针
 */
void filter_comp_reset(filter_comp_t *f);

/*======================滑动平均滤波器====================*/

/**
 * @brief	初始化滑动平均滤波器
 * @param	f	滤波器指针, 为空时直接返回
 * @param	win	窗口长度; 0 视为 1, 超过 FILTER_MA_MAX_WIN 按上限截断
 * @note	清零历史与累加和, 保留 win_len
 */
void filter_ma_init(filter_ma_t *f, uint8_t win_len);

/**
 * @brief	重置滑动平均滤波器
 * @param	f	滤波器指针
 * @note	清历史与已填计数, 保留 win_len; 滤波输出会从下一个样本重新开始
 */
void filter_ma_reset(filter_ma_t *f);

/**
 * @brief	更新滑动平均滤波器
 * @param	f		滤波器指针
 * @param	sample	新样本
 * @retval	当前窗口平均值; f 为空或 win_len 为 0 时原样返回 sample
 * @note	未填满期间按已填样本数取平均
 */
float filter_ma_update(filter_ma_t *f, float sample);

/**
 * @brief	查询滑动平均当前值(不更新滤波器)
 * @param	f	滤波器指针
 * @retval	最近一次的平均值; f 为空时返回 0
 */
float filter_ma_get(filter_ma_t *f);


#ifdef __cplusplus
}
#endif

#endif /* __FILTER_H */










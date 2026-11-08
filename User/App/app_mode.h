/**
  ******************************************************************************
  * @file    app_mode.h
  * @brief   三种模式切换
  * @author  yzcraft
  * @date 	 2026-09-29
  * @attention	本文件只管理模式状态,不直接操作硬件
  *
  ******************************************************************************
  */
#ifndef __APP_MODE_H
#define __APP_MODE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
typedef enum{
	MODE_CMD = 1,  	/**< 模式一 指令操控*/
	MODE_FOLLOW ,	/**< 模式二 姿态随动*/
	MODE_TRACK		/**< 模式三 自动追光*/
}app_mode_t;

/* Exported functions prototypes ---------------------------------------------*/

/**
 * @brief  上电初始化 默认模式一
 */
void app_mode_init(void);

/**
 * @brief	设置当前模式
 * @param	目标模式,可选 MODE_CMD / MODE_FOLLOW / MODE_TRACK
 */
void app_mode_set(app_mode_t mode);
/**
 * @brief	获取当前模式
 * @retval	当前模式 枚举类型
 * 				MODE_CMD
 * 				MODE_FOLLOW
 * 				MODE_TRACK
 * @note	使用前请先初始化
 */
app_mode_t app_mode_get(void);

/**
 * @brief	切换下一模式
 * @param 	传入当前模式 app_mode_t
 * @retval	返回下一模式 app_mode_t
 * @note	使用前请先初始化
 * @attention 本函数不改mode_current
 */
app_mode_t app_mode_next(app_mode_t mode_now);

/**
 * @brief	退回上一模式
 * @param	传入当前模式 app_mode_t
 * @retval	返回上一模式 app_mode_t
 * @note	使用前请先初始化
 * @attention 本函数不改mode_current
 */
app_mode_t app_mode_prev(app_mode_t mode_now);




#ifdef __cplusplus
}
#endif

#endif	/*__APP_MODE_H*/







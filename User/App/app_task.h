/**
  ******************************************************************************
  * @file    app_task.h
  * @brief   任务调度:集中调用各模块 task 函数
  * @author  yzcraft
  * @date    2026-10-01
  ******************************************************************************
  */
#ifndef __APP_TASK_H
#define __APP_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
/* Exported functions prototypes ---------------------------------------------*/

/**
 * @brief	任务调度初始化
 */
void app_task_init(void);

/**
 * @brief	任务调度主循环
 * @note	在main.c/while(1)内调用
 */
void app_task_run(void);



#ifdef __cplusplus
}
#endif

#endif /* __APP_TASK_H */



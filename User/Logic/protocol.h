/**
  ******************************************************************************
  * @file    protocol.h
  * @brief   负责解析串口指令,生成应答,执行动作,翻转 LED3,记录最近有效指令.
  * @author  yzcraft
  * @date	 2026-10-02
  ******************************************************************************
  */
#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Exported functions prototypes ---------------------------------------------*/

/**
 * @brief	初始化protocol
 */
void logic_protocol_init(void);

/**
 * @brief 	执行protocol逻辑
 */
void logic_protocol_task(void);

/**
 * @brief	获取最近有效指令 供oled_ui调用
 * @retval	最近一次有效指令字符串
 */
const char* logic_protocol_get_last_cmd(void);



#ifdef __cplusplus
}
#endif

#endif /* __PROTOCOL_H */



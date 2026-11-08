/**
  ******************************************************************************
  * @file    bsp_uart.h
  * @brief   UART串口的底层封装
  * @author  yzcraft
  * @date	 2026-10-02
  * @attention	本文件只负责 发字符串,发字节,拼成行
  ******************************************************************************
  */

#ifndef __BSP_UART_H
#define __BSP_UART_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
//#include "main.h"
#include <stdint.h>

/* Exported functions prototypes ---------------------------------------------*/

/**
 * @brief	BSP UART 初始化,启动接收中断
 */
void bsp_uart_init(void);

/**
 * @brief	UART 发送数据
 * @param	要发送的字符串 str
 */
void bsp_uart_send(const char* str);

/**
 * @brief	UART 发送单个字节
 * @param	要发送的字节
 * @note	测试用
 */
void bsp_uart_send_byte(uint8_t byte);

/**
 * @brief		从环形缓冲中取出一整行(以 '\n' 或 '\r' 结束)
 * @param[out]	buf			输出缓冲区
 * @param[in]	max_len	buf 的最大长度
 * @retval		0			没有完整行
 * @retval		1~32		实际行长度
 * @retval		33			该行超过 32 字符上限,buf 中为前 32 字符,协议层应应答 ERR
 * @note		行缓冲内部按 32 字符上限截断,超长行仍会读完再上报,不会漏字节
 */
uint8_t bsp_uart_read_line(char *buf ,uint8_t max_len);


#ifdef __cplusplus
}
#endif


#endif /* __BSP_UART_H */



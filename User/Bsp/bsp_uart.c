/**
  ******************************************************************************
  * @file    bsp_uart.c
  * @brief   UART串口的底层封装
  * @author  yzcraft
  * @date	 2026-10-02
  * @attention	本文件只负责 发字符串,发字节,拼成行
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp_uart.h"
#include "main.h"
#include "ring_buffer.h"
#include <string.h>

/* Private define ------------------------------------------------------------*/
#define UART_LINE_MAX_LEN 32U	   /**< 单帧最大字符数 */

/* Private variables ---------------------------------------------------------*/
static ring_buffer_t uart_rb;      /**< 串口接收环形缓冲 */
static uint8_t       rx_byte;      /**< 中断接收临时字节 */

/* Exported functions prototypes ---------------------------------------------*/

void bsp_uart_init(void)
{
	ring_buffer_init(&uart_rb);
	HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
}

void bsp_uart_send(const char* str)
{
	if (str == 0) {return;}
	HAL_UART_Transmit(&huart1, (uint8_t *)str, strlen(str), 100);
}

void bsp_uart_send_byte(uint8_t byte)
{
	HAL_UART_Transmit(&huart1, &byte, 1, 100);
}

uint8_t bsp_uart_read_line(char *buf ,uint8_t max_len)
{
	static char line[UART_LINE_MAX_LEN + 1];	/* 行缓冲,最后一位留给 '\0' */
	static uint8_t len_read = 0;	  /* 已读到的行长 */
	static uint8_t overflow = 0;	  /* 1=当前行超过上限,只计数不再存储 */
	uint8_t byte;

	if (buf == 0 || max_len == 0) {return 0;}

	while (ring_buffer_pop(&uart_rb, &byte))
	{
		/* 换行:一行结束  */
		if (byte == '\n' || byte == '\r')
		{
			if (len_read == 0 && overflow == 0) {
				continue;             /* 空行忽略 */
			}
			line[UART_LINE_MAX_LEN] = '\0';
			line[len_read] = '\0';        /* 在当前长度处终止字符串不带上一帧的残留(调试时加入) */
			strncpy(buf, line, max_len - 1);
			buf[max_len - 1] = '\0';

			uint8_t len = overflow ? (UART_LINE_MAX_LEN + 1) : len_read;
			len_read = 0; 						/* 重置 */
			overflow = 0;
			return len;						/* 超长时返回 33,由协议层回 ERR */
		}

		/* 非换行 */
		if (len_read < UART_LINE_MAX_LEN) 	/* 最后一位留给 '\0' */
		{
			line[len_read] = byte;
			len_read ++;
		}
		else
		{
			overflow = 1;					/* 只计数,等换行后统一上报超长 */
		}
	}
	return 0;
}

/* 中断回调:收到一个字节时触发 -------------------------------------------------------*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1) {
		ring_buffer_push(&uart_rb, rx_byte);          /* 塞入缓冲 */
		HAL_UART_Receive_IT(&huart1, &rx_byte, 1);    /* 重启接收 */
	}
}



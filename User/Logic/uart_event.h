/**
  ******************************************************************************
  * @file    uart_event.h
  * @brief   回传 KEY S, KEY L, MODE <>
  * @author  yzcraft
  * @date	 2026-10-02
  ******************************************************************************
  */

#ifndef __UART_EVENT_H
#define __UART_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "app_mode.h"

/* Exported functions prototypes ---------------------------------------------*/

/**
 * @brief	发送 "KEY S\r\n"
 */
void logic_uart_event_send_key_short(void);

/**
 * @brief	发送 "KEY L\r\n"
 */
void logic_uart_event_send_key_long(void);

/**
 * @brief	发送 "MODE<n>\r\n"
 * @param	app_mode_t :
 * 				MODE_CMD
 *				MODE_FOLLOW
 *				MODE_TRACK
 */
void logic_uart_event_send_mode(app_mode_t mode);

#ifdef __cplusplus
}
#endif

#endif /* __UART_EVENT_H */



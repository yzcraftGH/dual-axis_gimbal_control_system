/**
  ******************************************************************************
  * @file    uart_event.c
  * @brief   回传 KEY S, KEY L, MODE <>
  * @author  yzcraft
  * @date	 2026-10-02
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "uart_event.h"
#include "bsp_uart.h"
#include "app_mode.h"

/* Private variables ---------------------------------------------------------*/
const static char key_short[] = "KEY S\r\n";
const static char key_long[] = "KEY L\r\n";
const static char mode_1[] = "MODE1\r\n";
const static char mode_2[] = "MODE2\r\n";
const static char mode_3[] = "MODE3\r\n";

/* Exported functions prototypes ---------------------------------------------*/

void logic_uart_event_send_key_short(void)
{
	bsp_uart_send(key_short);
}

void logic_uart_event_send_key_long(void)
{
	bsp_uart_send(key_long);
}

void logic_uart_event_send_mode(app_mode_t mode)
{
	switch (mode) {
		case MODE_CMD:
			bsp_uart_send(mode_1);
			break;
		case MODE_FOLLOW:
			bsp_uart_send(mode_2);
			break;
		case MODE_TRACK:
			bsp_uart_send(mode_3);
			break;
		default:
			break;
	}
}



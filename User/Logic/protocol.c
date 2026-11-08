/**
  ******************************************************************************
  * @file    protocol.c
  * @brief   负责解析串口指令,生成应答,执行动作,翻转 LED3,记录最近有效指令.
  * @author  yzcraft
  * @date	 2026-10-02
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "protocol.h"
#include "bsp_uart.h"
#include "app_mode.h"
#include "led.h"
#include "gimbal.h"
#include "attitude.h"
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

/* Private variables ---------------------------------------------------------*/
static char last_valid_cmd[32];
/* Private define ------------------------------------------------------------*/
#define PROTOCOL_NO_PARAM   0xFFFF   /**< 无参数时的值 */
#define PROTOCOL_CMD_MAX_LEN 16U     /**< 命令最大长度,含 '\0' */
#define PROTOCOL_FRAME_MAX_LEN 32U	 /**< 协议单帧最大字符数,不含结束符 */
/* Private function prototypes -----------------------------------------------*/

/**
 * @brief  解析一行指令,拆分为命令和参数
 * @param  line        输入字符串,如 "YAW 90"
 * @param  line_cmd    输出缓冲区,存放命令,如 "YAW",调用者保证至少 16 字节
 * @param  line_param  输出参数,无参数时设为 PROTOCOL_NO_PARAM
 * @retval 0  解析成功
 * @retval 1  解析失败(空指针,空命令,命令超长,参数非数字,参数溢出)
 * @note   此函数只做格式解析,不判断命令是否合法,参数是否越界,
 *         命令合法性,参数范围,模式限制由 protocol_handle 负责.
 */
static uint8_t protocol_parse(const char *line, char *line_cmd, uint16_t *line_param);

/**
 * @brief		根据解析出的命令和参数,执行动作,生成应答,更新最近有效指令
 * @param[in]	const char *cmd 输入的指令
 * @param[in]	int param		指令后的参数
 */
static void protocol_handle(const char *cmd, uint16_t param);

/**
 * @brief 		发送 "OK\r\n"
 */
static void protocol_send_ok(void);

/**
 * @brief		发送 "ERR\r\n"
 */
static void protocol_send_err(void);

/**
 * @brief		发送 "OK MODE=<n>\r\n"
 */
static void protocol_send_status(void);

/* Exported functions --------------------------------------------------------*/
void logic_protocol_init(void)
{
	last_valid_cmd[0] = '\0';
}

void logic_protocol_task(void)
{
	/* 从串口取一行(内部按 32 字符上限截断,超长返回 33) */
	char line[PROTOCOL_FRAME_MAX_LEN + 1];
	uint8_t len = bsp_uart_read_line(line, sizeof(line));
	/* 没有完整行 返回 */
	if (len == 0) {return ;}
	/* 接收到完整指令翻转LED3 */
	logic_led_toggle_led3();



	if (len > PROTOCOL_FRAME_MAX_LEN)
	{
		/* 超长帧:不执行任何动作,应答 ERR */
		bsp_uart_send("ERR\r\n");
		return;
	}

	char cmd[PROTOCOL_CMD_MAX_LEN];
	uint16_t param;

	if (protocol_parse(line, cmd, &param) != 0)
	{
		/* 解析失败 */
		bsp_uart_send("ERR\r\n");
		return;
	}
	/* 执行命令,生成应答 */
	protocol_handle(cmd, param);



}

const char* logic_protocol_get_last_cmd(void)
{
	return last_valid_cmd;
}

/* Private functions ---------------------------------------------------------*/
static uint8_t protocol_parse(const char *line, char *line_cmd, uint16_t *line_param)
{
	if (line == 0 || line_cmd == 0 || line_param == 0) {return 1;}


	/* 跳过先导空格 */
	uint8_t read_pos_line =0;
	while(line[read_pos_line] == ' ')
	{
		read_pos_line++;
	}
	uint8_t cmd_start_pos = read_pos_line;
	/* 定位命令部分,并给出长度 */
	while (line[read_pos_line] != ' ' && line[read_pos_line] != '\0')
	{
		read_pos_line++;
	}
	uint8_t len_cmd = read_pos_line - cmd_start_pos;
    /* 命令长度检查 */
	if (len_cmd == 0 || len_cmd >= PROTOCOL_CMD_MAX_LEN)
	{
		return 1;
	}
	 /* 复制命令并转大写 */
	for (uint8_t i = 0; i < len_cmd; i++)
	{
		line_cmd[i] = (char)toupper((unsigned char)line[cmd_start_pos+i]);
	}
	line_cmd[len_cmd] = '\0';
	/* 跳过命令后的空格 */
	while(line[read_pos_line] == ' ')
	{
		read_pos_line ++;
	}
	/* 解析参数(如有) */
	if(line[read_pos_line] == '\0')
	{
		*line_param = PROTOCOL_NO_PARAM;
		return 0;
	}

	uint32_t value = 0;
	while (line[read_pos_line] != '\0')
	{
		/* 排除非法数字 */
		if (line[read_pos_line] < '0' || line[read_pos_line] > '9')
		{
			return 1;
		}
		/* 下一个进来的数字写在个位*/
		value = value*10 + (uint32_t)(line[read_pos_line] - '0');
		/* 防溢出(真能溢出的家里该请高人了)(逃 */
		if(value > 65535)
		{
			return 1;
		}
		read_pos_line ++;
	}
	*line_param = (uint16_t)(value);
	return 0;
}

static void protocol_handle(const char *cmd, uint16_t param)
{
	if(strcmp(cmd,"AT") == 0)
	{
		if (param != PROTOCOL_NO_PARAM) { protocol_send_err(); return; }
		protocol_send_ok();
		strcpy(last_valid_cmd,"AT");
	}
	else if (strcmp(cmd,"STATUS")== 0)
	{
		if (param != PROTOCOL_NO_PARAM) { protocol_send_err(); return; }
		protocol_send_status();
		strcpy(last_valid_cmd,"STATUS");
	}
	else if (strcmp(cmd,"YAW")== 0 || strcmp(cmd , "PITCH")== 0)
	{
		if( app_mode_get() != MODE_CMD || param > 180 || param == PROTOCOL_NO_PARAM )
		{
			protocol_send_err();
			return ;
		}
		 /* 任务三实现:调用 gimbal_set_angle() */
		if(strcmp(cmd,"YAW") == 0)
		{
			logic_gimbal_set_angle(LOGIC_GIMBAL_YAW, (uint8_t)param);
		}
		else
		{
			logic_gimbal_set_angle(LOGIC_GIMBAL_PITCH, (uint8_t)param);
		}
		protocol_send_ok();
		sprintf(last_valid_cmd,"%s %u",cmd,param);
	}
	else if (strcmp(cmd,"CAL")== 0)
	{
		if (app_mode_get() != MODE_FOLLOW || param != PROTOCOL_NO_PARAM )
		{
			protocol_send_err();
			return;
		}
		/* 执行零位校准 */

		if (logic_attitude_calibrate() == 0 ) {
			bsp_uart_send("OK\r\n");
			strcpy(last_valid_cmd, "CAL");
		}
		else
		{
			bsp_uart_send("ERRCAL\r\n");
		}
	}
	else
	{
		protocol_send_err();
	}


}
static void protocol_send_ok(void)
{
	bsp_uart_send("OK\r\n");
}

static void protocol_send_err(void)
{
	bsp_uart_send("ERR\r\n");
}

static void protocol_send_status(void)
{
	char msg[24];
	sprintf(msg,"OK MODE=%u\r\n", (uint8_t)(app_mode_get()));
	bsp_uart_send(msg);
}






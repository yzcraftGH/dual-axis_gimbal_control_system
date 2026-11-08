/**
  ******************************************************************************
  * @file    oled_ui.c
  * @brief   负责组织显示内容,做 取数据 调显示
  * @author  yzcraft
  * @date	 2026-10-03
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "oled_ui.h"
#include "main.h"
#include "led.h"
#include "dev_oled.h"
#include "app_mode.h"
#include "gimbal.h"
#include "attitude.h"
#include "track.h"
#include <string.h>
#include <stdio.h>

 /* Private define ------------------------------------------------------------*/
/* 刷新交由 app_task统一管理 */

 /* Private variables ---------------------------------------------------------*/
static char last_key[16];
static char last_cmd[16];
static uint8_t oled_ok = 0;      /**< OLED 是否可用,失败后停止刷新 */

/* 每行上次刷出的内容:内容没变就跳过整行 I2C 防止I2C被占用 */
static char oled_line_cache[4][17];

/* Exported functions --------------------------------------------------------*/

/**
 * @brief	每行显示固定 16 个字符,不够的用空格补齐.防止残留.
 */
static void oled_ui_show_line(uint8_t y, const char *str);

 /* Exported functions --------------------------------------------------------*/
void logic_oled_ui_init(void)
{
	if (dev_oled_init() == 1)
	{
		oled_ok = 0;
		logic_led_set_error(LED_ERR_OLED);
		return;
	}
	dev_oled_clear();
	memset(oled_line_cache, 0, sizeof(oled_line_cache));   /* 屏幕清空,缓存失效 */
	strcpy(last_key, "NONE");
	strcpy(last_cmd, "NONE");
	oled_ok = 1;

	logic_led_clear_error(LED_ERR_OLED);
}

void logic_oled_ui_task(void)
{
//	return ;
	if (oled_ok == 0) {return ;}
	/* 第0行 */
	oled_ui_show_line(0,"CLOUDHOUSE");
	/* 第1行 */
	/* 任务三开始将模式与按键合并为一行 */
	app_mode_t mode_now = app_mode_get();
	const char *mode_str;
	switch (mode_now) {
	case MODE_CMD:
		mode_str = "CMD";
		break;
	case MODE_FOLLOW:
		mode_str = "FOLLOW";
		break;
	case MODE_TRACK:
		mode_str = "TRACK";
		break;
	default:
		mode_str = "KSKBL";
		break;
	}
	char line1_out[32];
	snprintf(line1_out,sizeof(line1_out),"M:%-6s K:%s",mode_str,last_key);
	oled_ui_show_line(16, line1_out);
	/* 第2行 */
	char cmd_out[32];
	snprintf(cmd_out, sizeof(cmd_out), "CMD: %s", last_cmd);
	oled_ui_show_line(32,cmd_out);
	/* 第3行:两轴目标角/姿态角 */
	char line3_out[32];
	if (logic_attitude_is_ok() == 0 && app_mode_get() == MODE_FOLLOW)
	{
	    /* MPU6050 异常:优先显示错误信息 */
	    snprintf(line3_out, sizeof(line3_out), "MPU ERROR");
	}
	else if (app_mode_get() == MODE_FOLLOW)
	{
		/* 模式二:显示姿态角 */
	    snprintf(line3_out, sizeof(line3_out), "P:%+04d R:%+04d",
	             (int)logic_attitude_get_pitch(),
	             (int)logic_attitude_get_roll());
	}
	else if (app_mode_get() == MODE_TRACK)
	{
		/* 模式三:显示四路读数和追光状态 */
		uint16_t level[TRACK_SENSOR_NUM];
		const char *state;
		logic_track_get_level(level);
		switch (logic_track_get_state())
		{
		case TRACK_STATE_ALIGNED:
			state = "ON ";
			break;
		case TRACK_STATE_SEEKING:
			state = "SEEK";
			break;
		default:
			state = "OFF";
			break;
		}
		snprintf(line3_out,sizeof(line3_out),"%02u %02u %02u %02u %s",
				/* 除以42将[0,4095]压缩到两位数范围[0,97],方便显示与调试 */
				/* 按 左上 右上 左下 右下 显示 */
				(unsigned)(level[TRACK_INDEX_LT]/42U),
				(unsigned)(level[TRACK_INDEX_RT]/42U),
				(unsigned)(level[TRACK_INDEX_LB]/42U),
				(unsigned)(level[TRACK_INDEX_RB]/42U),
				state);
	}
	else
	{
		/* 模式一:显示目标角 */
	    snprintf(line3_out, sizeof(line3_out), "YAW:%03u PIT:%03u",
	             logic_gimbal_get_target_angle(LOGIC_GIMBAL_YAW),
	             logic_gimbal_get_target_angle(LOGIC_GIMBAL_PITCH));
	}
	oled_ui_show_line(48, line3_out);

}

void logic_oled_ui_last_key(const char *key)
{
	if (key == 0) {return ;}
	/* 防止溢出并手动补提示符,虽然不太可能 */
	strncpy(last_key, key, sizeof(last_key) - 1);
	last_key[sizeof(last_key) - 1] = '\0';
}

void logic_oled_ui_last_cmd(const char *cmd)
{
	if (cmd == 0) {return ;}
	strncpy(last_cmd, cmd, sizeof(last_cmd) - 1);
	last_cmd[sizeof(last_cmd) - 1] = '\0';
}

/* Private functions ---------------------------------------------------------*/
static void oled_ui_show_line(uint8_t y, const char *str)
{
	uint8_t line;
	char buf[20];
	if (str == 0 || y > 48 || (y % 16U) != 0U) {return ;}
	line = (uint8_t)(y / 16U);
	/* 左对齐,补空格到 16 字符 */
	snprintf(buf, sizeof(buf), "%-16s", str);

	/* 内容不变不刷新 */
	if ( strcmp(oled_line_cache[line],buf) == 0) {return ;}
	memcpy(oled_line_cache[line],buf,16);
	oled_line_cache[line][16] = '\0';
	dev_oled_show_string(0, y, buf);
}







/**
  ******************************************************************************
  * @file    bsp_gpio.c
  * @brief   板级 GPIO 驱动:LED,按键的底层实现
  * @author  yzcraft
  * @date 	 2026-09-28
  ******************************************************************************
  */

/* Includes-------------------------------------------------------------------*/
#include "main.h"
#include "bsp_gpio.h"

/* Private typedef -----------------------------------------------------------*/
/**
 * @brief GPIO 硬件映射表结构体
 */
typedef struct{
	GPIO_TypeDef *port; /*GPIO端口*/
	uint16_t pin;		/*GPIO引脚*/
}bsp_gpio_map_t;

/* Private define ------------------------------------------------------------*/

/**  按键 硬件引脚定义 */
#define KEY_PORT GPIOA
#define KEY_PIN GPIO_PIN_0

/* Private variables ---------------------------------------------------------*/

/**
 * @brief LED逻辑编号到引脚的映射表
 */
static const bsp_gpio_map_t led_map[] =
{
		{GPIOA , GPIO_PIN_5}, 	/**< LED1 PA5 */
		{GPIOA, GPIO_PIN_6},	/**< LED2 PA6 */
		{GPIOA, GPIO_PIN_7} 	/**< LED3 PA7 */
};

/* 用于检测参数是否越界 */
#define LED_MAP_SIZE (sizeof(led_map) / sizeof(led_map[0]))

/* Exported functions --------------------------------------------------------*/
void bsp_gpio_init (void)
{
	for (uint8_t i = 0; i < LED_MAP_SIZE; i++)
	{
		bsp_led_off((bsp_led_t)i);
	}
}

void bsp_led_on (bsp_led_t led)
{
	if ((uint8_t)led >= LED_MAP_SIZE){return;} /*越界即返回*/
	HAL_GPIO_WritePin(led_map[led].port, led_map[led].pin, GPIO_PIN_RESET);
}

void bsp_led_off (bsp_led_t led)
{
	if ((uint8_t)led >= LED_MAP_SIZE){return;}
	HAL_GPIO_WritePin(led_map[led].port, led_map[led].pin, GPIO_PIN_SET);
}

void bsp_led_toggle (bsp_led_t led)
{
	if ((uint8_t)led >= LED_MAP_SIZE){return;}
	HAL_GPIO_TogglePin(led_map[led].port, led_map[led].pin);
}

bsp_key_state_t bsp_key_read(void)
{
	if (HAL_GPIO_ReadPin(KEY_PORT, KEY_PIN) == GPIO_PIN_RESET) /* 上拉输入 低电压按下 */
	{
		return BSP_KEY_PRESSED ;
	}
	else
	{
		return BSP_KEY_RELEASED;
	}
}










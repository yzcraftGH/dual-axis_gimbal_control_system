/**
  *****************************************************************************
  * @file		bsp_adc.c
  * @brief		ADC封装,提供ADC开启,读取 接口
  * @author		yzcraft
  * @date		2026-10-08
  * @attention
  *****************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp_adc.h"
#include "main.h"
/* Private define ------------------------------------------------------------*/
#define BUF_LENGTH 4U 			/**< 缓冲区长度 */
/* Private variables ---------------------------------------------------------*/
static uint16_t adc_dma_buf[BUF_LENGTH]; /**< DMA 目标缓冲区 */

/* Exported functions --------------------------------------------------------*/

uint8_t bsp_adc_init(void)
{
	/* 上电校准 */
	if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK)
	{
		return 1;
	}

	/* 重复调用 Start_DMA 会返回 HAL_BUSY,先停一次 */
	HAL_ADC_Stop_DMA(&hadc1);

	if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_dma_buf, BUF_LENGTH) != HAL_OK)
	{
		return 1;
	}
	return 0;
}

void bsp_adc_read(uint16_t *buf)
{
	if (buf == 0) {return ;}
	for (uint8_t i = 0; i < BUF_LENGTH; i++)
	{
		buf[i] = adc_dma_buf[i];
	}
}












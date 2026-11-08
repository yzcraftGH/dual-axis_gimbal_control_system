/**
  *****************************************************************************
  * @file		bsp_adc.h
  * @brief		ADC封装,提供ADC开启,读取 接口
  * @author		yzcraft
  * @date		2026-10-08
  * @attention
  *****************************************************************************
  */
#ifndef __BSP_ADC_H
#define __BSP_ADC_H

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported functions prototypes ---------------------------------------------*/

/**
 * @brief	开启ADC DMA
 * @retval	0 - 成功; 1 - 失败
 */
uint8_t bsp_adc_init(void);

/**
 * @brief  从 DMA 缓冲区读出 4 路 ADC 原始值
 * @param  buf  输出缓冲区,长度 >= 4
 * @note   顺序 IN1~IN4
 */
void bsp_adc_read(uint16_t *buf);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_ADC_H */








/**
  ******************************************************************************
  * @file    ring_buffer.h
  * @brief   环形缓冲区,防止数据丢失
  * @author  yzcraft
  * @date	 2026-10-02
  * @attention
  *
  ******************************************************************************
  */

#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
/* Private define ------------------------------------------------------------*/
#define RING_BUFFER_SIZE 128U
/* Exported types ------------------------------------------------------------*/

/**
 * @brief	环形缓冲区结构体,储存缓冲区特征
 */
typedef struct{
	volatile uint8_t buf[RING_BUFFER_SIZE];	/**< 缓冲区 */
	volatile uint8_t head;					/**< 头 写位置 */
	volatile uint8_t tail;					/**< 尾 读位置 */
	volatile uint16_t count;				/**< 计数器 */
}ring_buffer_t;
/* Exported functions prototypes ---------------------------------------------*/

/**
 * @brief	初始化缓冲区
 * @note	清零 head tail count
 * @param	*rb		传入缓冲区指针
 */
void ring_buffer_init(ring_buffer_t * rb);

/**
 * @brief	向缓冲区中写入一个字节
 * @note	成功返回 1 ; 缓冲区满返回 0;
 * @param	*rb  	传入缓冲区指针
 * @param	byte	传入字节
 * @retval	1 -> success ; 0 -> full
 */
uint8_t ring_buffer_push(ring_buffer_t * rb , uint8_t byte);
/**
 * @brief		向缓冲区中读出一个字节
 * @note		成功返回 1 ; 缓冲区空返回 0;
 * @param[in]	*rb		传入缓冲区指针
 * @param[out]	*byte	接收字节的指针
 * @retval		1 -> success ; 0 -> empty
 */
uint8_t ring_buffer_pop(ring_buffer_t * rb , uint8_t *byte);

/**
 * @brief	查询缓冲区 当前有多少字节
 * @param	*rb		传入缓冲区指针
 * @retval	返回count
 */
uint16_t ring_buffer_count(ring_buffer_t * rb);

/**
 * @brief	清空缓冲区
 * @param	*rb		传入缓冲区指针
 */
void ring_buffer_clear(ring_buffer_t * rb);

#ifdef __cplusplus
}
#endif

#endif /* __RING_BUFFER_H */




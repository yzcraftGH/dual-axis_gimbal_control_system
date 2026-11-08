/**
  ******************************************************************************
  * @file    ring_buffer.c
  * @brief   环形缓冲区,防止数据丢失
  * @author  yzcraft
  * @date	 2026-10-02
  * @attention
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ring_buffer.h"
#include "main.h"
/* Exported functions prototypes ---------------------------------------------*/
void ring_buffer_init(ring_buffer_t * rb)
{
	if (rb == 0) {return ;}
	rb -> head = 0;
	rb -> tail = 0;
	rb -> count = 0;
}

uint8_t ring_buffer_push(ring_buffer_t * rb , uint8_t byte)
{
	uint8_t ok = 0;

	if (rb == 0) {return 0;}

	/* 关中断保护 判满/写入/head/  count++,防止被串口中断打断 */
	uint32_t primask = __get_PRIMASK();
	__disable_irq();

	if (rb->count < RING_BUFFER_SIZE)
	{
		rb->buf[rb->head] = byte;
		rb->head = ( uint8_t )(( rb->head + 1 ) % RING_BUFFER_SIZE);  /* 当head+1 == RING_BUFFER_SIZE时 head自动归零*/
		rb->count ++;
		ok = 1;
	}

	__set_PRIMASK(primask);

	return ok;
}

uint8_t ring_buffer_pop(ring_buffer_t * rb , uint8_t *byte)
{
	uint8_t ok = 0;

	if (rb == 0 || byte == 0) {return 0;}

	/* 关中断保护 判空/取字节/tail/  count--,防止被串口中断打断 */
	uint32_t primask = __get_PRIMASK();
	__disable_irq();

	if (rb->count != 0)
	{
		*byte = rb->buf[rb->tail];
		rb->tail = ( uint8_t )(( rb->tail + 1 ) % RING_BUFFER_SIZE); /* 当tail+1 == RING_BUFFER_SIZE时 tail自动归零*/
		rb->count --;
		ok = 1;
	}

	__set_PRIMASK(primask);
	/**
	 * pop 可能在其他中断里被调用,直接开__enable_irq()中断会破坏嵌套状态,
	 * 用 PRIMASK 保存恢复可以保证前后中断状态一致.
	 */
	return ok;
}

uint16_t ring_buffer_count(ring_buffer_t * rb)
{
	uint16_t cnt;

	if(rb == 0) {return 0;}

	uint32_t primask = __get_PRIMASK();
	__disable_irq();

	cnt = rb->count;

	__set_PRIMASK(primask);

	return cnt;
}

void ring_buffer_clear(ring_buffer_t * rb)
{
	if (rb == 0) {return ;}

	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	rb->count = 0;
	rb->head = 0;
	rb->tail = 0;
	__set_PRIMASK(primask);
}

/* 以上所有函数需要关中断保护,并且将返回标志移出临界区 */












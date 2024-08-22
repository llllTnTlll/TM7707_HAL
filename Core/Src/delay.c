/*
 * delay.c
 *
 *  Created on: Jan 13, 2024
 *      Author: zhiyuan
 */

#include "delay.h"

static uint32_t CLK_PER_US;
static uint32_t CLK_PER_MS;
static uint8_t didInit = 0;

#ifdef __cplusplus
extern "C"
{
#endif

	void initDelay()
	{
		if (!didInit)
		{
			HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK_DIV8);
			CLK_PER_US = SystemCoreClock / 8 / 1000000;
			CLK_PER_MS = CLK_PER_US * 1000;
			didInit = 1;
		}
	}

	void delayUSecs(uint32_t us)
	{
		if (!didInit)
		{
			initDelay();
		}

		uint32_t temp;
		SysTick->LOAD = us * CLK_PER_US;		  // 设置重装载值
		SysTick->VAL |= 0x00;					  // 将定时器归零
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; // 开启定时器

		do
		{
			temp = SysTick->CTRL;
		} while (temp & SysTick_CTRL_ENABLE_Msk && !(temp & SysTick_CTRL_COUNTFLAG_Msk));
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; // 关闭定时器
		SysTick->VAL |= 0x00;					   // 将定时器归零
	}

	void delayMSecs(uint32_t ms)
	{
		if (!didInit)
		{
			initDelay();
		}

		uint32_t temp;
		SysTick->LOAD = ms * CLK_PER_MS;
		SysTick->VAL |= 0x00;
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

		do
		{
			temp = SysTick->CTRL;
		} while (temp & SysTick_CTRL_ENABLE_Msk && !(temp & SysTick_CTRL_COUNTFLAG_Msk));
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
		SysTick->VAL |= 0x00;
	}

#ifdef __cplusplus
}
#endif

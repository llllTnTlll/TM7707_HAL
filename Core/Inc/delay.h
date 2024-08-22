/*
 * delay.h
 *
 *  Created on: Jan 13, 2024
 *      Author: zhiyuan
 */

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

#include <stm32f1xx_hal.h>

#ifdef __cplusplus
extern "C" {
#endif

void initDelay();
void delayUSecs(uint32_t us);
void delayMSecs(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* INC_DELAY_H_ */

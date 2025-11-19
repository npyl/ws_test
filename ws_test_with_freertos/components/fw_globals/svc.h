/*
 * svc.h
 *
 *  Created on: Nov 14, 2014
 *      Author: a0406448
 */

#ifndef SVC_H_
#define SVC_H_

#include "HL_sys_common.h"

#pragma SWI_ALIAS(unimplementedSVC,     0);
#pragma SWI_ALIAS(switchCpuMode,        1);
#pragma SWI_ALIAS(switchToSystemMode,   2);
#pragma SWI_ALIAS(switchToUserMode,     3);
#pragma SWI_ALIAS(writePrivRegister32, 33);
#pragma SWI_ALIAS(writePrivRegister8,  34);
#pragma SWI_ALIAS(testReentrantSVC,    35);

void     unimplementedSVC(void); /* Used to test fault handler */
uint32_t switchCpuMode(uint32_t u32ModeNum);
void     switchToSystemMode(void);
void     switchToUserMode(void);

/* The following SVC functions are implemented at C level */
uint32_t writePrivRegister32(uint32_t * pu32Address, uint32_t u32Value);
uint8_t  writePrivRegister8(uint8_t * pu8Address, uint8_t u8Value);
uint32_t testReentrantSVC(uint32_t * pu32Address, uint32_t u32Repetition);

#endif /* SVC_H_ */

/*
 * svc.c
 *
 *  Created on: Nov 14, 2014
 *      Author: a0406448
 */

#include "HL_sys_common.h"
#include "svc.h"

#ifdef DEBUG
#include <stdio.h>
#endif

extern uint32_t c_svc(uint32_t u32Param1, uint32_t u32Param2, uint32_t u32Handler);

static void testReentrantSvcInThumb(uint32_t * pu32Param1, uint32_t u32Param2)
{
	if (0ul != u32Param2)
	{
		testReentrantSVC(pu32Param1 + 1, u32Param2 - 1ul); /* Call function recursively */

		writePrivRegister32(pu32Param1, (uint32_t)pu32Param1);
	}

	return;
}

uint32_t c_svc(uint32_t u32Param1, uint32_t u32Param2, uint32_t u32Handler)
{
	uint32_t u32ReturnVal = 0ul;

#ifdef DEBUG
	printf("Called C Level SVC Handler #%u\n", u32Handler);
#endif

	switch(u32Handler)
	{
	case 33: /* writePrivRegister32() */
		u32ReturnVal           = *(uint32_t *)u32Param1;
		*(uint32_t *)u32Param1 = u32Param2;
		break;

	case 34: /* writePrivRegister8() */
		u32ReturnVal          = (uint32_t)(*(uint8_t *)u32Param1);
		*(uint8_t *)u32Param1 = (uint8_t)u32Param2;
		break;

	case 35: /* testReentrantSVC */
		testReentrantSvcInThumb((uint32_t *)u32Param1, u32Param2);
		break;

	default:
		ASSERT(0);
		break;
	}

	return u32ReturnVal;
}

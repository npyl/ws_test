/*----------------------------------------------------------------------------\
|          Copyright(c) 2024 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : tsk_c0_50ms_task.c Module File.                            |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 7 …·Ì 2024                                                 |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Application 50ms task Module                                              |
|                                                                             |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Compiler Controls                                                         |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

#include <stdio.h>

#include "tsk_c0_50ms_task.h"
#include "FreeRTOS.h"
#include "os_task.h"
#include "taskParams.h"
#include "coreParams.h"
#include "setup.h"
#include "trace.h"

/*----------------------------------------------------------------------------\
|   Public Constant Definitions                                               |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Public Data Definitions                                                   |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Private Type Definitions                                                  |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Private Constant Definitions                                              |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Private Data Definitions                                                  |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Private Function Declarations                                             |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Public Function Implementations                                           |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|                                                                             |
|    Procedure         :  task_C0_50MS_init                                   |
|                                                                             |
|    Description       :  Function to initialize Core 0 - 50ms task           |
|                                                                             |
|    Inputs            :  none.                                               |
|                                                                             |
|    Outputs           :  none.                                               |
|                                                                             |
|    Return            :  none.                                               |
|                                                                             |
|    Warnings          :  none.                                               |
|                                                                             |
\----------------------------------------------------------------------------*/

void task_C0_50MS_init( void )
{
	;
}

/*----------------------------------------------------------------------------\
|                                                                             |
|    Procedure         :  task_C0_50MS                                        |
|                                                                             |
|    Description       :  Core 0 - 50ms task                                  |
|                                                                             |
|    Inputs            :  Pointer to task's parameters.                       |
|                                                                             |
|    Outputs           :  none.                                               |
|                                                                             |
|    Return            :  none.                                               |
|                                                                             |
|    Warnings          :  none.                                               |
|                                                                             |
\----------------------------------------------------------------------------*/

void task_C0_50MS( void *params )
{
	portBaseType ok;
	S_SERIAL_TRACE_INFO SerialTraceInfo;
	S_TASKPROC_DATA	*procdata	= ( S_TASKPROC_DATA* ) params;

    tskInitTaskProcData( procdata );        /* Initialize task process data */
    tskInitTraceInfo( procdata, &SerialTraceInfo );   /* Initialize task's constant serial trace info */

	for ( ;; )
	{
#if 0
		/*
         * !!! Caution !!!
         * No point sending serial debug info to gatekeeper task that runs every 100ms.
		 * This action will cause queue overflow.
		 */
		ok = xQueueSend( procdata->xSerialTraceHandle, &SerialTraceInfo, 0 );
		/* Examine reason for unsuccessful send, eg. Queue Full ( ok = -4 ) */
		( ( ok == pdPASS ) ? ( { __asm volatile ( " nop" ); } ) : ( printf( "task_C0_50MS - Serial Debug Queue Error: %d\r\n", ok ) ) );
#endif

		/* Call the application task */
		app_task_50ms();

	    /* Update task process data */
		ok = tskUpdateTaskProcData( procdata );
		//?? ( ( ok == pdPASS ) ? ( { __asm volatile ( " nop" ); } ) : ( printf( "Task overrun: 50ms\r\n" ) ) );

#if 0
		printf( "C0 50ms task callcount: %d\r\n", procdata->callcount );
#endif

		/* Block until next run for the configured period */
		vTaskDelayUntil( ( TickType_t * ) &( procdata->starttime ), ( TickType_t ) procdata->period );
	}
}

/*----------------------------------------------------------------------------\
|   Private Function Implementations                                          |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   End of tsk_C0_50ms_task.c Module                                          |
\----------------------------------------------------------------------------*/

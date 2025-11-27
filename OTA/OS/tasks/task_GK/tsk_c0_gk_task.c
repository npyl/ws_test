/*----------------------------------------------------------------------------\
|          Copyright(c) 2024 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : tsk_c0_gk_task.c Module File.                              |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 7 …·Ì 2024                                                 |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Application tsk_c0_gk_task.c Module                                       |
|                                                                             |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Compiler Controls                                                         |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "tsk_c0_gk_task.h"
#include "global.h"
#include "FreeRTOS.h"
#include "os_task.h"
#include "coreParams.h"
#include "taskParams.h"
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
|    Procedure         :  task_C0_GK_init                                     |
|                                                                             |
|    Description       :  Function to initialize Core 0 - GK Task             |
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

void task_C0_GK_init( void )
{
	xSerialMessage[ eCORE_0 ].name[ TASK_NAME_LENGTH_MAX ] = '\0';
}

/*----------------------------------------------------------------------------\
|                                                                             |
|    Procedure         :  task_C0_GK                                          |
|                                                                             |
|    Description       :  Core 0 - GK Task                                    |
|                                                                             |
|    Inputs            :  Pointer to task's parameters.                       |
|                                                                             |
|    Outputs           :  none.                                               |
|                                                                              |
|    Return            :  none.                                               |
|                                                                             |
|    Warnings          :  none.                                               |
|                                                                             |
\----------------------------------------------------------------------------*/

void task_C0_GK( void *params )
{
	portBaseType ok;
	S_SERIAL_TRACE_INFO selfSerialTraceInfo, SerialTraceInfo;
	S_TASKPROC_DATA	*procdata	= ( S_TASKPROC_DATA* ) params;

    tskInitTaskProcData( procdata );             			/* Initialize task process data: start time and state */
    tskInitTraceInfo( procdata, &selfSerialTraceInfo );		/* Initialize task's constant serial trace info */

	for ( ;; )
	{
		ok = xQueueReceive( procdata->xSerialTraceHandle, &SerialTraceInfo, 0 );
		// ( ( ok == pdPASS ) ? ( { memcpy( &xSerialMessage[ procdata->coreid ], &SerialTraceInfo, sizeof( S_SERIAL_TRACE_INFO ) ); } ) : ( { __asm volatile ( " nop" ); } ) );

		/* Update task process data */
		ok = tskUpdateTaskProcData( procdata );
		//?? ( ( ok == pdPASS ) ? ( { __asm volatile ( " nop" ); } ) : ( printf( "Task overrun: task_C0_GK\r\n" ) ) );

		/* Block until next run for the configured period */
		vTaskDelayUntil( ( TickType_t * ) &( procdata->starttime ), ( TickType_t ) procdata->period );
	}
}

/*----------------------------------------------------------------------------\
|   Private Function Implementations                                          |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   End of tsk_c0_gk_task.c module                                            |
\----------------------------------------------------------------------------*/

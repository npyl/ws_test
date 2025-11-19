/*----------------------------------------------------------------------------\
|          Copyright(c) 2022 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : setup.h Header File.                                       |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 2022                                                       |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Application 50ms task Module                                              |
|                                                                             |
\----------------------------------------------------------------------------*/

#ifndef SETUP_H
#define SETUP_H

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

#include "FreeRTOS.h"
#include "os_portmacro.h"
#include "coreParams.h"
#include "trace.h"
#include "global.h"

/*----------------------------------------------------------------------------\
|   Public Type Declarations                                                  |
\----------------------------------------------------------------------------*/

#define TASK_STACK_SIZE_C0 ( 2048UL )       /* Size of stack for Core 0 Tasks */

/*----------------------------------------------------------------------------\
|   Public Constant Declarations                                              |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Public Data Declarations                                                  |
\----------------------------------------------------------------------------*/

extern portTaskHandleType AppIdleTaskHandle;
extern S_CORE_INFO CoreInfo[ NUMBER_OF_CORES ];
extern xQueueHandle xSerialTraceHandle;
extern S_SERIAL_TRACE_INFO xSerialMessage[ NUMBER_OF_CORES ];
extern xQueueHandle xEnetHandle;

/*----------------------------------------------------------------------------\
|   Public Function Declarations                                              |
\----------------------------------------------------------------------------*/

void tskInitCoreInfo( void );
void tskInitCoreStack( U8 coreid );
void tskInitProcData( void );
void tskInitTraceInfo( S_TASKPROC_DATA *procdata, S_SERIAL_TRACE_INFO *serinfo );
portBaseType tskCreateCoreTasks( U8 coreid );
void tskInitTaskProcData ( S_TASKPROC_DATA *procdata );
U32 tskCalcTaskUsedTime( S_TASKPROC_DATA *procdata );
portBaseType tskUpdateTaskProcData( S_TASKPROC_DATA *procdata );

/*----------------------------------------------------------------------------\
|   End of Tasks Setup Header File                                            |
\----------------------------------------------------------------------------*/

#endif /* SETUP_H */

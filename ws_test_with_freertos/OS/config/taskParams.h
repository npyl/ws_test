/*----------------------------------------------------------------------------\
|          Copyright(c) 2024 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : taskParams.h Header File.                                  |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 7 …·Ì 2024                                                 |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   <Module Header Description>                                               |
|                                                                             |
\----------------------------------------------------------------------------*/

#ifndef taskParams_H
#define taskParams_H

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

#include "FreeRTOS.h"
#include "os_queue.h"
#include "os_task.h"
#include "fw_types.h"
#include "taskIDs.h"

/*----------------------------------------------------------------------------\
|   Private Constant Declarations                                             |
\----------------------------------------------------------------------------*/

#define TASK_NAME_LENGTH_MAX    32u
#define TOPIC_LENGTH            15u
#define PAYLOAD_LENGTH          25u

/*----------------------------------------------------------------------------\
|   Public Type Declarations                                                  |
\----------------------------------------------------------------------------*/

/*
 * Function pointer types
 */
typedef pdTASK_CODE T_TASKMAINPROC;         /* Task's function */
typedef void ( *T_TASKINITPROC )( void );   /* An initializing function (At task creation time) if required */

/*
 * Fixed information about a single task
 */
typedef struct
{
    E_TASKID        taskid;
    T_TASKMAINPROC  mainproc;
    T_TASKINITPROC  initproc;
    U32             stacksize;                          /* Size of stack area [bytes] */
    CHAR            name[ TASK_NAME_LENGTH_MAX + 1 ];   /* Not used by FreeRTOS but for application specific purposes */
} S_TASK_CONFIG;

/*
 * Parameters for a single task used during application setup.
 * These should match the TaskParamsList.
 */
typedef struct
{
    U8              coreid;                 /* A task can be specific to a core */
    E_TASKID        taskid;
    BOOLEAN         enabled;
    U32             period;                 /* [ms] */
    U8              priority;               /* Lower value = higher priority */

    /* User defined:
     */
    QueueHandle_t   xSerialTraceHandle;     /* Handle to serial trace buffer for debugging */
} S_TASK_PARAMS;

/*
 * Contains parameters and runtime data for a particular task
 */
typedef struct
{
    /* Setup params */
    E_TASKID            taskid;
    CHAR                name[ TASK_NAME_LENGTH_MAX + 1];
    U8                  coreid;
    U8                  priority;
    BOOLEAN             taskvalid;      /* TRUE if task for this data is initialised */
    U32                 period;         /* Time between calls [ms] */

    /* Runtime data */
    TaskHandle_t        htask;
    U32                 starttime;      /* Tick count at start of last run */
    S32                 error;          /* Last error (eg return from xTaskCreate) */

    /* For test & debug */
    U32                 callcount;      /* Incremented on every call (rolls) */
    U8                  state;
    U32                 loopstarttime;  /* Tick count at start of loop (to calculate CPU time) */
    U64                 proctime;       /* Total time used for processing [ms] */

    /* User defined:
     */
    /* Trace Queue Handle */
    QueueHandle_t       xSerialTraceHandle;     /* Handle to serial trace buffer for debugging */
} S_TASKPROC_DATA;

/*----------------------------------------------------------------------------\
|   Public Constant Declarations                                              |
\----------------------------------------------------------------------------*/

/*
 * Task states
 */
#define TASK_STATE_NONE         0
#define TASK_STATE_CREATEOK     1
#define TASK_STATE_RUNINIT      2
#define TASK_STATE_RUNPROC      3
#define TASK_STATE_FAIL         99

/*----------------------------------------------------------------------------\
|   Public Data Declarations                                                  |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Public Function Declarations                                              |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   End of taskParams.h header file                                           |
\----------------------------------------------------------------------------*/

#endif  /* taskParams_H */

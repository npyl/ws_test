/*----------------------------------------------------------------------------\
|          Copyright(c) 2024 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : setup.c Module File.                                       |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 7 …·Ì 2024                                                 |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Application FreeRTOS tasks setup Module                                   |
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

#include "fw_types.h"
#include "FreeRTOS.h"
#include "os_task.h"
#include "coreParams.h"
#include "taskParams.h"
#include "trace.h"
#include "setup.h"
#include "global.h"

/* Header files of all tasks
 */
#include "tsk_c0_2ms_task.h"
#include "tsk_c0_10ms_task.h"
#include "tsk_c0_25ms_task.h"
#include "tsk_c0_50ms_task.h"
#include "tsk_c0_100ms_task.h"
#include "tsk_c0_500ms_task.h"
#include "tsk_c0_1000ms_task.h"
#include "tsk_c0_2000ms_task.h"
#include "tsk_c0_5000ms_task.h"
#include "tsk_c0_gk_task.h"
//#include "tsk_uart_gk.h"
//#include "tsk_i2c_gk.h"
//#include "tsk_spi_gk.h"

/*----------------------------------------------------------------------------\
|   Public Constant Definitions                                               |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Public Data Definitions                                                   |
\----------------------------------------------------------------------------*/

portInt8Type Core_0_Task_Stack[ TASK_STACK_SIZE_C0 ];	/* Total Core 0 stack size */
S_CORE_INFO	CoreInfo[ NUMBER_OF_CORES ];				/* Information regarding the cores */
xQueueHandle xSerialTraceHandle;                        /* Serial debug queue handle */
S_SERIAL_TRACE_INFO xSerialMessage[ NUMBER_OF_CORES ];  /* Serial debug message contents of a task */

/*----------------------------------------------------------------------------\
|   Private Type Definitions                                                  |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Private Constant Definitions                                              |
\----------------------------------------------------------------------------*/

static S_CORE_DATA Core_0_Data;							/* Runtime system data of a core */

static const S_CORE_CONFIG CoreConfigList [] =
{
	/*
	 * Cores configuration data!
	 * Keep ordered (Ascending) by coreid!
	 */
	/* coreid	coredata		corestack								stacksize */
	{  eCORE_0,	&Core_0_Data,	( portInt8Type * ) &Core_0_Task_Stack,	sizeof( Core_0_Task_Stack ) },     /* Core 0 configuration data */
};

static const S_TASK_CONFIG TaskConfigList[] =
{
	/*
	 * Task configuration data!
	 * Configuration data required for every task in TaskParamsList.
	 *     Indexed by taskid!
	 * Use NULL if no initproc specified!
	 * Note: Must match S_TASK_CONFIG struct.
	 */
	/* taskid			 	mainproc			initproc		   		stacksize	name[20] */
    {  E_TASKID_C0_GK,      task_C0_GK,         task_C0_GK_init,        128UL,      "C0 GK task" },         /* Application serial debug queue Gatekeeper task */
	{  E_TASKID_C0_2MS,	 	task_C0_2MS,		task_C0_2MS_init,		128UL,		"C0 2ms Task" },		/* 2ms task */
	{  E_TASKID_C0_10MS,	task_C0_10MS,		task_C0_10MS_init,		128UL,		"C0 10ms Task" },		/* 10ms task */
	{  E_TASKID_C0_25MS,	task_C0_25MS,		task_C0_25MS_init,		128UL,		"C0 25ms Task" },		/* 25ms task */
	{  E_TASKID_C0_50MS,	task_C0_50MS,		task_C0_50MS_init,		128UL,		"C0 50ms Task" },		/* 50ms task */
	{  E_TASKID_C0_100MS,	task_C0_100MS,		task_C0_100MS_init,		128UL,		"C0 100ms Task" },		/* 100ms task */
	{  E_TASKID_C0_500MS,	task_C0_500MS,		task_C0_500MS_init,		128UL,		"C0 500ms Task" },		/* 500ms task */
	{  E_TASKID_C0_1000MS,	task_C0_1000MS,		task_C0_1000MS_init,	128UL,		"C0 1000ms Task" },		/* 1000ms task */
	{  E_TASKID_C0_2000MS,	task_C0_2000MS,		task_C0_2000MS_init,	128UL,		"C0 2000ms Task" },		/* 2000ms task */
	{  E_TASKID_C0_5000MS,	task_C0_5000MS,		task_C0_5000MS_init,	128UL,		"C0 5000ms Task" },		/* 5000ms task */
//    {  E_TASKID_C0_UART_GK, task_C0_uart_gk,    task_C0_uart_gk_init,   128UL,      "C0 UART GK Task" },    /* UART interfaces Gatekeeper task */
//    {  E_TASKID_C0_I2C_GK,  task_C0_i2c_gk,     task_C0_i2c_gk_init,    128UL,      "C0 I2C GK Task" },     /* I2C interfaces Gatekeeper task */
//    {  E_TASKID_C0_SPI_GK,  task_C0_spi_gk,     task_C0_spi_gk_init,    128UL,      "C0 SPI GK Task" },     /* SPI interfaces Gatekeeper task */
};

#define TaskConfigCount sizeof( TaskConfigList ) / sizeof( TaskConfigList[ 0 ] )

static const S_TASK_PARAMS TaskParamsList[] =
{
	/*
	 * Initial task parameters passed at task creation!
	 * Notes:
	 *   Must match S_TASK_PARAMS struct.
	 *     tskIDLE_PRIORITY = 0
	 *     configMAX_PRIORITIES = 10.  Hence priorities from 0 .. 9
	 *     priority: Low number = Low priority
	 */
	/*                                                                   				|-- User Defined Task Encapsulated Variables --------> */
	/* coreid      taskid				enabled		period      priority				Serial trace handle */
	{  eCORE_0,    E_TASKID_C0_GK,		TRUE,       100u,		tskIDLE_PRIORITY + 7,	( QueueHandle_t ) &xSerialTraceHandle, },
	{  eCORE_0,    E_TASKID_C0_2MS,		TRUE,		2u,			tskIDLE_PRIORITY + 9,	( QueueHandle_t ) &xSerialTraceHandle, },
	{  eCORE_0,    E_TASKID_C0_10MS,	TRUE,		10u,		tskIDLE_PRIORITY + 9,	( QueueHandle_t ) &xSerialTraceHandle, },
	{  eCORE_0,    E_TASKID_C0_25MS,	TRUE,		25u,		tskIDLE_PRIORITY + 8,	( QueueHandle_t ) &xSerialTraceHandle, },
	{  eCORE_0,    E_TASKID_C0_50MS,	TRUE,		50u,		tskIDLE_PRIORITY + 8,	( QueueHandle_t ) &xSerialTraceHandle, },
	{  eCORE_0,    E_TASKID_C0_100MS,	TRUE,		100u,		tskIDLE_PRIORITY + 7,	( QueueHandle_t ) &xSerialTraceHandle, },
	{  eCORE_0,    E_TASKID_C0_500MS,	TRUE,		500u,		tskIDLE_PRIORITY + 6,	( QueueHandle_t ) &xSerialTraceHandle, },
	{  eCORE_0,    E_TASKID_C0_1000MS,	TRUE,		1000u,		tskIDLE_PRIORITY + 6,	( QueueHandle_t ) &xSerialTraceHandle, },
	{  eCORE_0,    E_TASKID_C0_2000MS,	TRUE,		2000u,		tskIDLE_PRIORITY + 2,	( QueueHandle_t ) &xSerialTraceHandle, },
	{  eCORE_0,    E_TASKID_C0_5000MS,	TRUE,		5000u,		tskIDLE_PRIORITY + 1,	( QueueHandle_t ) &xSerialTraceHandle, },
    {  eCORE_0,    E_TASKID_C0_UART_GK, TRUE,       100u,       tskIDLE_PRIORITY + 7,   ( QueueHandle_t ) &xSerialTraceHandle, },
    {  eCORE_0,    E_TASKID_C0_I2C_GK,  TRUE,       100u,       tskIDLE_PRIORITY + 7,   ( QueueHandle_t ) &xSerialTraceHandle, },
    {  eCORE_0,    E_TASKID_C0_SPI_GK,  TRUE,       100u,       tskIDLE_PRIORITY + 7,   ( QueueHandle_t ) &xSerialTraceHandle, },
};

static const U8 TaskParamsCount = ( U8 ) ( sizeof( TaskParamsList ) / sizeof( TaskParamsList[ 0 ] ) );

/*----------------------------------------------------------------------------\
|   Private Data Definitions                                                  |
\----------------------------------------------------------------------------*/

static S_TASKPROC_DATA ProcData[ TaskConfigCount ];		/* Parameters and runtime data of each task */

/*----------------------------------------------------------------------------\
|   Private Function Declarations                                             |
\----------------------------------------------------------------------------*/

static BOOLEAN tskTaskStackAlloc( S_CORE_DATA *coredata, U32 stacksize, StackType_t **stack );
static const S_TASK_CONFIG * tskGetTaskConfig( E_TASKID taskid );

/*----------------------------------------------------------------------------\
|   Public Function Implementations                                           |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|                                                                             |
|    Procedure         :  tskInitCoreInfo                                     |
|                                                                             |
|    Description       :  Initialize information relating to the cores:       |
|                           struct S_CORE_INFO:                               |
|                             corestate                                       |
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

void tskInitCoreInfo( void )
{
	memset( &CoreInfo, 0, sizeof( CoreInfo ) );
}

/*----------------------------------------------------------------------------\
|                                                                             |
|    Procedure         :  tskInitCoreStack                                    |
|                                                                             |
|    Description       :  Initialize Core's Stack Area.                       |
|                                                                             |
|    Inputs            :  Core ID.                                            |
|                                                                             |
|    Outputs           :  none.                                               |
|                                                                             |
|    Return            :  none.                                               |
|                                                                             |
|    Warnings          :  none.                                               |
|                                                                             |
\----------------------------------------------------------------------------*/

void tskInitCoreStack( U8 coreid )
{
	S_CORE_CONFIG const *corecfg = &CoreConfigList[ coreid ];
	S_CORE_DATA *coredata = NULL;
	portInt8Type *stack = NULL;
	U32 stacksize = 0;

	if ( NULL != corecfg )
	{
		coredata = corecfg->coredata;
		stack = corecfg->stack;
		stacksize = corecfg->stacksize;
	}

	if ( NULL != coredata )
	{
		memset( coredata, 0, sizeof( S_CORE_DATA ) );
		coredata->stack = stack;
		coredata->stacksize	= stacksize;
	}
}

/*----------------------------------------------------------------------------\
|                                                                             |
|    Procedure         :  tskInitProcData                                     |
|                                                                             |
|    Description       :  Initialize Task's Process Data:                     |
|                           struct S_TASKPROC_DATA:                           |
|                       	  === Setup params ===                            |
|                             E_TASKID			taskid;                       |
|							  CHAR				name[ TASK_NAME_LENGTH_MAX ]; |
|							  U8				coreid;                       |
|							  U8				priority;                     |
|							  BOOLEAN			taskvalid; TRUE if task for   |
|                                                          this data          |
|                                                          is initialized     |
|							  U32				period;	   Time between       |
|                                                          calls [ms]         |
|                            === Runtime data ===                             |
|                            portTaskHandleType	htask;     Task handle        |
|														   (returned from OS) |
|							 U32				starttime; Tick count at start|
|                                                          of last run        |
|                            S32				error;     Last error (eg.    |
|                                                          return from        |
|                                                          xTaskCreate)       |
|                            === For test & debug ===                         |
|                            U32				callcount; Incremented        |
|                                                          on every call      |
|                                                          (rolls)            |
|                           U8					state;                        |
|                           U32					loopstarttime; Tick count at  |
|                                                              start of loop  |
|                                                              (to calculate  |
|                                                              CPU time)      |
|                           U64					proctime;  Total time used for|
|                                                          processing [ms]    |
|                           === User defined ===                              |
|                           eg. U8 led_status                                 |
|							=== Trace Queue Handle ===                        |
|							xQueueHandle		xSerialTraceHandle; Handle to |
|                                                                   serial    |
|                                                                   trace     |
|                                                                   buffer for|
|                                                                   debugging |
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

void tskInitProcData( void )
{
	memset( &ProcData, 0, sizeof( ProcData ) );
}

/*----------------------------------------------------------------------------\
|                                                                             |
|    Procedure         :  tskCreateCoreTasks                                  |
|                                                                             |
|    Description       :  Function to create Core tasks.                      |
|                                                                             |
|    Inputs            :  Core ID.                                            |
|                                                                             |
|    Outputs           :  none.                                               |
|                                                                             |
|    Return            :  pdPASS if successful,                               |
|                         pdFAIL if unsuccessful                              |
|                                                                             |
|    Warnings          :  none.                                               |
|                                                                             |
\----------------------------------------------------------------------------*/

portBaseType tskCreateCoreTasks( U8 coreid )
{
    U8 i;

	/* Function result variables */
	portBaseType create_task_result = pdFAIL;
	BOOLEAN create_core_tasks_result = FALSE;

	/* Core configuration */
	const S_CORE_CONFIG *ptr_core_config_list = &CoreConfigList[ coreid ];

	/* Task configuration and parameters */
	const S_TASK_PARAMS *ptr_task_params_list = NULL;
	const S_TASK_CONFIG *ptr_task_config_list = NULL;
	S_TASKPROC_DATA *ptr_task_proc_data = NULL;
	xQueueHandle serial_queue_handle = NULL;

	StackType_t *stack = NULL;

	/* FreeRTOS task variables */
	xTaskParameters os_task_params = {};

	/* For all tasks of all cores */
	for ( i = 0; i < TaskParamsCount; i++ )
	{
		/* Check which tasks are for own core and that these tasks are enabled to run */
		if ( ( coreid == TaskParamsList[ i ].coreid ) && ( TaskParamsList[ i ].enabled == TRUE ) )
		{
			/* Locate task's process data area */
			ptr_task_proc_data = &ProcData[ i ];

			/* Get task's parameters */
			ptr_task_params_list = &TaskParamsList[ i ];

			/* Get task's configuration based on its E_TASKID */
			ptr_task_config_list = tskGetTaskConfig( ptr_task_params_list->taskid );

			/* Try to create task but only if it has configuration data */
			if ( NULL != ptr_task_config_list )
			{
				/* Create task if can allocate stack for it */
				if ( TRUE == tskTaskStackAlloc( ptr_core_config_list->coredata, ptr_task_config_list->stacksize, &stack ) )
				{
					/* Initialize FreeRTOS task parameters */
					memset( &os_task_params, 0, sizeof( os_task_params) );

					/* Prepare FreeRTOS task parameters */
					os_task_params.pvTaskCode = ptr_task_config_list->mainproc;
					os_task_params.usStackDepth = ptr_task_config_list->stacksize;
					os_task_params.pvParameters = ptr_task_proc_data;
					os_task_params.uxPriority = ptr_task_params_list->priority;
					os_task_params.puxStackBuffer = ( StackType_t * ) stack;

					/* Initialize task process data from task config and params lists */
					memcpy( ptr_task_proc_data->name, ptr_task_config_list->name, TASK_NAME_LENGTH_MAX );
					ptr_task_proc_data->taskid = ptr_task_params_list->taskid;
					ptr_task_proc_data->priority = ptr_task_params_list->priority;
					ptr_task_proc_data->coreid = ptr_task_params_list->coreid;
					ptr_task_proc_data->period = ptr_task_params_list->period;

					/* =====================================================================
					 *
					 * Assign Application Specific Parameters now
					 *
					 * ===================================================================== */

					/* Serial Debug trace handle */
					if ( NULL != ptr_task_params_list->xSerialTraceHandle )
					{
						memcpy( &serial_queue_handle, ptr_task_params_list->xSerialTraceHandle, sizeof( xQueueHandle ) );
						ptr_task_proc_data->xSerialTraceHandle = serial_queue_handle;
					}

					/* More Application Specific Parameters here
					 *
					 * ...
					 *
					 */

					/* =====================================================================
					 *
					 * End of Application Specific Parameters
					 *
					 * ===================================================================== */

					/* Signal that process data has been assigned to the task */
					ptr_task_proc_data->taskvalid = TRUE;

#if 0
				    if ( ptr_task_params_list->taskid == E_TASKID_C0_2000MS )
				    {
				        // Dynamically allocate stack memory
				        StackType_t *pxStack = ( StackType_t * ) pvPortMalloc(os_task_params.usStackDepth * sizeof( StackType_t ) );

				        if ( pxStack != NULL )
				        {
				            // Create a TaskParameters_t structure using initialization
				            const TaskParameters_t xRestrictedTaskParams =
				            {
				                os_task_params.pvTaskCode,         /* pvTaskCode */
				                ptr_task_config_list->name,        /* pcName */
				                os_task_params.usStackDepth,       /* usStackDepth */
				                os_task_params.pvParameters,       /* pvParameters */
				                os_task_params.uxPriority,         /* uxPriority */
				                pxStack,                           /* puxStackBuffer */

				                /* xRegions - Set all to 0 if no specific protection is needed */
				                {
				                    /* Base address   Length              Parameters */
				                    { 0,              0,                  0 },
				                    { 0,              0,                  0 },
				                    { 0,              0,                  0 }
				                }
				            };

				            // Create the task with restricted parameters
				            create_task_result = xTaskCreateRestricted( &xRestrictedTaskParams, &(ptr_task_proc_data->htask ) );

				            // Note: Do not free pxStack! It will be used by the task.
				            // The memory will be automatically freed when the task is deleted.
				        }
				        else
				        {
				            // Handle memory allocation failure
				            create_task_result = pdFAIL;
				        }
				    }
					else
					{
                        /* Try creating the task */
                        create_task_result = xTaskCreate( os_task_params.pvTaskCode,
                                                          ptr_task_config_list->name,
                                                          os_task_params.usStackDepth,
                                                          os_task_params.pvParameters,
                                                          os_task_params.uxPriority,
                                                          &( ptr_task_proc_data->htask ) );
					}
#else
                    create_task_result = xTaskCreate( os_task_params.pvTaskCode,
                                                      ptr_task_config_list->name,
                                                      os_task_params.usStackDepth,
                                                      os_task_params.pvParameters,
                                                      os_task_params.uxPriority,
                                                      &( ptr_task_proc_data->htask ) );
#endif

					/* Check the result of xTaskCreate */
					create_task_result == pdPASS ? ( create_core_tasks_result = TRUE ) : ( ptr_task_proc_data->error = create_task_result );
					if ( TRUE == create_core_tasks_result )
					{
						// printf( "Successfully created task: %s\r\n", ptr_task_config_list->name );
					}
					else
					{
						// printf( "Unsuccessfully created task: %s.  Error: %d\r\n", ptr_task_config_list->name, ptr_task_proc_data->error );
					}

					/* Run task's init function if defined ( Note: Task must have been created ) */
					if ( ( TRUE == create_core_tasks_result ) && ( NULL != ptr_task_config_list->initproc ) )
					{
						// printf( "Calling init function of task: %s\r\n", ptr_task_config_list->name );
						( ptr_task_config_list->initproc )();
					}
				}
			}
		}
	}

	return create_core_tasks_result;
}

/*----------------------------------------------------------------------------\
|                                                                             |
|    Procedure         :  tskInitTaskProcData                                 |
|                                                                             |
|    Description       :  Function to initialize the task's process data:     |
|                             starttime,                                      |
|                             state                                           |
|                                                                             |
|    Inputs            :  Pointer to the task's process data.                 |
|                                                                             |
|    Outputs           :  none.                                               |
|                                                                             |
|    Return            :  none.                                               |
|                                                                             |
|    Warnings          :  none.                                               |
|                                                                             |
\----------------------------------------------------------------------------*/

void tskInitTaskProcData( S_TASKPROC_DATA *procdata )
{
    procdata->starttime	= xTaskGetTickCount();
	procdata->state = TASK_STATE_RUNINIT;
}

/*----------------------------------------------------------------------------\
|                                                                             |
|    Procedure         :  tskInitTraceInfo                                    |
|                                                                             |
|    Description       :  Function to initialize the task's constant serial   |
|                         trace information:                                  |
|                           Task name                                         |
|                                                                             |
|    Inputs            :  Pointer to the task's process data.                 |
|                         Pointer to the task's serial trace message.         |
|                                                                             |
|    Outputs           :  none.                                               |
|                                                                             |
|    Return            :  none.                                               |
|                                                                             |
|    Warnings          :  none.                                               |
|                                                                             |
\----------------------------------------------------------------------------*/

void tskInitTraceInfo( S_TASKPROC_DATA *procdata, S_SERIAL_TRACE_INFO *serinfo )
{
	memcpy( serinfo->name, procdata->name, sizeof( procdata->name ) );
}

/*----------------------------------------------------------------------------\
|                                                                             |
|    Procedure         :  tskCalcTaskUsedTime                                 |
|                                                                             |
|    Description       :  Function to calculate the task's usage time.        |
|                                                                             |
|    Inputs            :  Pointer to the task's process data.                 |
|                                                                             |
|    Outputs           :  none.                                               |
|                                                                             |
|    Return            :  The time used.                                      |
|                                                                             |
|    Warnings          :  none.                                               |
|                                                                             |
\----------------------------------------------------------------------------*/

U32 tskCalcTaskUsedTime( S_TASKPROC_DATA *procdata )
{
	U32 timeused = 0;
	U32 timenow = xTaskGetTickCount();

	if ( timenow >= procdata->starttime )
	{
		timeused = timenow - procdata->starttime;
	}
	else
	{
		timeused = timenow + ( 0xFFFFFFFF - procdata->starttime );
	}

	return timeused;
}

/*----------------------------------------------------------------------------\
|                                                                             |
|    Procedure         :  tskUpdateTaskProcData                               |
|                                                                             |
|    Description       :  Function to update the task's process data and to   |
|                         check that there was no over-run.                   |
|                                                                             |
|    Inputs            :  Pointer to the task's process data.                 |
|                                                                             |
|    Outputs           :  none.                                               |
|                                                                             |
|    Return            :  pdPASS if successful,                               |
|                         pdFAIL if task overrun.                             |
|                                                                             |
|    Warnings          :  none.                                               |
|                                                                             |
\----------------------------------------------------------------------------*/

portBaseType tskUpdateTaskProcData( S_TASKPROC_DATA *procdata )
{
	portBaseType rslt = pdPASS;
	U32 timeused;

	procdata->state = TASK_STATE_RUNPROC;
	procdata->callcount++;
	timeused = tskCalcTaskUsedTime( procdata );
	procdata->proctime += timeused;
	if ( timeused > procdata->period )
	{
		rslt = pdFAIL;
	}
	return rslt;
}

/*----------------------------------------------------------------------------\
|   Private Function Implementations                                          |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|                                                                             |
|    Procedure         :  tskTaskStackAlloc                                   |
|                                                                             |
|    Description       :  Function to allocate Task Stack within Core Stack.  |
|                                                                             |
|    Inputs            :  Pointer to core's task stack allocation variables   |
|                         Task's stack requirements                           |
|                         Pointer to the address in Core Stack for Task Stack |
|                             allocation.                                     |
|                                                                             |
|    Outputs           :  none.                                               |
|                                                                             |
|    Return            :  TRUE if successful,                                 |
|                         FALSE if not successful.                            |
|                                                                             |
|    Warnings          :  none.                                               |
|                                                                             |
\----------------------------------------------------------------------------*/

static BOOLEAN tskTaskStackAlloc( S_CORE_DATA *coredata, U32 stacksize, StackType_t **stack )
{
	BOOLEAN	rslt = FALSE;

	if ( ( coredata->nextavail + stacksize ) <= coredata->stacksize )
	{
		/* Get pointer to start & update next available */
		*stack = (StackType_t *) &( coredata->stack[ coredata->nextavail ] );
		coredata->nextavail += stacksize;

		/* Make sure next available value is 8-byte aligned (OS requirement) */
		if ( ( coredata->nextavail % 8 ) != 0 )
		{
			coredata->nextavail  += ( 8 - ( coredata->nextavail % 8 ) );
		}
		rslt = TRUE;
	}
	return rslt;
}

/*----------------------------------------------------------------------------\
|                                                                             |
|    Procedure         :  tskGetTaskConfig                                    |
|                                                                             |
|    Description       :  Function to find the task's configuration data from |
|                         TaskConfigList.                                     |
|                                                                             |
|    Inputs            :  The task id.                                        |
|                                                                             |
|    Outputs           :  none.                                               |
|                                                                             |
|    Return            :  const S_TASK_CONFIG *                               |
|                                                                             |
|    Warnings          :  none.                                               |
|                                                                             |
\----------------------------------------------------------------------------*/

static const S_TASK_CONFIG *tskGetTaskConfig( E_TASKID taskid )
{
    U8 i;
	const S_TASK_CONFIG *ptr_task_config_list = NULL;

	for ( i = 0; i < TaskConfigCount; i++ )
	{
		if ( taskid == TaskConfigList[ i ].taskid )
		{
			ptr_task_config_list = &TaskConfigList[ i ];
			break;
		}
	}

	return ptr_task_config_list;
}

/*----------------------------------------------------------------------------\
|   End of setup.c module                                                     |
\----------------------------------------------------------------------------*/

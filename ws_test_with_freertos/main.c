#include "HL_sys_common.h"
#include "HL_system.h"
#include "HL_adc.h"
#include "HL_gio.h"

#include "main.h"
#include "app_task_2ms.h"
#include "app_task_10ms.h"
#include "app_task_25ms.h"
#include "app_task_50ms.h"
#include "app_task_100ms.h"
#include "app_task_500ms.h"
#include "app_task_1000ms.h"
#include "app_task_2000ms.h"
#include "app_task_5000ms.h"
#include "data_manager.h"
#include "fw_adc.h"
#include "fw_dio.h"
#include "fw_gio_dmm.h"
#include "fw_gio_het.h"
#include "setup.h"

#include "fw_gio_dmm.h"
#include "fw_gio_het.h"
#include "fw_gio_mibspi.h"

/**
 * main.c
 */
int main( void ) {
    U8 coreid = eCORE_0; /* Designate core that we are running on */
    portBaseType free_rtos_ok = pdFAIL; /* Defensively assume OS is down */

    dmDataManagerInit();

    /* CAUTION:
     *   Call to dioHandlerInit must precede GIO initializations
     */
    dioHandlerInit();

    // printf( "Initializing Digital Inputs\n" );
    dioHandlerInitInputPins();

    // printf( "Initializing Digital Outputs\n" );
    dioHandlerInitOutputPins();

    /* Execute application's tasks initializations */
    app_task_2ms_init();
    app_task_10ms_init();
    app_task_25ms_init();
    app_task_50ms_init();
    app_task_100ms_init();
    app_task_500ms_init();
    app_task_1000ms_init();
    app_task_2000ms_init();
    app_task_5000ms_init();

    /* FreeRTOS initialization and execution section */
    // printf( "Initializing core information\n" );                     /* Core's info initialization */
    tskInitCoreInfo();

    // printf( "Initializing process data of all FreeRTOS tasks\n" );   /* Tasks process data initialization */
    tskInitProcData();

    // printf( "Core's scheduler state: MAIN START\n" );                /* Update core scheduler state: "In main" */
    CoreInfo [ coreid ].corestate = CORE_STATE_MAINSTART;

    // printf( "Initializing the core's stack for FreeRTOS use\n" );    /* Initialize the core's stack for FreeRTOS use */
    tskInitCoreStack( coreid );

    // printf( "Core's scheduler state: INITIALIZED\n" );               /* Core's FreeRTOS scheduler initialized */
    CoreInfo [ coreid ].corestate = CORE_STATE_SCHEDOK;

    // TODO: serial queue

    // printf( "Creating core's FreeRTOS tasks!\r\n" );              /* Create the core tasks for FreeRTOS */
    free_rtos_ok = tskCreateCoreTasks( coreid );

    if ( pdPASS == free_rtos_ok ) /* Successfully created tasks */
    {
        CoreInfo [ coreid ].corestate = CORE_STATE_APPOK; /* Core Scheduler will be put in running state */

        // printf("Core's scheduler state: RUNNING\n");                 /* Start the FreeRTOS scheduler */
        vTaskStartScheduler(); /* Should never return from this call */

        // printf("Core's scheduler state: FAILED\n");                  /* Core Scheduler failed to enter running state */
        CoreInfo [ coreid ].corestate = CORE_STATE_SCHEDFAIL;
    }

    /* Should never reach this point
     */

    /* Enter an infinite loop */
    while ( 1 ) {
        /* 'Dummy' NOP to allow source level single stepping of tight while() loop */
        asm ( " nop" );
    }
}

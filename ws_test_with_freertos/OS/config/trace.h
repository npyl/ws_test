/*----------------------------------------------------------------------------\
|          Copyright(c) 2024 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : trace.h Header File.                                       |
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

#ifndef trace_H
#define trace_H

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

#include "os_portmacro.h"

#include "fw_globals.h"
#include "taskParams.h"

/*----------------------------------------------------------------------------\
|   Public Constant Definitions                                               |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Public Type Definitions                                                   |
\----------------------------------------------------------------------------*/

/*
 * Serial trace message contents for all tasks
 * We can add whatever we want in this struct
 */
typedef struct
{
    CHAR name[ TASK_NAME_LENGTH_MAX + 1 ];
} S_SERIAL_TRACE_INFO;


/*----------------------------------------------------------------------------\
|   Public Constant Declarations                                              |
\----------------------------------------------------------------------------*/

#define CRLF ( "\n\r" )
#define LFCR ( "\r\n" )

#define SERIAL_TRACE_QUEUE_LENGTH ( 5u )                                 /* The number of queue elements (records) */
#define SERIAL_TRACE_QUEUE_ITEM_SIZE sizeof( S_SERIAL_TRACE_INFO )       /* The size in bytes of each element (record) */
#define SERIAL_TRACE_QUEUE_SIZE ( ( SERIAL_TRACE_QUEUE_LENGTH * SERIAL_TRACE_QUEUE_ITEM_SIZE ) + portQUEUE_OVERHEAD_BYTES ) /* Total size of queue in bytes */

/*----------------------------------------------------------------------------\
|   Public Function Declarations                                              |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   End of trace.h header file                                                |
\----------------------------------------------------------------------------*/

#endif  /* trace_H */

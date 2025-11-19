/*----------------------------------------------------------------------------\
|          Copyright(c) 2024 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : coreParams.h Header File.                                  |
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

#ifndef coreParams_H
#define coreParams_H

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

#include "fw_types.h"

/*----------------------------------------------------------------------------\
|   Public Type Declarations                                                  |
\----------------------------------------------------------------------------*/

/*
 * Contains runtime system data for a particular core
 */
typedef struct
{
    /*
     * Stack allocation
     */
    portInt8Type    *stack;         /* Pointer to stack area */
    U32             stacksize;      /* Size of stack area */
    U32             nextavail;      /* Next available stack index */
} S_CORE_DATA;

/*
 * Fixed information about a single core
 */
typedef struct
{
    U8              coreid;             /* For multicore support */
    S_CORE_DATA     *coredata;          /* Stack allocation */
    portInt8Type    *stack;             /* Pointer to stack area */
    U32             stacksize;          /* Size of stack area */
} S_CORE_CONFIG;

/*
 * Contains information and debug data for a particular core
 */
typedef struct
{
    U8              corestate;      /* For startup debugging */
} S_CORE_INFO;

/*----------------------------------------------------------------------------\
|   Public Constant Declarations                                              |
\----------------------------------------------------------------------------*/

/*
 * Core states
 */
#define CORE_STATE_NONE         0
#define CORE_STATE_MAINSTART    1
#define CORE_STATE_SCHEDOK      2
#define CORE_STATE_APPOK        3
#define CORE_STATE_SCHEDFAIL    99

/*----------------------------------------------------------------------------\
|   Public Data Declarations                                                  |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Public Function Declarations                                              |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   End of coreParams.h header file                                           |
\----------------------------------------------------------------------------*/

#endif  /* coreParams_H */

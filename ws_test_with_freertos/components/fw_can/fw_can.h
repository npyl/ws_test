/*----------------------------------------------------------------------------\
|          Copyright(c) 2024 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : fw_can.h Header File.                                      |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 27 ÷Â‚ 2024                                                |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   <Module Header Description>                                               |
|                                                                             |
\----------------------------------------------------------------------------*/

#ifndef fw_can_H
#define fw_can_H

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

#include "HL_can.h"

#include "fw_types.h"

/*----------------------------------------------------------------------------\
|   Public Constant Declarations                                              |
\----------------------------------------------------------------------------*/

#define CAN_PAYLOAD_SIZE        32u     /* Bytes */

#define CAN( x )                ( ( ( x ) == eCAN_0 ) ? canREG1 : ( ( x ) == eCAN_1 ) ? canREG2 : ( ( x ) == eCAN_2 ) ? canREG3 : canREG4 )
#define CAN_IDX( x )            ( ( ( x ) == canREG1 ) ? eCAN_0 : ( ( x ) == canREG2 ) ? eCAN_1 : ( ( x ) == canREG3 ) ? eCAN_2 : eCAN_3 )

#define CAN_QUEUE_LENGTH ( 5u )                                                                     /* The number of queue elements (records) */
#define CAN_QUEUE_ITEM_SIZE sizeof( S_CAN_INFO )                                                    /* The size in bytes of each element (record) */
#define CAN_QUEUE_SIZE ( ( CAN_QUEUE_LENGTH * CAN_QUEUE_ITEM_SIZE ) + portQUEUE_OVERHEAD_BYTES )    /* Total size of queue in bytes */

/*----------------------------------------------------------------------------\
|   Public Data Declarations                                                  |
\----------------------------------------------------------------------------*/

extern xQueueHandle xCANQueueHandle;

/*----------------------------------------------------------------------------\
|   Public Type Definitions                                                   |
\----------------------------------------------------------------------------*/

typedef enum
{
    eCAN_0 = 0,
    eCAN_1,
    eCAN_2,
    eCAN_3,
    eCAN_MAX,
} E_CAN_ID;

typedef struct
{
    E_CAN_ID    id;
    canBASE_t   *can;


    BOOLEAN     enabled;
} S_CAN_CONFIG;

typedef struct
{
    E_CAN_ID        id;
    canBASE_t *     can;
    BYTE            payload[ CAN_PAYLOAD_SIZE ];
} S_CAN_INFO;

/*----------------------------------------------------------------------------\
|   Public Function Declarations                                              |
\----------------------------------------------------------------------------*/

void can_init( void );
const S_CAN_CONFIG * const canGetConfig( void );

/*----------------------------------------------------------------------------\
|   End of fw_can.h header file                                               |
\----------------------------------------------------------------------------*/

#endif  /* fw_can_H */

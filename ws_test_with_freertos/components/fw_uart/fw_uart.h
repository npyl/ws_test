/*----------------------------------------------------------------------------\
|          Copyright(c) 2024 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : fw_uart.h Header File.                                     |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 16 …·Ì 2024                                                |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   <Module Header Description>                                               |
|                                                                             |
\----------------------------------------------------------------------------*/

#ifndef fw_uart_H
#define fw_uart_H

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

#include "HL_sci.h"

#include "FreeRTOS.h"
#include "os_queue.h"

#include "fw_types.h"
#include "fw_globals.h"

/*----------------------------------------------------------------------------\
|   Public Constant Definitions                                               |
\----------------------------------------------------------------------------*/

#define UART_PAYLOAD_SIZE       100u                /* Bytes */
#define CR                      0x0A
#define LF                      0x0D
#define STX                     0x02
#define ETX                     0x03
#define EOP                     ( ETX )             /* End Of Packet character */
#define SCI_IDLE_INT            ( 0x00000800U )     /* Halcogen doesn't generate this in HL_sci.h */
#define SCI_TIMEOUT             10000

#define UART( x )               ( ( ( x ) == eUART_0 ) ? sciREG1 : ( ( x ) == eUART_1 ) ? sciREG2 : ( ( x ) == eUART_2 ) ? sciREG3 : sciREG4 )
#define UART_IDX( x )           ( ( ( x ) == sciREG1 ) ? eUART_0 : ( ( x ) == sciREG2 ) ? eUART_1 : ( ( x ) == sciREG3 ) ? eUART_2 : eUART_3 )

/*----------------------------------------------------------------------------\
|   Public Type Definitions                                                   |
\----------------------------------------------------------------------------*/

/* Notes:
 *   Index: 0:SCI1, 1:SCI2, 2:SCI3, 3:SCI4
 *   Will use SCI3 and SCI4, index 2 and 3 correspondingly
 */
typedef enum
{
    eUART_0 = 0u,
    eUART_1,
    eUART_2,
    eUART_3,
    eUART_MAX,
} E_UART_ID;

/* Note: Baud rate determines the prescaler value
 */
typedef enum
{
    eBAUD_9600 = 487U,
    eBAUD_14400 = 325U,
    eBAUD_19200 = 243U,
    eBAUD_28800 = 162U,
    eBAUD_33600 = 139U,
    eBAUD_38400 = 121U,
    eBAUD_57600 = 80U,
    eBAUD_115200 = 40U,
    eBAUD_MAX,
} E_BAUD_RATE;

typedef enum
{
    eSTOP_ONE = 0u,
    eSTOP_TWO,
    eSTOP_MAX,
} E_STOP_BITS;

typedef struct
{
    E_BAUD_RATE     baud;
    E_STOP_BITS     stop;
    BOOLEAN         parity_en;
    BOOLEAN         parity_even;
    BOOLEAN         loopback;
} S_UART_PARAMS;

typedef struct
{
    E_UART_ID       id;
    CHAR            label[ 32u ];
    S_UART_PARAMS   params;
    U8              txd_pin;
    U8              rxd_pin;
    BOOLEAN         enabled;
} S_UART_CONFIG;

/* UART communication info structure
 * We can add whatever we want in this struct
 */
typedef struct __attribute__ ( ( packed ) )
{
    E_UART_ID       id;
    sciBASE_t *     sci;
    U16             payload_length;
    BYTE            payload[ UART_PAYLOAD_SIZE ];
} S_UART_INFO;

/*----------------------------------------------------------------------------\
|   Public Constant Declarations                                              |
\----------------------------------------------------------------------------*/

#define UART_QUEUE_LENGTH ( 5u )                                                                    /* The number of queue elements (records) */
#define UART_QUEUE_ITEM_SIZE sizeof( S_UART_INFO )                                                  /* The size in bytes of each element (record) */
#define UART_QUEUE_SIZE ( ( UART_QUEUE_LENGTH * UART_QUEUE_ITEM_SIZE ) + portQUEUE_OVERHEAD_BYTES ) /* Total size of queue in bytes */

/*----------------------------------------------------------------------------\
|   Public Data Declarations                                                  |
\----------------------------------------------------------------------------*/

extern xQueueHandle xUARTQueueHandle;

/*----------------------------------------------------------------------------\
|   Public Function Declarations                                              |
\----------------------------------------------------------------------------*/

void uart_init( void );
const S_UART_CONFIG * const uartGetConfig( void );

/*----------------------------------------------------------------------------\
|   End of fw_uart.h header file                                              |
\----------------------------------------------------------------------------*/

#endif  /* fw_uart_H */

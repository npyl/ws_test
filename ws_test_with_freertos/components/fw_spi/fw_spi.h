/*----------------------------------------------------------------------------\
|          Copyright(c) 2024 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : fw_spi.h Header File.                                      |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 11 …·Ì 2024                                                |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   <Module Header Description>                                               |
|                                                                             |
\----------------------------------------------------------------------------*/

#ifndef fw_spi_H
#define fw_spi_H

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

#include "HL_spi.h"

#include "FreeRTOS.h"
#include "os_queue.h"

#include "fw_types.h"
#include "fw_globals.h"

/*----------------------------------------------------------------------------\
|   Public Constant Definitions                                               |
\----------------------------------------------------------------------------*/

#define SPI_PAYLOAD_SIZE       128u    /* Bytes */

#define SPI( x )               ( ( ( x ) == eSPI_0 ) ? spiREG1 : ( ( x ) == eSPI_1 ) ? spiREG2 : ( ( x ) == eSPI_2 ) ? spiREG3 : ( ( x ) == eSPI_3 ) ? spiREG4 : spiREG5 )
#define SPI_IDX( x )           ( ( ( x ) == spiREG1 ) ? eSPI_0 : ( ( x ) == spiREG2 ) ? eSPI_1 : ( ( x ) == spiREG3 ) ? eSPI_2 : ( ( x ) == spiREG4 ) ? eSPI_3 : eSPI_4 )

/*----------------------------------------------------------------------------\
|   Public Type Definitions                                                   |
\----------------------------------------------------------------------------*/

typedef enum
{
    e8_BIT = 0,
    e16_BIT,
    e32_BIT,
    eBIT_MAX,
} E_CHAR_LEN;

typedef enum
{
    eSPI_0 = 0,
    eSPI_1,
    eSPI_2,
    eSPI_3,
    eSPI_4,
    eSPI_MAX,
} E_SPI_ID;

typedef enum
{
    eSPI_FORMAT_0 = 0,
    eSPI_FORMAT_1,
    eSPI_FORMAT_2,
    eSPI_FORMAT_3,
    eSPI_FORMAT_MAX,
} E_SPI_DATA_FORMAT;

typedef enum
{
    e1000_kHz = 74u,
    e900_kHz = 82u,
    e800_kHz = 93u,
    e700_kHz = 106u,
    e600_kHz = 124u,
    e500_kHz = 149u,
    e400_kHz = 187u,
    e300_kHz = 249u,
    e_BAUD_MAX,
} E_SPI_BAUD;

typedef struct
{
    uint32      baud_prescale;  /* kHz */
    uint8       w_delay;        /* Wait between transmissions */
    E_CHAR_LEN  char_len;       /* Length of SPI transmission word */
    BOOLEAN     LSB_first;      /* Shift direction */
    BOOLEAN     parity_en;      /* Parity enable */
    BOOLEAN     clk_polarity;   /* When set, signal is high-inactive */
    BOOLEAN     wait_for_en;    /* Wait for enable signal before transmitting */
    BOOLEAN     odd_parity;     /* Even parity when not set */
    BOOLEAN     clk_phase;      /* When set, signal is delayed by half clock period */
} S_SPI_DATA_FORMAT;

typedef struct
{
    uint8       cs_to_tx_start;                     /* 0 = 26.667ns, 1 = 40.000ns, 2 = 53.333ns,... */
    uint8       tx_end_to_cs_inactive;              /* 0 = 13.333ns, 1 = 26.667ns,... */
    uint8       cs_to_ena_timeout;                  /* 0 = 0ns, 1 = 1ns,... */
    uint8       tx_finish_to_ena_inactive_timeout;  /* 0 = 0ns, 1 = 1ns,... */
} S_SPI_DELAYS;

typedef struct
{
    E_SPI_ID            id;
    CHAR                label[ 32u ];
    S_SPI_DATA_FORMAT   format[ eSPI_FORMAT_MAX ];
    E_SPI_DATA_FORMAT   selected_format;
    S_SPI_DELAYS        delay;
    BOOLEAN             loopback;
    BOOLEAN             enabled;
} S_SPI_CONFIG;

/* SPI communication info structure
 * We can add whatever we want in this struct
 */
typedef struct __attribute__ ( ( packed ) )
{
    E_SPI_ID        id;
    spiBASE_t *     spi;
    U16             payload_length;
    BYTE            payload[ SPI_PAYLOAD_SIZE ];
} S_SPI_INFO;

/*----------------------------------------------------------------------------\
|   Public Constant Declarations                                              |
\----------------------------------------------------------------------------*/

#define SPI_QUEUE_LENGTH ( 5u )                                                                     /* The number of queue elements (records) */
#define SPI_QUEUE_ITEM_SIZE sizeof( S_SPI_INFO )                                                    /* The size in bytes of each element (record) */
#define SPI_QUEUE_SIZE ( ( SPI_QUEUE_LENGTH * SPI_QUEUE_ITEM_SIZE ) + portQUEUE_OVERHEAD_BYTES )    /* Total size of queue in bytes */

/*----------------------------------------------------------------------------\
|   Public Data Declarations                                                  |
\----------------------------------------------------------------------------*/

extern xQueueHandle xSPIQueueHandle;

/*----------------------------------------------------------------------------\
|   Public Function Declarations                                              |
\----------------------------------------------------------------------------*/

void spi_init( void );
const S_SPI_CONFIG * const spiGetConfig( void );

/*----------------------------------------------------------------------------\
|   End of fw_spi.h header file                                               |
\----------------------------------------------------------------------------*/

#endif  /* fw_spi_H */

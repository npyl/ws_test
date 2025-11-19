/*----------------------------------------------------------------------------\
|          Copyright(c) 2024 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : fw_i2c.h Header File.                                      |
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

#ifndef fw_i2c_H
#define fw_i2c_H

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

#include "HL_i2c.h"

#include "FreeRTOS.h"
#include "os_queue.h"

#include "fw_types.h"
#include "fw_globals.h"

/*----------------------------------------------------------------------------\
|   Public Type Declarations                                                  |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Public Constant Declarations                                              |
\----------------------------------------------------------------------------*/

#define NO_OF_I2C_DEVICES       4u      /* The max number of I2C devices per I2C channel */
#define I2C_PAYLOAD_SIZE        32u     /* Bytes */

#define I2C( x )                ( ( ( x ) == eI2C_0 ) ? i2cREG1 : i2cREG2 )
#define I2C_IDX( x )            ( ( ( x ) == i2cREG1 ) ? eI2C_0 : eI2C_1 )
#define I2C_OWN_ADDR( x )       ( ( ( x ) == eI2C_0 ) ? 0x20 : 0x21 )

#define I2C_TIMEOUT             1000000U  /* TODO This needs tweaking */

/*----------------------------------------------------------------------------\
|   Public Type Definitions                                                   |
\----------------------------------------------------------------------------*/

typedef enum
{
    eI2C_0 = 0,
    eI2C_1,
    eI2C_MAX,
} E_I2C_ID;

typedef enum
{
    eI2C1_LNBH25L = 0,
    eI2C2_DEV_0 = 0,
    eI2C1_DEV_1 = 1,
    eI2C2_DEV_1 = 1,
    eI2C1_DEV_2 = 2,
    eI2C2_DEV_2 = 2,
    eI2C1_DEV_3 = 3,
    eI2C2_DEV_3 = 3,
    eI2C1_DEV_MAX,
} E_I2C_DEV_ID;

typedef enum
{
    I2C_2_BITS  = 0x2U,
    I2C_3_BITS  = 0x3U,
    I2C_4_BITS  = 0x4U,
    I2C_5_BITS  = 0x5U,
    I2C_6_BITS  = 0x6U,
    I2C_7_BITS  = 0x7U,
    I2C_8_BITS  = 0x0U
} E_I2C_BIT_COUNT;

typedef enum
{
    eADDR_7_BIT = 0x0000U,
    eADDR_10_BIT = 0x0100U,
} E_I2C_ADDR_MODE;

typedef enum
{
    eTRANNSMITTER = 0x0200U,
    eRECEIVER = 0x0000U,
} E_TX_RX;

typedef struct
{
    BOOLEAN         master;
    E_TX_RX         tx_rx;
    E_I2C_ADDR_MODE addr_mode;
    E_I2C_BIT_COUNT bit_count;
} S_I2C_FORMAT;

typedef struct
{
    E_I2C_DEV_ID    id;
    CHAR            label[ 32u ];
    U8              slave_addr;
    U8              id_reg;
} S_I2C_DEVICE_ID;

typedef struct
{
    E_I2C_ID        id;
    S_I2C_DEVICE_ID device[ NO_OF_I2C_DEVICES ];
    S_I2C_FORMAT    format;
    U32             baudrate;
    U32             prescale;
    U32             ckh;
    U32             ckl;
    BOOLEAN         loopback;
    BOOLEAN         enabled;
} S_I2C_CONFIG;

/* I2C communication info structure
 * We can add whatever we want in this struct
 */
typedef struct __attribute__ ( ( packed ) )
{
    E_I2C_ID        id;
    i2cBASE_t *     i2c;
    E_I2C_DEV_ID    dev_id;
    BYTE            payload[ I2C_PAYLOAD_SIZE ];
} S_I2C_INFO;

/*----------------------------------------------------------------------------\
|   Public Constant Declarations                                              |
\----------------------------------------------------------------------------*/

#define I2C_QUEUE_LENGTH ( 5u )                                                                     /* The number of queue elements (records) */
#define I2C_QUEUE_ITEM_SIZE sizeof( S_I2C_INFO )                                                    /* The size in bytes of each element (record) */
#define I2C_QUEUE_SIZE ( ( I2C_QUEUE_LENGTH * I2C_QUEUE_ITEM_SIZE ) + portQUEUE_OVERHEAD_BYTES )    /* Total size of queue in bytes */

/*----------------------------------------------------------------------------\
|   Public Data Declarations                                                  |
\----------------------------------------------------------------------------*/

extern xQueueHandle xI2CQueueHandle;

/*----------------------------------------------------------------------------\
|   Public Function Declarations                                              |
\----------------------------------------------------------------------------*/

void i2c_init( void );
const S_I2C_CONFIG * const i2cGetConfig( void );
bool i2c_write( E_I2C_ID id, E_I2C_DEV_ID dev_id, uint8_t reg_addr, uint8_t *data, uint8_t data_len );
bool i2c_read( E_I2C_ID id, E_I2C_DEV_ID dev_id, uint8_t reg_addr, uint8_t *data, uint8_t data_len );

/*----------------------------------------------------------------------------\
|   End of fw_i2c.h header file                                               |
\----------------------------------------------------------------------------*/

#endif  /* fw_i2c_H */

/*----------------------------------------------------------------------------\
|          Copyright(c) 2024 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : fw_test.c Module File.                                     |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 24 …·Ì 2024                                                |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   <Module Description>                                                      |
|                                                                             |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Compiler Controls                                                         |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "HL_can.h"
#include "HL_sci.h"

#include "fw_globals.h"
#include "fw_dio.h"
#include "fw_adc.h"
#include "fw_uart.h"
#include "fw_spi.h"
#include "fw_i2c.h"
#include "fw_can.h"
#include "bme280.h"
#include "fw_test.h"

/*----------------------------------------------------------------------------\
|   Public Constant Definitions                                               |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Public Data Definitions                                                   |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Private Type Definitions                                                  |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Private Constant Definitions                                              |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Private Data Declarations                                                 |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Private Function Declarations                                             |
\----------------------------------------------------------------------------*/

static BaseType_t test_dio( void );
static BaseType_t test_adc( void );
static BaseType_t test_uart( void );
static BaseType_t test_spi( void );
static BaseType_t test_i2c( void );
static BaseType_t test_can( void );

/*----------------------------------------------------------------------------\
|   Public Function Implementations                                           |
\----------------------------------------------------------------------------*/

BaseType_t fw_test( void )
{
    BaseType_t ok = pdFAIL;

    // ok = test_dio();
    // ok = test_adc();
    // ok = test_uart();
    // ok = test_spi();
    // ok = test_i2c();
    // ok = test_can();

    return ok;
}

/*----------------------------------------------------------------------------\
|   Private Function Implementations                                          |
\----------------------------------------------------------------------------*/

static BaseType_t test_dio( void )
{
    // TODO Test implementation for DIO
    BaseType_t ok = pdFAIL;

    // printf( "F/W Test: DIO\n" );

    return ok;
}

static BaseType_t test_adc( void )
{
    // TODO Test implementation for ADC
    BaseType_t ok = pdFAIL;

    // printf( "F/W Test: ADC\n" );

    return ok;
}

static BaseType_t test_uart( void )
{
    // TODO Test implementation for UART
    // TODO HIGH Remove sciSend test implementation in production release
    BaseType_t ok = pdFAIL;
    static S_UART_INFO tx_info;

    // printf( "F/W Test: UART\n" );
    tx_info.id = eUART_0;           /* SCI1 */
    tx_info.sci = UART( eUART_0 );

    tx_info.payload[0] = 0x02;  // STX
    tx_info.payload[1] = '1';   // Device Address
    tx_info.payload[2] = '1';   //
    tx_info.payload[3] = '0';   // Sub Address
    tx_info.payload[4] = '0';   //
    tx_info.payload[5] = 'C';   // Type
    tx_info.payload[6] = '2';   // Packet ID
    tx_info.payload[7] = '3';   //
    tx_info.payload[8] = '0';   // Length
    tx_info.payload[9] = '0';   //
    tx_info.payload[10] = '1';  // Command / Query ( f_CMDCommCenterFreq = '1', '0' )
    tx_info.payload[11] = '0';  //
    tx_info.payload[12] = '4';  // CRC: 0x40BA7EA2
    tx_info.payload[13] = '0';  //
    tx_info.payload[14] = 'B';  //
    tx_info.payload[15] = 'A';  //
    tx_info.payload[16] = '7';  //
    tx_info.payload[17] = 'E';  //
    tx_info.payload[18] = 'A';  //
    tx_info.payload[19] = '2';  //
    tx_info.payload[20] = 0x03; // ETX
    tx_info.payload[21] = '\0'; // NULL terminated

    sciSend( tx_info.sci, sizeof( tx_info ), ( uint8 * ) &tx_info );

    return ok;
}

static BaseType_t test_spi( void )
{
    // TODO Test implementation for SPI
    BaseType_t ok = pdFAIL;

    // printf( "F/W Test: SPI\n" );

    return ok;
}

static BaseType_t test_i2c( void )
{
    // TODO Test implementation for I2C
    static S_I2C_INFO tx_info;
    BaseType_t ok = pdFAIL;

    const S_I2C_CONFIG * const p_cfg = i2cGetConfig();

    // printf( "F/W Test: I2C\n" );

    tx_info.id = eI2C_0;            /* I2C2 */
    tx_info.i2c = I2C( eI2C_0 );    /* i2cREG2 */
    tx_info.dev_id = eI2C2_DEV_0;
    tx_info.payload[ 0 ] = p_cfg[ tx_info.id ].device[ tx_info.dev_id ].id_reg;

    bme280_init( &tx_info );

    return ok;
}

static BaseType_t test_can( void )
{
    // TODO Test implementation for CAN
    static S_CAN_INFO tx_info;
    BaseType_t ok = pdFAIL;

    const S_CAN_CONFIG * const p_cfg = canGetConfig();

    tx_info.id = eCAN_0;
    tx_info.can = CAN( tx_info.id );
    memset( &tx_info.payload, 0xAA, sizeof( tx_info.payload ) - 1 );
    tx_info.payload[ sizeof( tx_info.payload ) - 1 ] = 0x0A;

    canEnableloopback( tx_info.can, Internal_Lbk );
    canTransmit( tx_info.can, canMESSAGE_BOX1, ( const uint8 * ) &tx_info.payload[ 0 ] );

    return ok;
}

/*----------------------------------------------------------------------------\
|   End of fw_test.c module                                                   |
\----------------------------------------------------------------------------*/

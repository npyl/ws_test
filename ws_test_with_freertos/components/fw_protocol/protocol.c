/*----------------------------------------------------------------------------\
|          Copyright(c) 2025 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : protocol.c Module File.                                    |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 26 …·Ì 2025                                                |
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

#include <stdio.h>
#include <string.h>

#include "fw_globals.h"
#include "Typeconv.h"
#include "fw_crc.h"
#include "fw_uart.h"
#include "protocol.h"

/*----------------------------------------------------------------------------\
|   Public Constant Definitions                                               |
\----------------------------------------------------------------------------*/

// #define MIN_PACKET_SIZE ( sizeof( S_CMD_QRY_PACKET ) - PAYLOAD_SIZE )
#define MIN_PACKET_SIZE 15

/*----------------------------------------------------------------------------\
|   Public Data Definitions                                                   |
\----------------------------------------------------------------------------*/

 S_CMD_QRY_PACKET st_protocol;

/*----------------------------------------------------------------------------\
|   Private Type Definitions                                                  |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Private Constant Definitions                                              |
\----------------------------------------------------------------------------*/

static const S_CMD_REC buf_Commands[ c_CMDMAXCMD ] =
{
    { c_SUBADDR, 'C', eCmdCenterFrq,        8, &f_CMDCommCenterFreq },
    { c_SUBADDR, 'C', eCmdChBandWidth,      8, &f_CMDChBandWidth },
    { c_SUBADDR, 'C', eCmdGainSelect,       2, &f_CMDGainSelect },
    { c_SUBADDR, 'C', eCmdAICBypass,        2, &f_CMDAICBypass },
    { c_SUBADDR, 'C', eCmdGen22KHz,         2, &f_CMDGen22KHz },
    { c_SUBADDR, 'C', eCmdPolarizationVolt, 2, &f_CMDPolarizationVolt },
};

static const S_QRY_REC buf_Queries[ c_QRYMAXQRY ] =
{
    { c_SUBADDR, 'Q', eQryADCOutputLevel,   0, &f_QRYADCOutputLevel },
    { c_SUBADDR, 'Q', eQryLNBInputLevel,    0, &f_QRYLNBInputLevel },
    { c_SUBADDR, 'Q', eQryPowerLevel,       0, &f_QRYPowerLevel },
    { c_SUBADDR, 'Q', eQryAnalogAttn,       0, &f_QRYAnalogAttn },
    { c_SUBADDR, 'Q', eQryFaultRelayStatus, 0, &f_QRYFaultRelayStatus },
    { c_SUBADDR, 'Q', eQryAlarmStatus,      0, &f_QRYAlarmStatus },
};

/*----------------------------------------------------------------------------\
|   Private Data Declarations                                                 |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Private Function Declarations                                             |
\----------------------------------------------------------------------------*/

static void send_rs485( INT32U length, INT8U *tx_buffer );

/*----------------------------------------------------------------------------\
|   Public Function Implementations                                           |
\----------------------------------------------------------------------------*/

void f_Protocol_ReceiveProcessRS485Data( PINT8U p_rxdata )
{
    INT8U chk_pkt_res;
    S_UART_INFO rx_packet;

    rx_packet.id = eUART_0;
    rx_packet.sci = UART( eUART_0 );

    /* PINT8U p_rxdata = pointer to S_UART_INFO payload field. Payload should have the form: STX | ..ASCII chars.. | ETX | '\0' */
    chk_pkt_res = f_Protocol_FindAndCheckPacket( p_rxdata, ( PINT8U ) &rx_packet.payload[ 0 ] );
    /* rx_packet.payload should have the form: | .. ASCII chars .. | */
    rx_packet.payload_length = strlen( ( const char* ) rx_packet.payload );     // Get the actual payload data length if application needs it */
    if ( chk_pkt_res == PRCL_SUCCESS )
    {
        f_Protocol_Decode( ( PINT8U ) &rx_packet.payload[ 0 ] );
    }
    else
    {
        // do nothing ( = drop packet )
    }
}

void f_Protocol_Decode( PINT8U p_rx_packet )
{
    // Since p_rx_packet points to v_DevAddr, we need to adjust the base pointer
    // to properly access the full packet structure
    S_CMD_QRY_PACKET *pst_packet = ( S_CMD_QRY_PACKET * )( p_rx_packet - offsetof( S_CMD_QRY_PACKET, v_DevAddr ) );

    INT8U cmd_qry_code;
    INT8U sub_addr;
    INT8U i;

    cmd_qry_code = f_TypeConv_HexBin8( pst_packet->v_Cmd_Qry );
    sub_addr = f_TypeConv_HexBin8( pst_packet->v_SubAddr );

    if ( pst_packet->v_Type == 'C' )
    {
        for ( i = 0; i < c_CMDMAXCMD; i++ )
        {
            if ( ( buf_Commands[ i ].v_SubAddr == sub_addr ) &&
                 ( buf_Commands[ i ].v_Type == pst_packet->v_Type ) &&
                 ( buf_Commands[ i ].e_Command == cmd_qry_code ) )
            {
                // Found matching command, call its handler function
                if ( buf_Commands[ i ].f_LUT_CommFunc != NULL )
                {
                    buf_Commands[ i ].f_LUT_CommFunc( pst_packet );
                    return;
                }
            }
        }
    }
    else if ( pst_packet->v_Type == 'Q' )
    {
        for ( i = 0; i < c_QRYMAXQRY; i++ )
        {
            if ( ( buf_Queries[ i ].v_SubAddr == sub_addr ) &&
                 ( buf_Queries[ i ].v_Type == pst_packet->v_Type ) &&
                 ( buf_Queries[ i ].e_Query == cmd_qry_code ) )
            {
                // Found matching query, call its handler function
                if ( buf_Queries[ i ].f_LUT_CommFunc != NULL )
                {
                    buf_Queries[ i ].f_LUT_CommFunc( pst_packet );
                    return;
                }
            }
        }
    }
}

/*
    Added check for multiple STX characters
    Fixed state enum case consistency
    Only NULL terminates output buffer after validation
    Returns proper status codes for success/failure
    Properly structured the packet validation and copy sequence
    Moved NULL termination to output buffer instead of input buffer
    Added proper scope for validations within ETX case
    Validates packet CRC to that calculated
*/
INT8U f_Protocol_FindAndCheckPacket( PINT8U p_rxdata, PINT8U p_rxpacket )
{
    INT16U v_idx;
    INT8U v_data;
    INT8U v_startidx, v_stopidx;
    E_PACKET_ERRORS v_status = PRCL_ERR_NO_STX;  // Default to no STX found
    INT16U v_rcount, v_maxcount, v_len;
    E_PROTOCOL_STATE e_state;
    INT8U v_stx_found = 0;

    /* Discard on NULL Rx buffer */
    if ( p_rxdata == NULL )
        return PRCL_ERR_DATA_NULL;

    /* Discard on NULL packet buffer */
    if ( p_rxpacket == NULL )
        return PRCL_ERR_PKT_NULL;

    v_maxcount = f_TypeConv_StringLen( p_rxdata );
    v_rcount = 0;
    v_startidx = 0;
    v_stopidx = 0;

    /* default is active but no rx yet */
    e_state = ePRCLIDLE;

    for ( v_idx = 0; v_idx < v_maxcount; v_idx++ )
    {
        v_data = *( p_rxdata + v_idx );

        switch (v_data)
        {
            case c_STX:
                /* Track that we found at least one STX */
                v_stx_found = 1;
                /* Reset counters and start new packet search */
                v_startidx = v_idx;
                e_state = ePRCLSOP;
                v_rcount = 0;
                /* Update status to looking for ETX */
                v_status = PRCL_ERR_NO_ETX;
                break;

            case c_ETX:
                /* Ensure EOP follows SOP */
                if ( v_startidx < v_idx && e_state == ePRCLSOP )
                {
                    v_stopidx = v_idx;

                    v_len = v_stopidx - v_startidx - 1;

                    if ( v_len < MIN_PACKET_SIZE )
                    {
                        v_status = PRCL_ERR_SHORTFRM;
                        e_state = ePRCLIDLE;
                        continue;
                    }

                    // Validate packet contents as ASCII characters
                    if ( !f_TypeConv_BufferASCII_Valid( &p_rxdata[ v_startidx + 1 ], v_len ) )
                    {
                        v_status = PRCL_ERR_ASCII;
                        e_state = ePRCLIDLE;
                        continue;
                    }

                    // Copy ascii validated packet to output buffer
                    memcpy( p_rxpacket, &p_rxdata[v_startidx + 1 ], v_len );
                    p_rxpacket[ v_len ] = 0;  // NULL terminate

                    // Check CRC
                    uint32_t crc_pos =  strlen( ( char * ) p_rxpacket) - 8;

                    char pkt_crc_str[ 9 ] = { 0 };
                    strncpy( pkt_crc_str, ( char * ) &p_rxpacket[ crc_pos ], 8 );

                    uint32_t calc_crc = crc32( &p_rxpacket[ 0 ], crc_pos );
                    char calc_crc_str[ 9 ] = { 0 };
                    f_TypeConv_BinHex32( calc_crc, ( INT8 * ) calc_crc_str );

                    if ( strcmp( calc_crc_str, pkt_crc_str ) != 0 )
                    {
                        v_status = PRCL_ERR_CRC;
                        e_state = ePRCLIDLE;
                        continue;
                    }

                    v_status = PRCL_SUCCESS;
                    break;
                }
                break;

            default:
                if ( e_state == ePRCLSOP )
                {
                    v_rcount += 1;
                }
                break;
        }

        if ( v_status == PRCL_SUCCESS )
        {
            break;  // Exit if we found a valid packet
        }
    }

    /* If we reach the end without finding an STX, return NO_STX error */
    if ( !v_stx_found )
    {
        v_status = PRCL_ERR_NO_STX;
    }

    return v_status;
}

void send_ack_nak( INT8U v_ack_nak,
                   INT8U *v_DevAddr,
                   INT8U *v_SubAddr,
                   INT8U *v_PacketID )
{
    S_ACK_NAK ack_nak_response = { 0 };
    INT32U v_crc;

    ack_nak_response.c_stx = c_STX;
    memcpy( ack_nak_response.v_DevAddr, v_DevAddr, sizeof( ack_nak_response.v_DevAddr ) );
    memcpy( ack_nak_response.v_SubAddr, v_SubAddr, sizeof( ack_nak_response.v_SubAddr ) );
    ack_nak_response.v_ack_nak = v_ack_nak;
    memcpy( ack_nak_response.v_PacketID, v_PacketID, sizeof( ack_nak_response.v_PacketID ) );
    v_crc = crc32( &ack_nak_response.v_DevAddr,
                   sizeof( ack_nak_response ) -
                   sizeof( ack_nak_response.c_stx ) -
                   sizeof( ack_nak_response.c_etx ) -
                   sizeof( ack_nak_response.v_crc ) );
    f_TypeConv_BinHex32( v_crc, ( INT8 * ) ack_nak_response.v_crc );
    ack_nak_response.c_etx = c_ETX;

    send_rs485( sizeof( ack_nak_response ), ( uint8 * ) &ack_nak_response );
}

void send_qry_response( INT8U *v_DevAddr,
                        INT8U *v_SubAddr,
                        INT8U v_Type,
                        INT8U *v_PacketID,
                        INT8U *v_Length,
                        INT8U *v_CmdQuery,
                        INT8U *buf_payload )
{
    S_CMD_QRY_PACKET qry_response = { 0 };
    INT32U v_crc;

    qry_response.c_stx = c_STX;
    memcpy( qry_response.v_DevAddr, v_DevAddr, sizeof( qry_response.v_DevAddr ) );
    memcpy( qry_response.v_SubAddr, v_SubAddr, sizeof( qry_response.v_SubAddr ) );
    qry_response.v_Type = v_Type;
    memcpy( qry_response.v_PacketID, v_PacketID, sizeof( qry_response.v_PacketID ) );
    memcpy( qry_response.v_payload_length, v_Length, sizeof( qry_response.v_payload_length ) );
    v_crc = crc32( &qry_response.v_DevAddr,
                   sizeof( qry_response ) -
                   sizeof( qry_response.c_stx ) -
                   sizeof( qry_response.c_etx ) -
                   sizeof( qry_response.v_crc ) );
    f_TypeConv_BinHex32( v_crc, ( INT8 *) &qry_response.v_crc );
    qry_response.c_etx = c_ETX;

    send_rs485( sizeof( qry_response ), ( uint8 * ) &qry_response );
}

/**********************************************
 *
 * Commands
 *
 **********************************************/

void f_CMDCommCenterFreq( S_CMD_QRY_PACKET *pst_prtcl )
{
    // Perform action

#if 1
    send_ack_nak( c_ACK,
                  pst_prtcl->v_DevAddr,
                  pst_prtcl->v_SubAddr,
                  pst_prtcl->v_PacketID );
#endif
}

void f_CMDChBandWidth( S_CMD_QRY_PACKET *pst_prtcl )
{
    // Perform action

    send_ack_nak( c_ACK,
                  pst_prtcl->v_DevAddr,
                  pst_prtcl->v_SubAddr,
                  pst_prtcl->v_PacketID );
}

void f_CMDGainSelect( S_CMD_QRY_PACKET *pst_prtcl )
{
    // Perform action

    send_ack_nak( c_ACK,
                  pst_prtcl->v_DevAddr,
                  pst_prtcl->v_SubAddr,
                  pst_prtcl->v_PacketID );
}

void f_CMDAICBypass( S_CMD_QRY_PACKET *pst_prtcl )
{
    // Perform action

    send_ack_nak( c_ACK,
                  pst_prtcl->v_DevAddr,
                  pst_prtcl->v_SubAddr,
                  pst_prtcl->v_PacketID );
}

void f_CMDGen22KHz( S_CMD_QRY_PACKET *pst_prtcl )
{
    // Perform action

    send_ack_nak( c_ACK,
                  pst_prtcl->v_DevAddr,
                  pst_prtcl->v_SubAddr,
                  pst_prtcl->v_PacketID );
}

void f_CMDPolarizationVolt( S_CMD_QRY_PACKET *pst_prtcl )
{
    // Perform action

    send_ack_nak( c_ACK,
                  pst_prtcl->v_DevAddr,
                  pst_prtcl->v_SubAddr,
                  pst_prtcl->v_PacketID );
}

/**********************************************
 *
 * Queries
 *
 **********************************************/

void f_QRYADCOutputLevel( S_CMD_QRY_PACKET *pst_prtcl )
{
    INT8U qry_code[ 2 ] = { '8', '1' };
    INT8U payload_size = 0;
    INT8U payload_size_str[ 2 ];
    INT8U tx_buf[ PAYLOAD_SIZE ] = { 0 };

    // TODO Ilias: Populate 'tx_buf' with retrieved ascii data and calculate 'payload_size'
    //

    f_TypeConv_BinHex8( payload_size, ( INT8 * ) payload_size_str );
    send_qry_response( pst_prtcl->v_DevAddr,
                       pst_prtcl->v_SubAddr,
                       c_RSP,
                       pst_prtcl->v_PacketID,
                       payload_size_str,
                       qry_code,
                       tx_buf );
}

void f_QRYLNBInputLevel( S_CMD_QRY_PACKET *pst_prtcl )
{
    INT8U qry_code[ 2 ] = { '8', '6' };
    INT8U payload_size = 0;
    INT8U payload_size_str[ 2 ];
    INT8U tx_buf[ PAYLOAD_SIZE ] = { 0 };

    // TODO Ilias: Populate 'tx_buf' with retrieved ascii data and calculate 'payload_size'
    //

    f_TypeConv_BinHex8( payload_size, ( INT8 * ) payload_size_str );
    send_qry_response( pst_prtcl->v_DevAddr,
                       pst_prtcl->v_SubAddr,
                       c_RSP,
                       pst_prtcl->v_PacketID,
                       payload_size_str,
                       qry_code,
                       tx_buf );
}

void f_QRYPowerLevel( S_CMD_QRY_PACKET *pst_prtcl )
{
    INT8U qry_code[ 2 ] = { '9', '1' };
    INT8U payload_size = 0;
    INT8U payload_size_str[ 2 ];
    INT8U tx_buf[ PAYLOAD_SIZE ] = { 0 };

    // TODO Ilias: Populate 'tx_buf' with retrieved ascii data and calculate 'payload_size'
    //

    f_TypeConv_BinHex8( payload_size, ( INT8 * ) payload_size_str );
    send_qry_response( pst_prtcl->v_DevAddr,
                       pst_prtcl->v_SubAddr,
                       c_RSP,
                       pst_prtcl->v_PacketID,
                       payload_size_str,
                       qry_code,
                       tx_buf );
}

void f_QRYAnalogAttn( S_CMD_QRY_PACKET *pst_prtcl )
{
    INT8U qry_code[ 2 ] = { '9', '6' };
    INT8U payload_size = 0;
    INT8U payload_size_str[ 2 ];
    INT8U tx_buf[ PAYLOAD_SIZE ] = { 0 };

    // TODO Ilias: Populate 'tx_buf' with retrieved ascii data and calculate 'payload_size'
    //

    f_TypeConv_BinHex8( payload_size, ( INT8 * ) payload_size_str );
    send_qry_response( pst_prtcl->v_DevAddr,
                       pst_prtcl->v_SubAddr,
                       c_RSP,
                       pst_prtcl->v_PacketID,
                       payload_size_str,
                       qry_code,
                       tx_buf );
}

void f_QRYFaultRelayStatus( S_CMD_QRY_PACKET *pst_prtcl )
{
    INT8U qry_code[ 2 ] = { 'A', '1' };
    INT8U payload_size = 0;
    INT8U payload_size_str[ 2 ];
    INT8U tx_buf[ PAYLOAD_SIZE ] = { 0 };

    // TODO Ilias: Populate 'tx_buf' with retrieved ascii data and calculate 'payload_size'
    //

    f_TypeConv_BinHex8( payload_size, ( INT8 * ) payload_size_str );
    send_qry_response( pst_prtcl->v_DevAddr,
                       pst_prtcl->v_SubAddr,
                       c_RSP,
                       pst_prtcl->v_PacketID,
                       payload_size_str,
                       qry_code,
                       tx_buf );
}

void f_QRYAlarmStatus( S_CMD_QRY_PACKET *pst_prtcl )
{
    INT8U qry_code[ 2 ] = { 'A', '3' };
    INT8U payload_size = 0;
    INT8U payload_size_str[ 2 ];
    INT8U tx_buf[ PAYLOAD_SIZE ] = { 0 };

    // TODO Ilias: Populate 'tx_buf' with retrieved ascii data and calculate 'payload_size'
    //

    f_TypeConv_BinHex8( payload_size, ( INT8 * ) payload_size_str );
    send_qry_response( pst_prtcl->v_DevAddr,
                       pst_prtcl->v_SubAddr,
                       c_RSP,
                       pst_prtcl->v_PacketID,
                       payload_size_str,
                       qry_code,
                       tx_buf );
}
/*----------------------------------------------------------------------------\
|   Private Function Implementations                                          |
\----------------------------------------------------------------------------*/

static void send_rs485( INT32U length, INT8U *tx_buffer )
{
    sciBASE_t *sci = UART( eUART_0 );

    // TODO Enable TX GPIO
    sciSend( sci, length, ( INT8U * ) tx_buffer );
}

/*----------------------------------------------------------------------------\
|   End of protocol.c module                                                  |
\----------------------------------------------------------------------------*/

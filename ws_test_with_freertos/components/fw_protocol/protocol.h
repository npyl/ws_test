/*----------------------------------------------------------------------------\
|          Copyright(c) 2025 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : protocol.h Header File.                                    |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 26 …·Ì 2025                                                |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   <Module Header Description>                                               |
|                                                                             |
\----------------------------------------------------------------------------*/

#ifndef protocol_H
#define protocol_H

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

#include "Typeconv.h"

/*----------------------------------------------------------------------------\
|   Public Constant Declarations                                              |
\----------------------------------------------------------------------------*/

#define c_SUBADDR       0x00
#define c_CMDMAXCMD     ( 6 )
#define c_QRYMAXQRY     ( 6 )
#define PAYLOAD_SIZE    ( 50 )
#define c_STX           ( 0x02 )
#define c_ETX           ( 0x03 )
#define c_ACK           'A'         /* Acknowledge */
#define c_NAK           'N'         /* Not Acknowledge */
#define c_RSP           'R'         /* Response */

/*----------------------------------------------------------------------------\
|   Public Type Definitions                                                   |
\----------------------------------------------------------------------------*/

typedef enum {
    PRCL_SUCCESS            = 0x00, // Valid packet found, validated, and copied
    PRCL_ERR_SHORTFRM       = 0xF9, // Short packet length
    PRCL_ERR_CRC            = 0xFA, // CRC error
    PRCL_ERR_ASCII          = 0xFB, // ASCII validation failed
    PRCL_ERR_NO_STX         = 0xFC, // No STX found in buffer
    PRCL_ERR_NO_ETX         = 0xFD, // Found STX but no matching ETX
    PRCL_ERR_PKT_NULL       = 0xFE, // Output packet buffer NULL
    PRCL_ERR_DATA_NULL      = 0xFF, // Input data buffer NULL
} E_PACKET_ERRORS;

typedef enum
{
    eCmdCenterFrq           = 0x10,
    eCmdChBandWidth         = 0x20,
    eCmdGainSelect          = 0x30,
    eCmdAICBypass           = 0x31,
    eCmdGen22KHz            = 0x34,
    eCmdPolarizationVolt    = 0x35,
} E_COMMAND;

typedef enum
{
    eQryADCOutputLevel      = 0x80,
    eQryLNBInputLevel       = 0x85,
    eQryPowerLevel          = 0x90,
    eQryAnalogAttn          = 0x95,
    eQryFaultRelayStatus    = 0xA0,
    eQryAlarmStatus         = 0xA2,
} E_QUERY;

typedef enum
{
    ePRCLIDLE,
    ePRCLSOP,
    ePRCLEOP,
} E_PROTOCOL_STATE;

typedef struct
{
    INT8U c_stx;                        // Start of Command/Query/Response packet
    INT8U v_DevAddr[ 2 ];               // Address of responding device
    INT8U v_SubAddr[ 2 ];               // Sub system responding within device
    INT8U v_Type;                       // Packet type: R: Response, A: ACK, N: NAK
    INT8U v_PacketID[ 2 ];              // Packet ID of command responsding
    INT8U v_payload_length[ 2 ];        // Payload length in bytes
    INT8U v_Cmd_Qry[ 2 ];               // Command/Query/Response code
    INT8U payload[ PAYLOAD_SIZE ];      // Parameters
    INT8U v_crc[ 8 ];                   // CRC32
    INT8U c_etx;                        // End of Command/Query/Response packet
} S_CMD_QRY_PACKET;

typedef struct
{
    INT8U v_SubAddr;
    INT8U v_Type;
    E_COMMAND e_Command;
    INT8U v_Length;
    void ( *f_LUT_CommFunc ) ( S_CMD_QRY_PACKET *pst_prtcl );
} S_CMD_REC;

typedef struct
{
    INT8U v_SubAddr;
    INT8U v_Type;
    E_QUERY e_Query;
    INT8U v_Length;
    void ( *f_LUT_CommFunc ) ( S_CMD_QRY_PACKET *pst_prtcl );
} S_QRY_REC;

typedef struct
{
    INT8U c_stx;
    INT8U v_DevAddr[ 2 ];
    INT8U v_SubAddr[ 2 ];
    INT8U v_ack_nak;
    INT8U v_PacketID[ 2 ];
    INT8U v_crc[ 8 ];
    INT8U c_etx;
} S_ACK_NAK;

/*----------------------------------------------------------------------------\
|   Public Function Declarations                                              |
\----------------------------------------------------------------------------*/

INT8U f_Protocol_FindAndCheckPacket( PINT8U p_rxdata, PINT8U p_rxpacket );
void f_Protocol_ReceiveProcessRS485Data( PINT8U p_rxdata );
void f_Protocol_Decode( PINT8U p_rx_packet );

/*
 * Commands
 */
void f_CMDCommCenterFreq( S_CMD_QRY_PACKET *pst_prtcl );
void f_CMDChBandWidth( S_CMD_QRY_PACKET *pst_prtcl );
void f_CMDGainSelect( S_CMD_QRY_PACKET *pst_prtcl );
void f_CMDAICBypass( S_CMD_QRY_PACKET *pst_prtcl );
void f_CMDGen22KHz( S_CMD_QRY_PACKET *pst_prtcl );
void f_CMDPolarizationVolt( S_CMD_QRY_PACKET *pst_prtcl );

/*
 * Queries
 */
void f_QRYADCOutputLevel( S_CMD_QRY_PACKET *pst_prtcl );
void f_QRYLNBInputLevel( S_CMD_QRY_PACKET *pst_prtcl );
void f_QRYPowerLevel( S_CMD_QRY_PACKET *pst_prtcl );
void f_QRYAnalogAttn( S_CMD_QRY_PACKET *pst_prtcl );
void f_QRYFaultRelayStatus( S_CMD_QRY_PACKET *pst_prtcl );
void f_QRYAlarmStatus( S_CMD_QRY_PACKET *pst_prtcl );

void send_qry_response( INT8U *v_DevAddr,
                        INT8U *v_SubAddr,
                        INT8U v_Type,
                        INT8U *v_PacketID,
                        INT8U *v_Length,
                        INT8U *v_CmdQuery,
                        INT8U *buf_payload );

void send_ack_nak( INT8U v_ack_nak,
                   INT8U *v_DevAddr,
                   INT8U *v_SubAddr,
                   INT8U *v_PacketID );

void handle_packet( PINT8U p_rxpacket );

/*----------------------------------------------------------------------------\
|   End of protocol.h header file                                             |
\----------------------------------------------------------------------------*/

#endif  /* protocol_H */

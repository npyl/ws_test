/*----------------------------------------------------------------------------\
|          Copyright(c) 2024 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : fw_uart.c Module File.                                     |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 16 …·Ì 2024                                                |
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

#include <string.h>

#include "HL_sci.h"
#include "HL_sys_vim.h"
#include "math.h"
#include "FreeRTOS.h"
#include "os_queue.h"

#include "fw_uart.h"

/*----------------------------------------------------------------------------\
|   Public Constant Definitions                                               |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Public Data Declarations                                                  |
\----------------------------------------------------------------------------*/

xQueueHandle xUARTQueueHandle;

/*----------------------------------------------------------------------------\
|   Private Type Definitions                                                  |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Private Constant Definitions                                              |
\----------------------------------------------------------------------------*/

static const S_UART_CONFIG uart_config_defs[ eUART_MAX ] =
{
     {
        .id = eUART_0,
        .label = "SCI1",
        .params =
        {
            .baud = eBAUD_115200,
            .stop = eSTOP_ONE,
            .parity_en = FALSE,
            .parity_even = FALSE,
            .loopback = TRUE,
        },
        .enabled = TRUE,
     },
     {
        .id = eUART_1,
        .label = "SCI2",
        .params =
        {
             .baud = eBAUD_115200,
             .stop = eSTOP_ONE,
             .parity_en = FALSE,
             .parity_even = FALSE,
             .loopback = FALSE,
        },
        .enabled = FALSE,
     },
     {
        .id = eUART_2,
        .label = "SCI3",
        .params =
        {
             .baud = eBAUD_115200,
             .stop = eSTOP_ONE,
             .parity_en = FALSE,
             .parity_even = FALSE,
             .loopback = FALSE,
        },
        .enabled = FALSE,
     },
     {
        .id = eUART_3,
        .label = "SCI4",
        .params =
        {
             .baud = eBAUD_115200,
             .stop = eSTOP_ONE,
             .parity_en = FALSE,
             .parity_even = FALSE,
             .loopback = FALSE,
        },
        .enabled = FALSE,
     },
};

/*----------------------------------------------------------------------------\
|   Private Data Declarations                                                 |
\----------------------------------------------------------------------------*/

static uint8 sci_rx_buffer[ eUART_MAX ][ sizeof( S_UART_INFO ) ];
static uint8 sci_rx_buffer_idx[ eUART_MAX ];

/** @struct g_sciTransfer
*   @brief Interrupt mode globals
*
*/
static volatile struct g_sciTransfer
{
    uint32   mode;         /* Used to check for TX interrupt Enable */
    uint32   tx_length;    /* Transmit data length in number of Bytes */
    uint32   rx_length;    /* Receive data length in number of Bytes */
    uint8    * tx_data;    /* Transmit data pointer */
    uint8    * rx_data;    /* Receive data pointer */
} g_sciTransfer_t[ eUART_MAX ];

/*----------------------------------------------------------------------------\
|   Private Function Declarations                                             |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Public Function Implementations                                           |
\----------------------------------------------------------------------------*/

void uart_init( void )
{
    int i;
    sciBASE_t * p_sci;
    uint32_t timeout;

    xUARTQueueHandle = xQueueCreate( UART_QUEUE_LENGTH, UART_QUEUE_ITEM_SIZE );
    const S_UART_CONFIG * const p_cfg = uartGetConfig();

    for ( i = 0; i < eUART_MAX; i++ )
    {

        p_sci = UART( i );
        if ( p_cfg[ i ].enabled == TRUE )
        {
            /* Bring SCI out of reset */
            p_sci->GCR0 = 0U;
            p_sci->GCR0 = 1U;

            /* Disable all interrupts */
            p_sci->CLEARINT = 0xFFFFFFFFU;
            p_sci->CLEARINTLVL = 0xFFFFFFFFU;

            /* Global control 1 register */
            p_sci->GCR1 = ( uint32 ) ( ( uint32 ) 1U << 25U )  /* Enable transmit interrupt */
                        | ( uint32 ) ( ( uint32 ) 1U << 24U )  /* Enable receive interrupt */
                        | ( uint32 ) ( ( uint32 ) 1U << 5U )   /* Internal clock ( Device has no clock pin ) */
                        | ( uint32 ) ( ( uint32 ) 1U << 1U );  /* Asynchronous timing mode */

            p_sci->GCR1 |= ( uint32 ) ( ( uint32 ) p_cfg[ i ].params.parity_en << 2U );
            p_sci->GCR1 |= ( uint32 ) ( ( uint32 ) p_cfg[ i ].params.parity_even << 3U );
            p_sci->GCR1 |= ( uint32 ) ( ( uint32 ) p_cfg[ i ].params.stop << 4U );

            /* Baud rate */
            p_sci->BRS = p_cfg[ i ].params.baud;

            /* Transmission length */
            p_sci->FORMAT = 7U;

            /* Set SCI pins functional mode */
            p_sci->PIO0 = ( uint32 ) ( ( uint32 ) 1U << 2U )   /* tx pin */
                        | ( uint32 ) ( ( uint32 ) 1U << 1U );  /* rx pin */

            /* Set SCI pins default output value */
            p_sci->PIO3 = ( uint32 ) ( ( uint32 ) 0U << 2U )   /* tx pin */
                        | ( uint32 ) ( ( uint32 ) 0U << 1U );  /* rx pin */

            /* Set SCI pins output direction */
            p_sci->PIO1 = ( uint32 ) ( ( uint32 ) 0U << 2U )   /* tx pin */
                        | ( uint32 ) ( ( uint32 ) 0U << 1U );  /* rx pin */

            /* Set SCI pins open drain enable */
            p_sci->PIO6 = ( uint32 ) ( ( uint32 ) 0U << 2U )   /* tx pin */
                        | ( uint32 ) ( ( uint32 ) 0U << 1U );  /* rx pin */

            /* Set SCI pins pullup/pulldown enable */
            p_sci->PIO7 = ( uint32 ) ( ( uint32 ) 0U << 2U )   /* tx pin */
                        | ( uint32 ) ( ( uint32) 0U << 1U );   /* rx pin */

            /* Set SCI pins pullup / pulldown select */
            p_sci->PIO8 = ( uint32 ) ( ( uint32 ) 1U << 2U )   /* tx pin */
                        | ( uint32 ) ( ( uint32 ) 1U << 1U );  /* rx pin */

            /* Set interrupt level */
            p_sci->SETINTLVL = ( uint32 ) ( ( uint32 ) 0U << 26U )     /* Framing error */
                             | ( uint32 ) ( ( uint32 ) 0U << 25U )     /* Overrun error */
                             | ( uint32 ) ( ( uint32 ) 0U << 24U )     /* Parity error */
                             | ( uint32 ) ( ( uint32 ) 0U << 9U )      /* Receive */
                             | ( uint32 ) ( ( uint32 ) 0U << 8U )      /* Transmit */
                             | ( uint32 ) ( ( uint32 ) 0U << 1U )      /* Wakeup */
                             | ( uint32 ) ( ( uint32 ) 0U << 0U );     /* Break detect */

            /* Set interrupt enable */
            p_sci->SETINT = ( uint32 ) ( ( uint32 ) 0U << 26U )        /* Framing error */
                          | ( uint32 ) ( ( uint32 ) 0U << 25U )        /* Overrun error */
                          | ( uint32 ) ( ( uint32 ) 0U << 24U )        /* Parity error */
                          | ( uint32 ) ( ( uint32 ) 1U << 9U )         /* Receive */
                          | ( uint32 ) ( ( uint32 ) 0U << 1U )         /* Wakeup */
                          | ( uint32 ) ( ( uint32 ) 0U << 0U );        /* Break detect */

            /* Initialize global transfer variables */
            g_sciTransfer_t[ p_cfg[ i ].id ].mode = ( uint32 ) 1U << 8U;
            g_sciTransfer_t[ p_cfg[ i ].id ].tx_length = 0U;
            g_sciTransfer_t[ p_cfg[ i ].id ].rx_length = 0U;

            p_cfg[ i ].params.loopback == TRUE ? ( { sciEnableLoopback( p_sci, Digital_Lbk ); } ) : ( { asm ( " nop" ); } );

            /* Clear Rx buffer and index */
            memset( &sci_rx_buffer[ i ], 0, sizeof( S_UART_INFO ) );
            sci_rx_buffer_idx[ i ] = 0;

            /* Finally start SCI */
            p_sci->GCR1 |= 0x80U;

            /* Enable notifications for this uart */
            // sciEnableNotification( p_sci, SCI_RX_INT );

            /* Wait till SCI IDLE */
            timeout = SCI_TIMEOUT;
            while( ( ( p_sci->FLR & SCI_IDLE ) == SCI_IDLE ) && ( --timeout > 0 ) );

            /* Must setup g_sciTransfer_t .rx_length to 1 in order to
             * trigger SCI Notification when 1st byte arrives
             */
            uint8 ch;
            sciReceive( p_sci, 1, ( uint8 * ) &sci_rx_buffer[0] );
        }
    }
}

const S_UART_CONFIG * const uartGetConfig( void )
{
    /* Return the address of the UART configuration data */
    return uart_config_defs;
}

void sciNotification(sciBASE_t *sci, uint32 flags)
{
    uint8 ch;
    BaseType_t xHigherPriorityTaskWoken;
    S_UART_INFO sci_info;
    /* Determine which SCI RX buffer array index to use:
     *   eUART_0 .. eUART_3 corresponding to sciREG1 .. sciREG3
     */
    E_UART_ID sci_idx = UART_IDX( sci );
    static bool collecting_data = false;

    switch ( sci_idx )
    {
        case eUART_0:
            if ( flags & SCI_RX_INT )   /* Check for received char */
            {
                /* Get received character and prepare for next */
                sciReceive( sci, 1, ( uint8 * ) &ch );

                if ( ch == STX )
                {
                    /* Start of new packet - reset buffer even if we were collecting */
                    collecting_data = true;
                    sci_rx_buffer_idx[ sci_idx ] = 0;
                    sci_rx_buffer[ sci_idx ][ sci_rx_buffer_idx[ sci_idx ]++ ] = ch;
                }
                else if ( ch == ETX && collecting_data )
                {
                    /* End of packet */
                    sci_rx_buffer[ sci_idx ][ sci_rx_buffer_idx[ sci_idx ]++ ] = ETX;
                    sci_rx_buffer[ sci_idx ][ sci_rx_buffer_idx[ sci_idx ] ] = '\0';

                    /* Populate sci_info structure */
                    sci_info.id = sci_idx;
                    sci_info.sci = sci;
                    sci_info.payload_length = sci_rx_buffer_idx[ sci_idx ];
                    memcpy( &sci_info.payload, &sci_rx_buffer[ sci_idx ][ 0 ], sci_info.payload_length );

                    /* Send to queue */
                    xQueueSendFromISR( xUARTQueueHandle, &sci_info, &xHigherPriorityTaskWoken );

                    /* Reset for next packet */
                    collecting_data = false;
                    sci_rx_buffer_idx[ sci_idx ] = 0;
                }
                else if ( collecting_data )
                {
                    /* Store intermediate bytes if we're collecting */
                    if ( sci_rx_buffer_idx[ sci_idx ] < sizeof( sci_info ) - 1 )
                    {
                       sci_rx_buffer[ sci_idx ][ sci_rx_buffer_idx[ sci_idx ]++ ] = ch;
                    }
                    else
                    {
                       /* Buffer overflow - reset */
                       collecting_data = false;
                       sci_rx_buffer_idx[ sci_idx ] = 0;
                    }
                }
            }
            if ( flags & SCI_TX_INT )
            {
                // TODO Switch to receive RS485
                __asm ( " nop" );
            }
            break;
        case eUART_1:
            break;
        case eUART_2:
            break;
        case eUART_3:
            break;
        default:
            break;
    }
}

/* SourceId : SCI_SourceId_002 */
/* DesignId : SCI_DesignId_002 */
/* Requirements : HL_CONQ_SCI_SR6 */
/** @fn void sciSetFunctional(sciBASE_t *sci, uint32 port)
*   @brief Change functional behavior of pins at runtime.
*   @param[in] sci   - sci module base address
*   @param[in] port  - Value to write to PIO0 register
*
*   Change the value of the PCPIO0 register at runtime, this allows to
*   dynamically change the functionality of the SCI pins between functional
*   and GIO mode.
*/
void sciSetFunctional(sciBASE_t *sci, uint32 port)
{
/* USER CODE BEGIN (4) */
/* USER CODE END */

    sci->PIO0 = port;

/* USER CODE BEGIN (5) */
/* USER CODE END */
}


/* SourceId : SCI_SourceId_003 */
/* DesignId : SCI_DesignId_003 */
/* Requirements : HL_CONQ_SCI_SR7 */
/** @fn void sciSetBaudrate(sciBASE_t *sci, uint32 baud)
*   @brief Change baudrate at runtime.
*   @param[in] sci  - sci module base address
*   @param[in] baud - baudrate in Hz
*
*   Change the SCI baudrate at runtime.
*/
void sciSetBaudrate(sciBASE_t *sci, uint32 baud)
{
    float64   vclk = 75.000 * 1000000.0;
    uint32 f    = ((sci->GCR1 & 2U) == 2U) ? 16U : 1U;
    uint32 temp;
    float64 temp2;
/* USER CODE BEGIN (6) */
/* USER CODE END */

    /*SAFETYMCUSW 96 S MR:6.1 <APPROVED> "Calculations including int and float cannot be avoided" */
    temp = (f*(baud));
    temp2 = ((vclk)/((float64)temp))-1U;
    temp2 = floor(temp2 + 0.5);        /* Rounding-off to the closest integer */
    sci->BRS = (uint32)((uint32)temp2 & 0x00FFFFFFU);

/* USER CODE BEGIN (7) */
/* USER CODE END */
}


/* SourceId : SCI_SourceId_004 */
/* DesignId : SCI_DesignId_004 */
/* Requirements : HL_CONQ_SCI_SR8 */
/** @fn uint32 sciIsTxReady(sciBASE_t *sci)
*   @brief Check if Tx buffer empty
*   @param[in] sci - sci module base address
*
*   @return The TX ready flag
*
*   Checks to see if the Tx buffer ready flag is set, returns
*   0 is flags not set otherwise will return the Tx flag itself.
*/
uint32 sciIsTxReady(sciBASE_t *sci)
{
/* USER CODE BEGIN (8) */
/* USER CODE END */

    return sci->FLR & (uint32)SCI_TX_INT;
}


/* SourceId : SCI_SourceId_005 */
/* DesignId : SCI_DesignId_005 */
/* Requirements : HL_CONQ_SCI_SR9 */
/** @fn void sciSendByte(sciBASE_t *sci, uint8 byte)
*   @brief Send Byte
*   @param[in] sci  - sci module base address
*   @param[in] byte - byte to transfer
*
*   Sends a single byte in polling mode, will wait in the
*   routine until the transmit buffer is empty before sending
*   the byte.  Use sciIsTxReady to check for Tx buffer empty
*   before calling sciSendByte to avoid waiting.
*/
void sciSendByte(sciBASE_t *sci, uint8 byte)
{
/* USER CODE BEGIN (9) */
/* USER CODE END */

    /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
    while ((sci->FLR & (uint32)SCI_TX_INT) == 0U)
    {
    } /* Wait */
    sci->TD = byte;

/* USER CODE BEGIN (10) */
/* USER CODE END */
}


/* SourceId : SCI_SourceId_006 */
/* DesignId : SCI_DesignId_006 */
/* Requirements : HL_CONQ_SCI_SR10 */
/** @fn void sciSend(sciBASE_t *sci, uint32 length, uint8 * data)
*   @brief Send Data
*   @param[in] sci    - sci module base address
*   @param[in] length - number of data words to transfer
*   @param[in] data   - pointer to data to send
*
*   Send a block of data pointed to by 'data' and 'length' bytes
*   long.  If interrupts have been enabled the data is sent using
*   interrupt mode, otherwise polling mode is used.  In interrupt
*   mode transmission of the first byte is started and the routine
*   returns immediately, sciSend must not be called again until the
*   transfer is complete, when the sciNotification callback will
*   be called.  In polling mode, sciSend will not return  until
*   the transfer is complete.
*
*   @note if data word is less than 8 bits, then the data must be left
*         aligned in the data byte.
*/
void sciSend(sciBASE_t *sci, uint32 length, uint8 * data)
{
    uint32 index = (sci  == sciREG1) ? 0U :
                         ((sci == sciREG2) ? 1U :
                         ((sci == sciREG3) ? 2U : 3U));
    uint8 txdata;

/* USER CODE BEGIN (11) */
/* USER CODE END */
/*SAFETYMCUSW 139 S MR:13.7 <APPROVED> "Mode variable is configured in sciEnableNotification()" */
    if ((g_sciTransfer_t[index].mode & (uint32)SCI_TX_INT) != 0U)
    {
        /* we are in interrupt mode */

        g_sciTransfer_t[index].tx_length = length;
        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
        g_sciTransfer_t[index].tx_data   = data;

        /* start transmit by sending first byte */
        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
        txdata = *g_sciTransfer_t[index].tx_data;
        sci->TD     = (uint32)(txdata);
        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
        /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
        g_sciTransfer_t[index].tx_data++;
        sci->SETINT = (uint32)SCI_TX_INT;
    }
    else
    {
        /* send the data */
        /*SAFETYMCUSW 30 S MR:12.2,12.3 <APPROVED> "Used for data count in Transmit/Receive polling and Interrupt mode" */
        while (length > 0U)
        {
            /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
            while ((sci->FLR & (uint32)SCI_TX_INT) == 0U)
            {
            } /* Wait */
            /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
            txdata = *data;
            sci->TD = (uint32)(txdata);
            /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
            /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
            data++;
            length--;
        }
    }

/* USER CODE BEGIN (12) */
/* USER CODE END */
}


/* SourceId : SCI_SourceId_007 */
/* DesignId : SCI_DesignId_007 */
/* Requirements : HL_CONQ_SCI_SR11 */
/** @fn uint32 sciIsRxReady(sciBASE_t *sci)
*   @brief Check if Rx buffer full
*   @param[in] sci - sci module base address
*
*   @return The Rx ready flag
*
*   Checks to see if the Rx buffer full flag is set, returns
*   0 is flags not set otherwise will return the Rx flag itself.
*/
uint32 sciIsRxReady(sciBASE_t *sci)
{
/* USER CODE BEGIN (13) */
/* USER CODE END */

    return sci->FLR & (uint32)SCI_RX_INT;
}

/* SourceId : SCI_SourceId_008 */
/* DesignId : SCI_DesignId_008 */
/* Requirements : HL_CONQ_SCI_SR12 */
/** @fn uint32 sciIsIdleDetected(sciBASE_t *sci)
*   @brief Check if Idle Period is Detected
*   @param[in] sci - sci module base address
*
*   @return The Idle flag
*
*   Checks to see if the SCI Idle flag is set, returns 0 is flags
*   not set otherwise will return the Ilde flag itself.
*/
uint32 sciIsIdleDetected(sciBASE_t *sci)
{
/* USER CODE BEGIN (14) */
/* USER CODE END */

    return sci->FLR & (uint32)SCI_IDLE;
}


/* SourceId : SCI_SourceId_009 */
/* DesignId : SCI_DesignId_009 */
/* Requirements : HL_CONQ_SCI_SR13 */
/** @fn uint32 sciRxError(sciBASE_t *sci)
*   @brief Return Rx Error flags
*   @param[in] sci - sci module base address
*
*   @return The Rx error flags
*
*   Returns the Rx framing, overrun and parity errors flags,
*   also clears the error flags before returning.
*/
uint32 sciRxError(sciBASE_t *sci)
{
    uint32 status = (sci->FLR & ((uint32)SCI_FE_INT | (uint32)SCI_OE_INT |(uint32)SCI_PE_INT));

/* USER CODE BEGIN (15) */
/* USER CODE END */

    sci->FLR = ((uint32)SCI_FE_INT | (uint32)SCI_OE_INT | (uint32)SCI_PE_INT);
    return status;
}


/* SourceId : SCI_SourceId_010 */
/* DesignId : SCI_DesignId_010 */
/* Requirements : HL_CONQ_SCI_SR14 */
/** @fn uint32 sciReceiveByte(sciBASE_t *sci)
*   @brief Receive Byte
*   @param[in] sci - sci module base address
*
*   @return Received byte
*
*    Receives a single byte in polling mode.  If there is
*    not a byte in the receive buffer the routine will wait
*    until one is received.   Use sciIsRxReady to check to
*    see if the buffer is full to avoid waiting.
*/
uint32 sciReceiveByte(sciBASE_t *sci)
{
/* USER CODE BEGIN (16) */
/* USER CODE END */

    /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
    while ((sci->FLR & (uint32)SCI_RX_INT) == 0U)
    {
    } /* Wait */

    return (sci->RD & (uint32)0x000000FFU);
}


/* SourceId : SCI_SourceId_011 */
/* DesignId : SCI_DesignId_011 */
/* Requirements : HL_CONQ_SCI_SR15 */
/** @fn void sciReceive(sciBASE_t *sci, uint32 length, uint8 * data)
*   @brief Receive Data
*   @param[in] sci    - sci module base address
*   @param[in] length - number of data words to transfer
*   @param[in] data   - pointer to data buffer to receive data
*
*   Receive a block of 'length' bytes long and place it into the
*   data buffer pointed to by 'data'.  If interrupts have been
*   enabled the data is received using interrupt mode, otherwise
*   polling mode is used.  In interrupt mode receive is setup and
*   the routine returns immediately, sciReceive must not be called
*   again until the transfer is complete, when the sciNotification
*   callback will be called.  In polling mode, sciReceive will not
*   return  until the transfer is complete.
*/
void sciReceive(sciBASE_t *sci, uint32 length, uint8 * data)
{
/* USER CODE BEGIN (17) */
/* USER CODE END */

    if ((sci->SETINT & (uint32)SCI_RX_INT) == (uint32)SCI_RX_INT)
    {
        /* we are in interrupt mode */
        uint32 index = (sci  == sciREG1) ? 0U :
                       ((sci == sciREG2) ? 1U :
                       ((sci == sciREG3) ? 2U : 3U));

        /* clear error flags */
        sci->FLR = ((uint32) SCI_FE_INT | (uint32) SCI_OE_INT | (uint32) SCI_PE_INT);

        g_sciTransfer_t[index].rx_length = length;
        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
        g_sciTransfer_t[index].rx_data   = data;
    }
    else
    {
        while (length > 0U)
        {
            /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
            while ((sci->FLR & (uint32)SCI_RX_INT) == 0U)
            {
            } /* Wait */
            /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
            *data = (uint8)(sci->RD & 0x000000FFU);
            /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
            /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
            data++;
            length--;
        }
    }
/* USER CODE BEGIN (18) */
/* USER CODE END */
}

/* SourceId : SCI_SourceId_012 */
/* DesignId : SCI_DesignId_014 */
/* Requirements : HL_CONQ_SCI_SR18 */
/** @fn void sciEnableLoopback(sciBASE_t *sci, loopBackType_t Loopbacktype)
*   @brief Enable Loopback mode for self test
*   @param[in] sci        - sci module base address
*   @param[in] Loopbacktype  - Digital or Analog
*
*   This function enables the Loopback mode for self test.
*/
void sciEnableLoopback(sciBASE_t *sci, loopBackType_t Loopbacktype)
{
/* USER CODE BEGIN (19) */
/* USER CODE END */

    /* Clear Loopback incase enabled already */
    sci->IODFTCTRL = 0U;

    /* Enable Loopback either in Analog or Digital Mode */
    sci->IODFTCTRL = (uint32)0x00000A00U
                   | (uint32)((uint32)Loopbacktype << 1U);

/* USER CODE BEGIN (20) */
/* USER CODE END */
}

/* SourceId : SCI_SourceId_013 */
/* DesignId : SCI_DesignId_015 */
/* Requirements : HL_CONQ_SCI_SR19 */
/** @fn void sciDisableLoopback(sciBASE_t *sci)
*   @brief Enable Loopback mode for self test
*   @param[in] sci        - sci module base address
*
*   This function disable the Loopback mode.
*/
void sciDisableLoopback(sciBASE_t *sci)
{
/* USER CODE BEGIN (21) */
/* USER CODE END */

    /* Disable Loopback Mode */
    sci->IODFTCTRL = 0x00000500U;

/* USER CODE BEGIN (22) */
/* USER CODE END */
}

/* SourceId : SCI_SourceId_014 */
/* DesignId : SCI_DesignId_012 */
/* Requirements : HL_CONQ_SCI_SR16 */
/** @fn sciEnableNotification(sciBASE_t *sci, uint32 flags)
*   @brief Enable interrupts
*   @param[in] sci   - sci module base address
*   @param[in] flags - Interrupts to be enabled, can be ored value of:
*                      SCI_FE_INT    - framing error,
*                      SCI_OE_INT    - overrun error,
*                      SCI_PE_INT    - parity error,
*                      SCI_RX_INT    - receive buffer ready,
*                      SCI_TX_INT    - transmit buffer ready,
*                      SCI_WAKE_INT  - wakeup,
*                      SCI_BREAK_INT - break detect
*/
void sciEnableNotification(sciBASE_t *sci, uint32 flags)
{
    uint32 index = (sci  == sciREG1) ? 0U :
                  ((sci == sciREG2) ? 1U :
                  ((sci == sciREG3) ? 2U : 3U));

/* USER CODE BEGIN (23) */
/* USER CODE END */

    g_sciTransfer_t[index].mode |= (flags & (uint32)SCI_TX_INT);
    sci->SETINT                = (flags & (uint32)(~(uint32)(SCI_TX_INT)));

/* USER CODE BEGIN (24) */
/* USER CODE END */
}


/* SourceId : SCI_SourceId_015 */
/* DesignId : SCI_DesignId_013 */
/* Requirements : HL_CONQ_SCI_SR17 */
/** @fn sciDisableNotification(sciBASE_t *sci, uint32 flags)
*   @brief Disable interrupts
*   @param[in] sci   - sci module base address
*   @param[in] flags - Interrupts to be disabled, can be ored value of:
*                      SCI_FE_INT    - framing error,
*                      SCI_OE_INT    - overrun error,
*                      SCI_PE_INT    - parity error,
*                      SCI_RX_INT    - receive buffer ready,
*                      SCI_TX_INT    - transmit buffer ready,
*                      SCI_WAKE_INT  - wakeup,
*                      SCI_BREAK_INT - break detect
*/
void sciDisableNotification(sciBASE_t *sci, uint32 flags)
{
    uint32 index = (sci  == sciREG1) ? 0U :
                  ((sci == sciREG2) ? 1U :
                  ((sci == sciREG3) ? 2U : 3U));

/* USER CODE BEGIN (25) */
/* USER CODE END */

    g_sciTransfer_t[index].mode &= (uint32)(~(flags & (uint32)SCI_TX_INT));
    sci->CLEARINT                = (flags & (uint32)(~(uint32)(SCI_TX_INT)));

/* USER CODE BEGIN (26) */
/* USER CODE END */
}

/* SourceId : SCI_SourceId_016 */
/* DesignId : SCI_DesignId_001 */
/* Requirements :   */
/** @fn sciEnterResetState(sciBASE_t *sci)
*   @brief Enter reset state
*   @param[in] sci   - sci module base address
*   @note The SCI should only be configured while in reset state
*/
void sciEnterResetState(sciBASE_t *sci)
{
    sci->GCR1 &= 0xFFFFFF7FU;
}

/* SourceId : SCI_SourceId_017 */
/* DesignId : SCI_DesignId_001 */
/* Requirements :   */
/** @fn scixitResetState(sciBASE_t *sci)
*   @brief Exit reset state
*   @param[in] sci   - sci module base address
*   @note The SCI should only be configured while in reset state
*/
void sciExitResetState(sciBASE_t *sci)
{
    sci->GCR1 |= 0x00000080U;
}


/* SourceId : SCI_SourceId_018 */
/* DesignId : SCI_DesignId_016 */
/* Requirements : HL_CONQ_SCI_SR23 */
/** @fn void sci1GetConfigValue(sci_config_reg_t *config_reg, config_value_type_t type)
*   @brief Get the initial or current values of the SCI1 configuration registers
*
*   @param[in] *config_reg: pointer to the struct to which the initial or current
*                           value of the configuration registers need to be stored
*   @param[in] type:    whether initial or current value of the configuration registers need to be stored
*                       - InitialValue: initial value of the configuration registers will be stored
*                                       in the struct pointed by config_reg
*                       - CurrentValue: initial value of the configuration registers will be stored
*                                       in the struct pointed by config_reg
*
*   This function will copy the initial or current value (depending on the parameter 'type')
*   of the configuration registers to the struct pointed by config_reg
*
*/

void sci1GetConfigValue(sci_config_reg_t *config_reg, config_value_type_t type)
{
    if (type == InitialValue)
    {
        config_reg->CONFIG_GCR0      = SCI1_GCR0_CONFIGVALUE;
        config_reg->CONFIG_GCR1      = SCI1_GCR1_CONFIGVALUE;
        config_reg->CONFIG_SETINT    = SCI1_SETINT_CONFIGVALUE;
        config_reg->CONFIG_SETINTLVL = SCI1_SETINTLVL_CONFIGVALUE;
        config_reg->CONFIG_FORMAT    = SCI1_FORMAT_CONFIGVALUE;
        config_reg->CONFIG_BRS       = SCI1_BRS_CONFIGVALUE;
        config_reg->CONFIG_PIO0      = SCI1_PIO0_CONFIGVALUE;
        config_reg->CONFIG_PIO1      = SCI1_PIO1_CONFIGVALUE;
        config_reg->CONFIG_PIO6      = SCI1_PIO6_CONFIGVALUE;
        config_reg->CONFIG_PIO7      = SCI1_PIO7_CONFIGVALUE;
        config_reg->CONFIG_PIO8      = SCI1_PIO8_CONFIGVALUE;
    }
    else
    {
    /*SAFETYMCUSW 134 S MR:12.2 <APPROVED> "Register read back support" */
        config_reg->CONFIG_GCR0      = sciREG1->GCR0;
        config_reg->CONFIG_GCR1      = sciREG1->GCR1;
        config_reg->CONFIG_SETINT    = sciREG1->SETINT;
        config_reg->CONFIG_SETINTLVL = sciREG1->SETINTLVL;
        config_reg->CONFIG_FORMAT    = sciREG1->FORMAT;
        config_reg->CONFIG_BRS       = sciREG1->BRS;
        config_reg->CONFIG_PIO0      = sciREG1->PIO0;
        config_reg->CONFIG_PIO1      = sciREG1->PIO1;
        config_reg->CONFIG_PIO6      = sciREG1->PIO6;
        config_reg->CONFIG_PIO7         = sciREG1->PIO7;
        config_reg->CONFIG_PIO8      = sciREG1->PIO8;
    }
}

/* SourceId : SCI_SourceId_019 */
/* DesignId : SCI_DesignId_016 */
/* Requirements : HL_CONQ_SCI_SR24 */
/** @fn void sci2GetConfigValue(sci_config_reg_t *config_reg, config_value_type_t type)
*   @brief Get the initial or current values of the SCI2 configuration registers
*
*   @param[in] *config_reg: pointer to the struct to which the initial or current
*                           value of the configuration registers need to be stored
*   @param[in] type:    whether initial or current value of the configuration registers need to be stored
*                       - InitialValue: initial value of the configuration registers will be stored
*                                       in the struct pointed by config_reg
*                       - CurrentValue: initial value of the configuration registers will be stored
*                                       in the struct pointed by config_reg
*
*   This function will copy the initial or current value (depending on the parameter 'type')
*   of the configuration registers to the struct pointed by config_reg
*
*/

void sci2GetConfigValue(sci_config_reg_t *config_reg, config_value_type_t type)
{
    if (type == InitialValue)
    {
        config_reg->CONFIG_GCR0      = SCI2_GCR0_CONFIGVALUE;
        config_reg->CONFIG_GCR1      = SCI2_GCR1_CONFIGVALUE;
        config_reg->CONFIG_SETINT    = SCI2_SETINT_CONFIGVALUE;
        config_reg->CONFIG_SETINTLVL = SCI2_SETINTLVL_CONFIGVALUE;
        config_reg->CONFIG_FORMAT    = SCI2_FORMAT_CONFIGVALUE;
        config_reg->CONFIG_BRS       = SCI2_BRS_CONFIGVALUE;
        config_reg->CONFIG_PIO0      = SCI2_PIO0_CONFIGVALUE;
        config_reg->CONFIG_PIO1      = SCI2_PIO1_CONFIGVALUE;
        config_reg->CONFIG_PIO6      = SCI2_PIO6_CONFIGVALUE;
        config_reg->CONFIG_PIO7      = SCI2_PIO7_CONFIGVALUE;
        config_reg->CONFIG_PIO8      = SCI2_PIO8_CONFIGVALUE;
    }
    else
    {
    /*SAFETYMCUSW 134 S MR:12.2 <APPROVED> "Register read back support" */
        config_reg->CONFIG_GCR0      = sciREG2->GCR0;
        config_reg->CONFIG_GCR1      = sciREG2->GCR1;
        config_reg->CONFIG_SETINT    = sciREG2->SETINT;
        config_reg->CONFIG_SETINTLVL = sciREG2->SETINTLVL;
        config_reg->CONFIG_FORMAT    = sciREG2->FORMAT;
        config_reg->CONFIG_BRS       = sciREG2->BRS;
        config_reg->CONFIG_PIO0      = sciREG2->PIO0;
        config_reg->CONFIG_PIO1      = sciREG2->PIO1;
        config_reg->CONFIG_PIO6      = sciREG2->PIO6;
        config_reg->CONFIG_PIO7      = sciREG2->PIO7;
        config_reg->CONFIG_PIO8      = sciREG2->PIO8;
    }
}

/* SourceId : SCI_SourceId_020 */
/* DesignId : SCI_DesignId_016 */
/* Requirements : HL_CONQ_SCI_SR25 */
/** @fn void sci3GetConfigValue(sci_config_reg_t *config_reg, config_value_type_t type)
*   @brief Get the initial or current values of the SCI3 configuration registers
*
*    @param[in] *config_reg: pointer to the struct to which the initial or current
*                           value of the configuration registers need to be stored
*    @param[in] type:     whether initial or current value of the configuration registers need to be stored
*                        - InitialValue: initial value of the configuration registers will be stored
*                                       in the struct pointed by config_reg
*                        - CurrentValue: initial value of the configuration registers will be stored
*                                       in the struct pointed by config_reg
*
*   This function will copy the initial or current value (depending on the parameter 'type')
*   of the configuration registers to the struct pointed by config_reg
*
*/

void sci3GetConfigValue(sci_config_reg_t *config_reg, config_value_type_t type)
{
    if (type == InitialValue)
    {
        config_reg->CONFIG_GCR0      = SCI3_GCR0_CONFIGVALUE;
        config_reg->CONFIG_GCR1      = SCI3_GCR1_CONFIGVALUE;
        config_reg->CONFIG_SETINT    = SCI3_SETINT_CONFIGVALUE;
        config_reg->CONFIG_SETINTLVL = SCI3_SETINTLVL_CONFIGVALUE;
        config_reg->CONFIG_FORMAT    = SCI3_FORMAT_CONFIGVALUE;
        config_reg->CONFIG_BRS       = SCI3_BRS_CONFIGVALUE;
        config_reg->CONFIG_PIO0      = SCI3_PIO0_CONFIGVALUE;
        config_reg->CONFIG_PIO1      = SCI3_PIO1_CONFIGVALUE;
        config_reg->CONFIG_PIO6      = SCI3_PIO6_CONFIGVALUE;
        config_reg->CONFIG_PIO7      = SCI3_PIO7_CONFIGVALUE;
        config_reg->CONFIG_PIO8      = SCI3_PIO8_CONFIGVALUE;
    }
    else
    {
    /*SAFETYMCUSW 134 S MR:12.2 <APPROVED> "Register read back support" */
        config_reg->CONFIG_GCR0      = sciREG3->GCR0;
        config_reg->CONFIG_GCR1      = sciREG3->GCR1;
        config_reg->CONFIG_SETINT    = sciREG3->SETINT;
        config_reg->CONFIG_SETINTLVL = sciREG3->SETINTLVL;
        config_reg->CONFIG_FORMAT    = sciREG3->FORMAT;
        config_reg->CONFIG_BRS       = sciREG3->BRS;
        config_reg->CONFIG_PIO0      = sciREG3->PIO0;
        config_reg->CONFIG_PIO1      = sciREG3->PIO1;
        config_reg->CONFIG_PIO6      = sciREG3->PIO6;
        config_reg->CONFIG_PIO7      = sciREG3->PIO7;
        config_reg->CONFIG_PIO8      = sciREG3->PIO8;
    }
}

/* SourceId : SCI_SourceId_021 */
/* DesignId : SCI_DesignId_016 */
/* Requirements : HL_CONQ_SCI_SR26 */
/** @fn void sci4GetConfigValue(sci_config_reg_t *config_reg, config_value_type_t type)
*   @brief Get the initial or current values of the SCI4 configuration registers
*
*    @param[in] *config_reg: pointer to the struct to which the initial or current
*                           value of the configuration registers need to be stored
*    @param[in] type:     whether initial or current value of the configuration registers need to be stored
*                        - InitialValue: initial value of the configuration registers will be stored
*                                       in the struct pointed by config_reg
*                        - CurrentValue: initial value of the configuration registers will be stored
*                                       in the struct pointed by config_reg
*
*   This function will copy the initial or current value (depending on the parameter 'type')
*   of the configuration registers to the struct pointed by config_reg
*
*/

void sci4GetConfigValue(sci_config_reg_t *config_reg, config_value_type_t type)
{
    if (type == InitialValue)
    {
        config_reg->CONFIG_GCR0      = SCI4_GCR0_CONFIGVALUE;
        config_reg->CONFIG_GCR1      = SCI4_GCR1_CONFIGVALUE;
        config_reg->CONFIG_SETINT    = SCI4_SETINT_CONFIGVALUE;
        config_reg->CONFIG_SETINTLVL = SCI4_SETINTLVL_CONFIGVALUE;
        config_reg->CONFIG_FORMAT    = SCI4_FORMAT_CONFIGVALUE;
        config_reg->CONFIG_BRS       = SCI4_BRS_CONFIGVALUE;
        config_reg->CONFIG_PIO0      = SCI4_PIO0_CONFIGVALUE;
        config_reg->CONFIG_PIO1      = SCI4_PIO1_CONFIGVALUE;
        config_reg->CONFIG_PIO6      = SCI4_PIO6_CONFIGVALUE;
        config_reg->CONFIG_PIO7      = SCI4_PIO7_CONFIGVALUE;
        config_reg->CONFIG_PIO8      = SCI4_PIO8_CONFIGVALUE;
    }
    else
    {
    /*SAFETYMCUSW 134 S MR:12.2 <APPROVED> "Register read back support" */
        config_reg->CONFIG_GCR0      = sciREG4->GCR0;
        config_reg->CONFIG_GCR1      = sciREG4->GCR1;
        config_reg->CONFIG_SETINT    = sciREG4->SETINT;
        config_reg->CONFIG_SETINTLVL = sciREG4->SETINTLVL;
        config_reg->CONFIG_FORMAT    = sciREG4->FORMAT;
        config_reg->CONFIG_BRS       = sciREG4->BRS;
        config_reg->CONFIG_PIO0      = sciREG4->PIO0;
        config_reg->CONFIG_PIO1      = sciREG4->PIO1;
        config_reg->CONFIG_PIO6      = sciREG4->PIO6;
        config_reg->CONFIG_PIO7      = sciREG4->PIO7;
        config_reg->CONFIG_PIO8      = sciREG4->PIO8;
    }
}

/* USER CODE BEGIN (27) */
/* USER CODE END */

/* SourceId : SCI_SourceId_022 */
/* DesignId : SCI_DesignId_017 */
/* Requirements : HL_CONQ_SCI_SR20, HL_CONQ_SCI_SR21 */
/** @fn void lin1HighLevelInterrupt(void)
*   @brief Level 0 Interrupt for SCI1
*/
#pragma CODE_STATE(lin1HighLevelInterrupt, 32)
#pragma INTERRUPT(lin1HighLevelInterrupt, IRQ)

void lin1HighLevelInterrupt(void)
{
    uint32 vec = sciREG1->INTVECT0;
    uint8 byte;
/* USER CODE BEGIN (28) */
/* USER CODE END */

    switch (vec)
    {
    case 1U:
        sciNotification(sciREG1, (uint32)SCI_WAKE_INT);
        break;
    case 3U:
        sciNotification(sciREG1, (uint32)SCI_PE_INT);
        break;
    case 6U:
        sciNotification(sciREG1, (uint32)SCI_FE_INT);
        break;
    case 7U:
        sciNotification(sciREG1, (uint32)SCI_BREAK_INT);
        break;
    case 9U:
        sciNotification(sciREG1, (uint32)SCI_OE_INT);
        break;

    case 11U:
        /* receive */
        byte = (uint8)(sciREG1->RD & 0x000000FFU);

            if (g_sciTransfer_t[0U].rx_length > 0U)
            {
                *g_sciTransfer_t[0U].rx_data = byte;
                g_sciTransfer_t[0U].rx_data++;

                g_sciTransfer_t[0U].rx_length--;
                if (g_sciTransfer_t[0U].rx_length == 0U)
                {
                    sciNotification(sciREG1, (uint32)SCI_RX_INT);
                }
            }

        break;

    case 12U:
        /* transmit */
        /*SAFETYMCUSW 30 S MR:12.2,12.3 <APPROVED> "Used for data count in Transmit/Receive polling and Interrupt mode" */
        --g_sciTransfer_t[0U].tx_length;
        if (g_sciTransfer_t[0U].tx_length > 0U)
        {
            uint8 txdata = *g_sciTransfer_t[0U].tx_data;
            sciREG1->TD = (uint32)txdata;
            g_sciTransfer_t[0U].tx_data++;
        }
        else
        {
            sciREG1->CLEARINT = SCI_TX_INT;
            sciNotification(sciREG1, (uint32)SCI_TX_INT);
        }
        break;

    default:
        /* phantom interrupt, clear flags and return */
        sciREG1->FLR = sciREG1->SETINTLVL & 0x07000303U;
         break;
    }
/* USER CODE BEGIN (29) */
/* USER CODE END */
}


/* SourceId : SCI_SourceId_024 */
/* DesignId : SCI_DesignId_017 */
/* Requirements : HL_CONQ_SCI_SR20, HL_CONQ_SCI_SR21 */
/** @fn void lin2HighLevelInterrupt(void)
*   @brief  Level 0 Interrupt for SCI2
*/
#pragma CODE_STATE(lin2HighLevelInterrupt, 32)
#pragma INTERRUPT(lin2HighLevelInterrupt, IRQ)
void lin2HighLevelInterrupt(void)
{
    uint32 vec = sciREG2->INTVECT0;
    uint8 byte;
/* USER CODE BEGIN (33) */
/* USER CODE END */

    switch (vec)
    {
    case 1U:
        sciNotification(sciREG2, (uint32)SCI_WAKE_INT);
        break;
    case 3U:
        sciNotification(sciREG2, (uint32)SCI_PE_INT);
        break;
    case 6U:
        sciNotification(sciREG2, (uint32)SCI_FE_INT);
        break;
    case 7U:
        sciNotification(sciREG2, (uint32)SCI_BREAK_INT);
        break;
    case 9U:
        sciNotification(sciREG2, (uint32)SCI_OE_INT);
        break;

    case 11U:
        /* receive */
        byte = (uint8)(sciREG2->RD & 0x000000FFU);

            if (g_sciTransfer_t[1U].rx_length > 0U)
            {
                *g_sciTransfer_t[1U].rx_data = byte;
                g_sciTransfer_t[1U].rx_data++;
                g_sciTransfer_t[1U].rx_length--;
                if (g_sciTransfer_t[1U].rx_length == 0U)
                {
                    sciNotification(sciREG2, (uint32)SCI_RX_INT);
                }
            }

        break;

    case 12U:
        /* transmit */
        /*SAFETYMCUSW 30 S MR:12.2,12.3 <APPROVED> "Used for data count in Transmit/Receive polling and Interrupt mode" */
        --g_sciTransfer_t[1U].tx_length;
        if (g_sciTransfer_t[1U].tx_length > 0U)
        {
            uint8 txdata = *g_sciTransfer_t[1U].tx_data;
            sciREG2->TD = (uint32)txdata;
            g_sciTransfer_t[1U].tx_data++;
        }
        else
        {
            sciREG2->CLEARINT = SCI_TX_INT;
            sciNotification(sciREG2, (uint32)SCI_TX_INT);
        }
        break;

    default:
        /* phantom interrupt, clear flags and return */
        sciREG2->FLR = sciREG2->SETINTLVL & 0x07000303U;
         break;
    }
/* USER CODE BEGIN (34) */
/* USER CODE END */
}


/* SourceId : SCI_SourceId_026 */
/* DesignId : SCI_DesignId_017 */
/* Requirements : HL_CONQ_SCI_SR20, HL_CONQ_SCI_SR21 */
/** @fn void sci3HighLevelInterrupt(void)
*   @brief  Level 0 Interrupt for SCI3
*/
#pragma CODE_STATE(sci3HighLevelInterrupt, 32)
#pragma INTERRUPT(sci3HighLevelInterrupt, IRQ)
void sci3HighLevelInterrupt(void)
{
    uint32 vec = sciREG3->INTVECT0;
    uint8 byte;
/* USER CODE BEGIN (37) */
/* USER CODE END */

    switch (vec)
    {
    case 1U:
        sciNotification(sciREG3, (uint32)SCI_WAKE_INT);
        break;
    case 3U:
        sciNotification(sciREG3, (uint32)SCI_PE_INT);
        break;
    case 6U:
        sciNotification(sciREG3, (uint32)SCI_FE_INT);
        break;
    case 7U:
        sciNotification(sciREG3, (uint32)SCI_BREAK_INT);
        break;
    case 9U:
        sciNotification(sciREG3, (uint32)SCI_OE_INT);
        break;

    case 11U:
        /* receive */
        byte = (uint8)(sciREG3->RD & 0x000000FFU);

            if (g_sciTransfer_t[2U].rx_length > 0U)
            {
                *g_sciTransfer_t[2U].rx_data = byte;
                g_sciTransfer_t[2U].rx_data++;
                g_sciTransfer_t[2U].rx_length--;
                if (g_sciTransfer_t[2U].rx_length == 0U)
                {
                    sciNotification(sciREG3, (uint32)SCI_RX_INT);
                }
            }

        break;

    case 12U:
        /* transmit */
        /*SAFETYMCUSW 30 S MR:12.2,12.3 <APPROVED> "Used for data count in Transmit/Receive polling and Interrupt mode" */
        --g_sciTransfer_t[2U].tx_length;
        if (g_sciTransfer_t[2U].tx_length > 0U)
        {
            uint8 txdata = *g_sciTransfer_t[2U].tx_data;
            sciREG3->TD = (uint32)txdata;
            g_sciTransfer_t[2U].tx_data++;
        }
        else
        {
            sciREG3->CLEARINT = (uint32)SCI_TX_INT;
            sciNotification(sciREG3, (uint32)SCI_TX_INT);
        }
        break;

    default:
        /* phantom interrupt, clear flags and return */
        sciREG3->FLR = sciREG3->SETINTLVL & 0x07000303U;
         break;
    }
/* USER CODE BEGIN (38) */
/* USER CODE END */
}


/* SourceId : SCI_SourceId_028 */
/* DesignId : SCI_DesignId_017 */
/* Requirements : HL_CONQ_SCI_SR20, HL_CONQ_SCI_SR21 */
/** @fn void sci4HighLevelInterrupt(void)
*   @brief  Level 0 Interrupt for SCI4
*/
#pragma CODE_STATE(sci4HighLevelInterrupt, 32)
#pragma INTERRUPT(sci4HighLevelInterrupt, IRQ)
void sci4HighLevelInterrupt(void)
{
    uint32 vec = sciREG4->INTVECT0;
    uint8 byte;
/* USER CODE BEGIN (41) */
/* USER CODE END */

    switch (vec)
    {
    case 1U:
        sciNotification(sciREG4, (uint32)SCI_WAKE_INT);
        break;
    case 3U:
        sciNotification(sciREG4, (uint32)SCI_PE_INT);
        break;
    case 6U:
        sciNotification(sciREG4, (uint32)SCI_FE_INT);
        break;
    case 7U:
        sciNotification(sciREG4, (uint32)SCI_BREAK_INT);
        break;
    case 9U:
        sciNotification(sciREG4, (uint32)SCI_OE_INT);
        break;

    case 11U:
        /* receive */
        byte = (uint8)(sciREG4->RD & 0x000000FFU);

            if (g_sciTransfer_t[3U].rx_length > 0U)
            {
                *g_sciTransfer_t[3U].rx_data = byte;
                g_sciTransfer_t[3U].rx_data++;
                g_sciTransfer_t[3U].rx_length--;
                if (g_sciTransfer_t[3U].rx_length == 0U)
                {
                    sciNotification(sciREG4, (uint32)SCI_RX_INT);
                }
            }

        break;

    case 12U:
        /* transmit */
        /*SAFETYMCUSW 30 S MR:12.2,12.3 <APPROVED> "Used for data count in Transmit/Receive polling and Interrupt mode" */
        --g_sciTransfer_t[3U].tx_length;
        if (g_sciTransfer_t[3U].tx_length > 0U)
        {
            uint8 txdata = *g_sciTransfer_t[3U].tx_data;
            sciREG4->TD = (uint32)txdata;
            g_sciTransfer_t[3U].tx_data++;
        }
        else
        {
            sciREG4->CLEARINT = (uint32)SCI_TX_INT;
            sciNotification(sciREG4, (uint32)SCI_TX_INT);
        }
        break;

    default:
        /* phantom interrupt, clear flags and return */
        sciREG4->FLR = sciREG4->SETINTLVL & 0x07000303U;
         break;
    }
/* USER CODE BEGIN (42) */
/* USER CODE END */
}

/*----------------------------------------------------------------------------\
|   Private Function Implementations                                          |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   End of fw_uart.c module                                                   |
\----------------------------------------------------------------------------*/

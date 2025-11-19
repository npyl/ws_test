/*----------------------------------------------------------------------------\
|          Copyright(c) 2024 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : fw_spi.c Module File.                                      |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 11 …·Ì 2024                                                |
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

#include "FreeRTOS.h"
#include "os_queue.h"

#include "fw_spi.h"

/*----------------------------------------------------------------------------\
|   Public Constant Definitions                                               |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Public Data Declarations                                                  |
\----------------------------------------------------------------------------*/

xQueueHandle xSPIQueueHandle;

/*----------------------------------------------------------------------------\
|   Private Type Definitions                                                  |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Private Constant Definitions                                              |
\----------------------------------------------------------------------------*/

static const S_SPI_CONFIG spi_config_defs[ eSPI_MAX ] =
{
     {
          .id = eSPI_0,
          .label = "SPI1",
          .format =
          {
               {    /* SPI Format 0 */
                    .baud_prescale = e1000_kHz,
                    .w_delay = 0u,
                    .char_len = e8_BIT,
                    .parity_en = FALSE,
                    .clk_polarity = FALSE,
                    .wait_for_en = FALSE,
                    .odd_parity = FALSE,
                    .clk_phase = FALSE,
               },
               {    /* SPI Format 1 */
                    .baud_prescale = e1000_kHz,
                    .w_delay = 0u,
                    .char_len = e8_BIT,
                    .parity_en = FALSE,
                    .clk_polarity = FALSE,
                    .wait_for_en = FALSE,
                    .odd_parity = FALSE,
                    .clk_phase = FALSE,
               },
               {    /* SPI Format 2 */
                    .baud_prescale = e1000_kHz,
                    .w_delay = 0u,
                    .char_len = e8_BIT,
                    .parity_en = FALSE,
                    .clk_polarity = FALSE,
                    .wait_for_en = FALSE,
                    .odd_parity = FALSE,
                    .clk_phase = FALSE,
               },
               {    /* SPI Format 3 */
                    .baud_prescale = e1000_kHz,
                    .w_delay = 0u,
                    .char_len = e8_BIT,
                    .parity_en = FALSE,
                    .clk_polarity = FALSE,
                    .wait_for_en = FALSE,
                    .odd_parity = FALSE,
                    .clk_phase = FALSE,
               },
          },
          .selected_format = eSPI_FORMAT_0,
          .delay =
          {
               .cs_to_tx_start = 0u,
               .tx_end_to_cs_inactive = 0u,
               .cs_to_ena_timeout = 0u,
               .tx_finish_to_ena_inactive_timeout = 0u,
          },
          .loopback = FALSE,
          .enabled = FALSE,
     },
     {
          .id = eSPI_1,
          .label = "SPI2",
          .format =
          {
               {    /* SPI Format 0 */
                    .baud_prescale = e1000_kHz,
                    .w_delay = 0u,
                    .char_len = e8_BIT,
                    .parity_en = FALSE,
                    .clk_polarity = FALSE,
                    .wait_for_en = FALSE,
                    .odd_parity = FALSE,
                    .clk_phase = FALSE,
               },
               {    /* SPI Format 1 */
                    .baud_prescale = e1000_kHz,
                    .w_delay = 0u,
                    .char_len = e8_BIT,
                    .parity_en = FALSE,
                    .clk_polarity = FALSE,
                    .wait_for_en = FALSE,
                    .odd_parity = FALSE,
                    .clk_phase = FALSE,
               },
               {    /* SPI Format 2 */
                    .baud_prescale = e1000_kHz,
                    .w_delay = 0u,
                    .char_len = e8_BIT,
                    .parity_en = FALSE,
                    .clk_polarity = FALSE,
                    .wait_for_en = FALSE,
                    .odd_parity = FALSE,
                    .clk_phase = FALSE,
               },
               {    /* SPI Format 3 */
                    .baud_prescale = e1000_kHz,
                    .w_delay = 0u,
                    .char_len = e8_BIT,
                    .parity_en = FALSE,
                    .clk_polarity = FALSE,
                    .wait_for_en = FALSE,
                    .odd_parity = FALSE,
                    .clk_phase = FALSE,
               },
          },
          .selected_format = eSPI_FORMAT_0,
          .delay =
          {
               .cs_to_tx_start = 0u,
               .tx_end_to_cs_inactive = 0u,
               .cs_to_ena_timeout = 0u,
               .tx_finish_to_ena_inactive_timeout = 0u,
          },
          .loopback = FALSE,
          .enabled = FALSE,
     },
     {
          .id = eSPI_2,
          .label = "SPI3",
          .format =
          {
               {    /* SPI Format 0 */
                    .baud_prescale = e1000_kHz,
                    .w_delay = 0u,
                    .char_len = e8_BIT,
                    .parity_en = FALSE,
                    .clk_polarity = FALSE,
                    .wait_for_en = FALSE,
                    .odd_parity = FALSE,
                    .clk_phase = FALSE,
               },
               {    /* SPI Format 1 */
                    .baud_prescale = e1000_kHz,
                    .w_delay = 0u,
                    .char_len = e8_BIT,
                    .parity_en = FALSE,
                    .clk_polarity = FALSE,
                    .wait_for_en = FALSE,
                    .odd_parity = FALSE,
                    .clk_phase = FALSE,
               },
               {    /* SPI Format 2 */
                    .baud_prescale = e1000_kHz,
                    .w_delay = 0u,
                    .char_len = e8_BIT,
                    .parity_en = FALSE,
                    .clk_polarity = FALSE,
                    .wait_for_en = FALSE,
                    .odd_parity = FALSE,
                    .clk_phase = FALSE,
               },
               {    /* SPI Format 3 */
                    .baud_prescale = e1000_kHz,
                    .w_delay = 0u,
                    .char_len = e8_BIT,
                    .parity_en = FALSE,
                    .clk_polarity = FALSE,
                    .wait_for_en = FALSE,
                    .odd_parity = FALSE,
                    .clk_phase = FALSE,
               },
          },
          .selected_format = eSPI_FORMAT_0,
          .delay =
          {
               .cs_to_tx_start = 0u,
               .tx_end_to_cs_inactive = 0u,
               .cs_to_ena_timeout = 0u,
               .tx_finish_to_ena_inactive_timeout = 0u,
          },
          .loopback = FALSE,
          .enabled = TRUE,
     },
     {
          .id = eSPI_3,
          .label = "SPI4",
          .format =
          {
               {    /* SPI Format 0 */
                    .baud_prescale = e1000_kHz,
                    .w_delay = 0u,
                    .char_len = e8_BIT,
                    .parity_en = FALSE,
                    .clk_polarity = FALSE,
                    .wait_for_en = FALSE,
                    .odd_parity = FALSE,
                    .clk_phase = FALSE,
               },
               {    /* SPI Format 1 */
                    .baud_prescale = e1000_kHz,
                    .w_delay = 0u,
                    .char_len = e8_BIT,
                    .parity_en = FALSE,
                    .clk_polarity = FALSE,
                    .wait_for_en = FALSE,
                    .odd_parity = FALSE,
                    .clk_phase = FALSE,
               },
               {    /* SPI Format 2 */
                    .baud_prescale = e1000_kHz,
                    .w_delay = 0u,
                    .char_len = e8_BIT,
                    .parity_en = FALSE,
                    .clk_polarity = FALSE,
                    .wait_for_en = FALSE,
                    .odd_parity = FALSE,
                    .clk_phase = FALSE,
               },
               {    /* SPI Format 3 */
                    .baud_prescale = e1000_kHz,
                    .w_delay = 0u,
                    .char_len = e8_BIT,
                    .parity_en = FALSE,
                    .clk_polarity = FALSE,
                    .wait_for_en = FALSE,
                    .odd_parity = FALSE,
                    .clk_phase = FALSE,
               },
          },
          .selected_format = eSPI_FORMAT_0,
          .delay =
          {
               .cs_to_tx_start = 0u,
               .tx_end_to_cs_inactive = 0u,
               .cs_to_ena_timeout = 0u,
               .tx_finish_to_ena_inactive_timeout = 0u,
          },
          .loopback = FALSE,
          .enabled = FALSE,
     },
     {
          .id = eSPI_4,
          .label = "SPI5",
          .format =
          {
               {    /* SPI Format 0 */
                    .baud_prescale = e1000_kHz,
                    .w_delay = 0u,
                    .char_len = e8_BIT,
                    .parity_en = FALSE,
                    .clk_polarity = FALSE,
                    .wait_for_en = FALSE,
                    .odd_parity = FALSE,
                    .clk_phase = FALSE,
               },
               {    /* SPI Format 1 */
                    .baud_prescale = e1000_kHz,
                    .w_delay = 0u,
                    .char_len = e8_BIT,
                    .parity_en = FALSE,
                    .clk_polarity = FALSE,
                    .wait_for_en = FALSE,
                    .odd_parity = FALSE,
                    .clk_phase = FALSE,
               },
               {    /* SPI Format 2 */
                    .baud_prescale = e1000_kHz,
                    .w_delay = 0u,
                    .char_len = e8_BIT,
                    .parity_en = FALSE,
                    .clk_polarity = FALSE,
                    .wait_for_en = FALSE,
                    .odd_parity = FALSE,
                    .clk_phase = FALSE,
               },
               {    /* SPI Format 3 */
                    .baud_prescale = e1000_kHz,
                    .w_delay = 0u,
                    .char_len = e8_BIT,
                    .parity_en = FALSE,
                    .clk_polarity = FALSE,
                    .wait_for_en = FALSE,
                    .odd_parity = FALSE,
                    .clk_phase = FALSE,
               },
          },
          .selected_format = eSPI_FORMAT_0,
          .delay =
          {
               .cs_to_tx_start = 0u,
               .tx_end_to_cs_inactive = 0u,
               .cs_to_ena_timeout = 0u,
               .tx_finish_to_ena_inactive_timeout = 0u,
          },
          .loopback = FALSE,
          .enabled = FALSE,
     },
};

/*----------------------------------------------------------------------------\
|   Private Data Declarations                                                 |
\----------------------------------------------------------------------------*/

static uint8 spi_rx_buffer[ eSPI_MAX ][ sizeof( S_SPI_INFO ) ];
static uint8 spi_rx_buffer_idx[ eSPI_MAX ];

/** @struct g_spiPacket
*   @brief globals
*
*/
static volatile struct g_spiPacket
{
    spiDAT1_t g_spiDataFormat;
    uint32  tx_length;
    uint32  rx_length;
    uint16  * txdata_ptr;
    uint16  * rxdata_ptr;
    SpiDataStatus_t tx_data_status;
    SpiDataStatus_t rx_data_status;
} g_spiPacket_t[5U];

/*----------------------------------------------------------------------------\
|   Private Function Declarations                                             |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Public Function Implementations                                           |
\----------------------------------------------------------------------------*/

void spi_init( void )
{
    int i;
    spiBASE_t * p_spi;

    xSPIQueueHandle = xQueueCreate( SPI_QUEUE_LENGTH, SPI_QUEUE_ITEM_SIZE );
    const S_SPI_CONFIG * const p_cfg = spiGetConfig();

    for ( i = 0; i < eSPI_MAX; i++ )
    {
        p_spi = SPI( i );
        if ( p_cfg[ i ].enabled == TRUE )
        {
            /* Bring SPI out of reset */
            p_spi->GCR0 = 0U;
            p_spi->GCR0 = 1U;

            /* Master mode and clock configuration */
            /* CLOKMOD, MASTER */
            p_spi->GCR1 = ( p_spi->GCR1 & 0xFFFFFFFCU ) | ( ( uint32 ) ( ( uint32 ) 1U << 1U ) | 1U );

            /* SPI Enable pin configuration */
            p_spi->INT0 = ( p_spi->INT0 & 0xFEFFFFFFU ) | ( uint32 ) ( ( uint32 ) 0U << 24U );

            /* SPI Delays */
            p_spi->DELAY = ( uint32 ) ( ( uint32 ) p_cfg[ i ].delay.cs_to_tx_start << 24U )
                         | ( uint32 ) ( ( uint32 ) p_cfg[ i ].delay.tx_end_to_cs_inactive << 16U )
                         | ( uint32 ) ( ( uint32 ) p_cfg[ i ].delay.tx_finish_to_ena_inactive_timeout << 8U )
                         | ( uint32 ) ( ( uint32 ) p_cfg[ i ].delay.cs_to_ena_timeout << 0U );

            /* SPI Format 0 */
            p_spi->FMT0 = ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_0 ].w_delay << 24U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_0 ].odd_parity << 23U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_0 ].parity_en << 22U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_0 ].wait_for_en << 21U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_0 ].LSB_first << 20U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_0 ].clk_polarity << 17U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_0 ].clk_phase << 16U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_0 ].baud_prescale << 8U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_0 ].char_len << 8U );

            /* SPI Format 1 */
            p_spi->FMT1 = ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_1 ].w_delay << 24U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_1 ].odd_parity << 23U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_1 ].parity_en << 22U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_1 ].wait_for_en << 21U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_1 ].LSB_first << 20U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_1 ].clk_polarity << 17U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_1 ].clk_phase << 16U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_1 ].baud_prescale << 8U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_1 ].char_len << 8U );

            /* SPI Format 2 */
            p_spi->FMT2 = ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_2 ].w_delay << 24U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_2 ].odd_parity << 23U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_2 ].parity_en << 22U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_2 ].wait_for_en << 21U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_2 ].LSB_first << 20U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_2 ].clk_polarity << 17U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_2 ].clk_phase << 16U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_2 ].baud_prescale << 8U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_2 ].char_len << 8U );

            /* SPI Format 3 */
            p_spi->FMT3 = ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_3 ].w_delay << 24U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_3 ].odd_parity << 23U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_3 ].parity_en << 22U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_3 ].wait_for_en << 21U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_3 ].LSB_first << 20U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_3 ].clk_polarity << 17U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_3 ].clk_phase << 16U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_3 ].baud_prescale << 8U )
                        | ( uint32 ) ( ( uint32 ) p_cfg[ i ].format[ eSPI_FORMAT_3 ].char_len << 8U );

            /* Set interrupt levels */
            p_spi->LVL = ( uint32 ) ( ( uint32 ) 0U << 9U )    /* TXINT */
                       | ( uint32 ) ( ( uint32 ) 0U << 8U )    /* RXINT */
                       | ( uint32 ) ( ( uint32 ) 0U << 6U )    /* OVRNINT */
                       | ( uint32 ) ( ( uint32 ) 0U << 4U )    /* BITERR */
                       | ( uint32 ) ( ( uint32 ) 0U << 3U )    /* DESYNC */
                       | ( uint32 ) ( ( uint32 ) 0U << 2U )    /* PARERR */
                       | ( uint32 ) ( ( uint32 ) 0U << 1U )    /* TIMEOUT */
                       | ( uint32 ) ( ( uint32 ) 0U << 0U );   /* DLENERR */

            /* Clear any pending interrupts */
            p_spi->FLG |= 0xFFFFU;

            /* Enable interrupts */
            p_spi->INT0 = ( p_spi->INT0 & 0xFFFF0000U )
                        | ( uint32 ) ( ( uint32 ) 0U << 9U )   /* TXINT */
                        | ( uint32 ) ( ( uint32 ) 0U << 8U )   /* RXINT */
                        | ( uint32 ) ( ( uint32 ) 0U << 6U )   /* OVRNINT */
                        | ( uint32 ) ( ( uint32 ) 0U << 4U )   /* BITERR */
                        | ( uint32 ) ( ( uint32 ) 0U << 3U )   /* DESYNC */
                        | ( uint32 ) ( ( uint32 ) 0U << 2U )   /* PARERR */
                        | ( uint32 ) ( ( uint32 ) 0U << 1U )   /* TIMEOUT */
                        | ( uint32 ) ( ( uint32 ) 0U << 0U );  /* DLENERR */

            /* Initialize SPI Port */
            /* SPI Port output values */
            p_spi->PC3 = ( uint32 ) ( ( uint32 ) 1U << 0U )    /* SCS[0] */
                       | ( uint32 ) ( ( uint32 ) 1U << 1U )    /* SCS[1] */
                       | ( uint32 ) ( ( uint32 ) 1U << 2U )    /* SCS[2] */
                       | ( uint32 ) ( ( uint32 ) 1U << 3U )    /* SCS[3] */
                       | ( uint32 ) ( ( uint32 ) 1U << 4U )    /* SCS[4] */
                       | ( uint32 ) ( ( uint32 ) 1U << 5U )    /* SCS[5] */
                       | ( uint32 ) ( ( uint32 ) 0U << 8U )    /* ENA */
                       | ( uint32 ) ( ( uint32 ) 0U << 9U )    /* CLK */
                       | ( uint32 ) ( ( uint32 ) 0U << 10U )   /* SIMO[0] */
                       | ( uint32 ) ( ( uint32 ) 0U << 11U )   /* SOMI[0] */
                       | ( uint32 ) ( ( uint32 ) 0U << 17U )   /* SIMO[1] */
                       | ( uint32 ) ( ( uint32 ) 0U << 25U );  /* SOMI[1] */

            /* SPI Port direction */
            p_spi->PC1 = ( uint32 ) ( ( uint32 ) 1U << 0U )    /* SCS[0] */
                       | ( uint32 ) ( ( uint32 ) 1U << 1U )    /* SCS[1] */
                       | ( uint32 ) ( ( uint32 ) 1U << 2U )    /* SCS[2] */
                       | ( uint32 ) ( ( uint32 ) 1U << 3U )    /* SCS[3] */
                       | ( uint32 ) ( ( uint32 ) 1U << 4U )    /* SCS[4] */
                       | ( uint32 ) ( ( uint32 ) 1U << 5U )    /* SCS[5] */
                       | ( uint32 ) ( ( uint32 ) 0U << 8U )    /* ENA */
                       | ( uint32 ) ( ( uint32 ) 1U << 9U )    /* CLK */
                       | ( uint32 ) ( ( uint32 ) 1U << 10U )   /* SIMO[0] */
                       | ( uint32 ) ( ( uint32 ) 0U << 11U )   /* SOMI[0] */
                       | ( uint32 ) ( ( uint32 ) 0U << 17U )   /* SIMO[1] */
                       | ( uint32 ) ( ( uint32 ) 0U << 25U );  /* SOMI[1] */

            /* SPI Port open drain enable */
            p_spi->PC6 = ( uint32 ) ( ( uint32 ) 0U << 0U )    /* SCS[0] */
                       | ( uint32 ) ( ( uint32 ) 0U << 1U )    /* SCS[1] */
                       | ( uint32 ) ( ( uint32 ) 0U << 2U )    /* SCS[2] */
                       | ( uint32 ) ( ( uint32 ) 0U << 3U )    /* SCS[3] */
                       | ( uint32 ) ( ( uint32 ) 0U << 4U )    /* SCS[4] */
                       | ( uint32 ) ( ( uint32 ) 0U << 5U )    /* SCS[5] */
                       | ( uint32 ) ( ( uint32 ) 0U << 8U )    /* ENA */
                       | ( uint32 ) ( ( uint32 ) 0U << 9U )    /* CLK */
                       | ( uint32 ) ( ( uint32 ) 0U << 10U )   /* SIMO[0] */
                       | ( uint32 ) ( ( uint32 ) 0U << 11U )   /* SOMI[0] */
                       | ( uint32 ) ( ( uint32 ) 0U << 17U )   /* SIMO[1] */
                       | ( uint32 ) ( ( uint32 ) 0U << 25U );  /* SOMI[1] */

            /* SPI Port pullup / pulldown selection */
            p_spi->PC8 = ( uint32 ) ( ( uint32 ) 1U << 0U )    /* SCS[0] */
                       | ( uint32 ) ( ( uint32 ) 1U << 1U )    /* SCS[1] */
                       | ( uint32 ) ( ( uint32 ) 1U << 2U )    /* SCS[2] */
                       | ( uint32 ) ( ( uint32 ) 1U << 3U )    /* SCS[3] */
                       | ( uint32 ) ( ( uint32 ) 1U << 4U )    /* SCS[4] */
                       | ( uint32 ) ( ( uint32 ) 1U << 5U )    /* SCS[5] */
                       | ( uint32 ) ( ( uint32 ) 1U << 8U )    /* ENA */
                       | ( uint32 ) ( ( uint32 ) 1U << 9U )    /* CLK */
                       | ( uint32 ) ( ( uint32 ) 1U << 10U )   /* SIMO[0] */
                       | ( uint32 ) ( ( uint32 ) 1U << 11U )   /* SOMI[0] */
                       | ( uint32 ) ( ( uint32 ) 1U << 17U )   /* SIMO[1] */
                       | ( uint32 ) ( ( uint32 ) 1U << 25U );  /* SOMI[1] */

            /* SPI Port pullup / pulldown enable */
            p_spi->PC7 = ( uint32 ) ( ( uint32 ) 0U << 0U )    /* SCS[0] */
                       | ( uint32 ) ( ( uint32 ) 0U << 1U )    /* SCS[1] */
                       | ( uint32 ) ( ( uint32 ) 0U << 2U )    /* SCS[2] */
                       | ( uint32 ) ( ( uint32 ) 0U << 3U )    /* SCS[3] */
                       | ( uint32 ) ( ( uint32 ) 0U << 4U )    /* SCS[4] */
                       | ( uint32 ) ( ( uint32 ) 0U << 5U )    /* SCS[5] */
                       | ( uint32 ) ( ( uint32 ) 0U << 8U )    /* ENA */
                       | ( uint32 ) ( ( uint32 ) 0U << 9U )    /* CLK */
                       | ( uint32 ) ( ( uint32 ) 0U << 10U )   /* SIMO[0] */
                       | ( uint32 ) ( ( uint32 ) 0U << 11U )   /* SOMI[0] */
                       | ( uint32 ) ( ( uint32 ) 0U << 17U )   /* SIMO[1] */
                       | ( uint32 ) ( ( uint32 ) 0U << 25U );  /* SOMI[1] */

            /* SPI set all pins to functional */
            p_spi->PC0 = ( uint32 ) ( ( uint32 ) 1U << 0U )    /* SCS[0] */
                       | ( uint32 ) ( ( uint32)1U << 1U )      /* SCS[1] */
                       | ( uint32 ) ( ( uint32)1U << 2U )      /* SCS[2] */
                       | ( uint32 ) ( ( uint32)1U << 3U )      /* SCS[3] */
                       | ( uint32 ) ( ( uint32)1U << 4U )      /* SCS[4] */
                       | ( uint32 ) ( ( uint32)1U << 5U )      /* SCS[5] */
                       | ( uint32 ) ( ( uint32)1U << 8U )      /* ENA */
                       | ( uint32 ) ( ( uint32)1U << 9U )      /* CLK */
                       | ( uint32 ) ( ( uint32)1U << 10U )     /* SIMO[0] */
                       | ( uint32 ) ( ( uint32)1U << 11U )     /* SOMI[0] */
                       | ( uint32 ) ( ( uint32)1U << 17U )     /* SIMO[1] */
                       | ( uint32 ) ( ( uint32)1U << 25U );    /* SOMI[1] */

            /* Initialize TX and RX data buffer Status */
            g_spiPacket_t[ i ].tx_data_status  = SPI_READY;
            g_spiPacket_t[ i ].rx_data_status  = SPI_READY;

            /* Finally start SPI */
            p_spi->GCR1 = ( p_spi->GCR1 & 0xFEFFFFFFU ) | 0x01000000U;
        }
    }
}

const S_SPI_CONFIG * const spiGetConfig( void )
{
    /* Return the address of the SPI configuration data */
    return spi_config_defs;
}

void spiNotification( spiBASE_t *spi, uint32 flags )
{
    S_SPI_INFO spi_info;

    /* Determine which SPI RX buffer array index to use:
     *   eSPI_0 .. eSPI_4 corresponding to spiREG1 .. sciREG5
     */
    E_SPI_ID spi_idx = SPI_IDX( spi );

    if ( flags & SPI_COMPLETED )
    {
    }
}

void spiEndNotification( spiBASE_t * spi )
{
    SpiDataStatus_t RxStatus_e = SpiRxStatus( spi );

    if ( RxStatus_e == SPI_COMPLETED )
    {
    }
}

/** @fn void spiSetFunctional(spiBASE_t *spi, uint32 port)
*   @brief Change functional behavior of pins at runtime.
*   @param[in] spi   - Spi module base address
*   @param[in] port  - Value to write to PC0 register
*
*   Change the value of the PC0 register at runtime, this allows to
*   dynamically change the functionality of the SPI pins between functional
*   and GIO mode.
*/
/* SourceId : SPI_SourceId_002 */
/* DesignId : SPI_DesignId_002 */
/* Requirements : HL_CONQ_SPI_SR9 */
void spiSetFunctional(spiBASE_t *spi, uint32 port)
{
/* USER CODE BEGIN (4) */
/* USER CODE END */

    spi->PC0 = port;

/* USER CODE BEGIN (5) */
/* USER CODE END */
}


/** @fn uint32 spiReceiveData(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint32 blocksize, uint16 * destbuff)
*   @brief Receives Data using polling method
*   @param[in] spi           - Spi module base address
*   @param[in] dataconfig_t    - Spi DAT1 register configuration
*   @param[in] blocksize    - number of data
*   @param[in] destbuff        - Pointer to the destination data (16 bit).
*
*   @return flag register value.
*
*   This function transmits blocksize number of data from source buffer using polling method.
*/
/* SourceId : SPI_SourceId_003 */
/* DesignId : SPI_DesignId_007 */
/* Requirements : HL_CONQ_SPI_SR14 */
uint32 spiReceiveData(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint32 blocksize, uint16 * destbuff)
{
/* USER CODE BEGIN (6) */
/* USER CODE END */
    uint32 Chip_Select_Hold = (dataconfig_t->CS_HOLD) ? 0x10000000U : 0U;
    uint32 WDelay = (dataconfig_t->WDEL) ? 0x04000000U : 0U;
    SPIDATAFMT_t DataFormat = dataconfig_t->DFSEL;
    uint8 ChipSelect = dataconfig_t->CSNR;

    while(blocksize != 0U)
    {
        if((spi->FLG & 0x000000FFU) !=0U)
        {
          break;
        }
        if(blocksize == 1U)
        {
           Chip_Select_Hold = 0U;
        }

        /*SAFETYMCUSW 51 S MR:12.3 <APPROVED> "Needs shifting for 32-bit value" */
        spi->DAT1 = ((uint32)DataFormat << 24U) |
                    ((uint32)ChipSelect << 16U) |
                    (WDelay)            |
                    (Chip_Select_Hold)  |
                    (0x00000000U);
        /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
        while((spi->FLG & 0x00000100U) != 0x00000100U)
        {
        } /* Wait */
        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
        *destbuff = (uint16)spi->BUF;
        /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
        destbuff++;
        blocksize--;
    }

/* USER CODE BEGIN (7) */
/* USER CODE END */

    return (spi->FLG & 0xFFU);
}


/** @fn uint32 spiGetData(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint32 blocksize, uint16 * destbuff)
*   @brief Receives Data using interrupt method
*   @param[in] spi           - Spi module base address
*   @param[in] dataconfig_t    - Spi DAT1 register configuration
*   @param[in] blocksize    - number of data
*   @param[in] destbuff        - Pointer to the destination data (16 bit).
*
*   @return flag register value.
*
*   This function transmits blocksize number of data from source buffer using interrupt method.
*/
/* SourceId : SPI_SourceId_004 */
/* DesignId : SPI_DesignId_008 */
/* Requirements : HL_CONQ_SPI_SR15 */
void spiGetData(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint32 blocksize, uint16 * destbuff)
{
     uint32 index = (spi == spiREG1) ? 0U :((spi==spiREG2) ? 1U : ((spi==spiREG3) ? 2U:((spi==spiREG4) ? 3U:4U)));

/* USER CODE BEGIN (8) */
/* USER CODE END */

     g_spiPacket_t[index].rx_length = blocksize;
     g_spiPacket_t[index].rxdata_ptr   = destbuff;
     g_spiPacket_t[index].g_spiDataFormat = *dataconfig_t;
     g_spiPacket_t[index].rx_data_status = SPI_PENDING;

     spi->INT0 |= 0x0100U;

/* USER CODE BEGIN (9) */
/* USER CODE END */
}


/** @fn uint32 spiTransmitData(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint32 blocksize, uint16 * srcbuff)
*   @brief Transmits Data using polling method
*   @param[in] spi           - Spi module base address
*   @param[in] dataconfig_t    - Spi DAT1 register configuration
*   @param[in] blocksize    - number of data
*   @param[in] srcbuff        - Pointer to the source data ( 16 bit).
*
*   @return flag register value.
*
*   This function transmits blocksize number of data from source buffer using polling method.
*/
/* SourceId : SPI_SourceId_005 */
/* DesignId : SPI_DesignId_005 */
/* Requirements : HL_CONQ_SPI_SR12 */
uint32 spiTransmitData(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint32 blocksize, uint16 * srcbuff)
{
    volatile uint32 SpiBuf;
    uint16 Tx_Data;
    uint32 Chip_Select_Hold = (dataconfig_t->CS_HOLD) ? 0x10000000U : 0U;
    uint32 WDelay = (dataconfig_t->WDEL) ? 0x04000000U : 0U;
    SPIDATAFMT_t DataFormat = dataconfig_t->DFSEL;
    uint8 ChipSelect = dataconfig_t->CSNR;

/* USER CODE BEGIN (10) */
/* USER CODE END */
    while(blocksize != 0U)
    {
        if((spi->FLG & 0x000000FFU) !=0U)
        {
           break;
        }

        if(blocksize == 1U)
        {
           Chip_Select_Hold = 0U;
        }
        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
        Tx_Data = *srcbuff;

        spi->DAT1 =  ((uint32)DataFormat << 24U) |
                     ((uint32)ChipSelect << 16U) |
                     (WDelay)           |
                     (Chip_Select_Hold) |
                     (uint32)Tx_Data;
        /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
        srcbuff++;
        /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
        while((spi->FLG & 0x00000100U) != 0x00000100U)
        {
        } /* Wait */
        SpiBuf = spi->BUF;

        blocksize--;
    }

/* USER CODE BEGIN (11) */
/* USER CODE END */

    return (spi->FLG & 0xFFU);
}


/** @fn void spiSendData(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint32 blocksize, uint16 * srcbuff)
*   @brief Transmits Data using interrupt method
*   @param[in] spi           - Spi module base address
*   @param[in] dataconfig_t    - Spi DAT1 register configuration
*   @param[in] blocksize    - number of data
*   @param[in] srcbuff        - Pointer to the source data ( 16 bit).
*
*   @return flag register value.
*
*   This function transmits blocksize number of data from source buffer using interrupt method.
*/
/* SourceId : SPI_SourceId_006 */
/* DesignId : SPI_DesignId_006 */
/* Requirements : HL_CONQ_SPI_SR13 */
void spiSendData(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint32 blocksize, uint16 * srcbuff)
{
        uint32 index = (spi == spiREG1) ? 0U :((spi==spiREG2) ? 1U : ((spi==spiREG3) ? 2U:((spi==spiREG4) ? 3U:4U)));

/* USER CODE BEGIN (12) */
/* USER CODE END */

     g_spiPacket_t[index].tx_length = blocksize;
     g_spiPacket_t[index].txdata_ptr   = srcbuff;
     g_spiPacket_t[index].g_spiDataFormat = *dataconfig_t;
     g_spiPacket_t[index].tx_data_status = SPI_PENDING;

     spi->INT0 |= 0x0200U;

/* USER CODE BEGIN (13) */
/* USER CODE END */
}


/** @fn uint32 spiTransmitAndReceiveData(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint32 blocksize, uint16 * srcbuff, uint16 * destbuff)
*   @brief Transmits and Receive Data using polling method
*   @param[in] spi           - Spi module base address
*   @param[in] dataconfig_t    - Spi DAT1 register configuration
*   @param[in] blocksize    - number of data
*   @param[in] srcbuff        - Pointer to the source data ( 16 bit).
*   @param[in] destbuff        - Pointer to the destination data ( 16 bit).
*
*   @return flag register value.
*
*   This function transmits and receives blocksize number of data from source buffer using polling method.
*/
/* SourceId : SPI_SourceId_007 */
/* DesignId : SPI_DesignId_009 */
/* Requirements : HL_CONQ_SPI_SR16 */
uint32 spiTransmitAndReceiveData(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint32 blocksize, uint16 * srcbuff, uint16 * destbuff)
{
    uint16 Tx_Data;
    uint32 Chip_Select_Hold = (dataconfig_t->CS_HOLD) ? 0x10000000U : 0U;
    uint32 WDelay = (dataconfig_t->WDEL) ? 0x04000000U : 0U;
    SPIDATAFMT_t DataFormat = dataconfig_t->DFSEL;
    uint8 ChipSelect = dataconfig_t->CSNR;

/* USER CODE BEGIN (14) */
/* USER CODE END */
    while(blocksize != 0U)
    {
        if((spi->FLG & 0x000000FFU) != 0U)
        {
           break;
        }

        if(blocksize == 1U)
        {
           Chip_Select_Hold = 0U;
        }
        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
        Tx_Data = *srcbuff;

        spi->DAT1 =((uint32)DataFormat << 24U) |
                   ((uint32)ChipSelect << 16U) |
                   (WDelay)           |
                   (Chip_Select_Hold) |
                   (uint32)Tx_Data;
        /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
        srcbuff++;
        /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
        while((spi->FLG & 0x00000100U) != 0x00000100U)
        {
        } /* Wait */
        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
        *destbuff = (uint16)spi->BUF;
        /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
        destbuff++;

        blocksize--;
    }

/* USER CODE BEGIN (15) */
/* USER CODE END */

    return (spi->FLG & 0xFFU);
}

/* USER CODE BEGIN (16) */
/* USER CODE END */

/** @fn void spiSendAndGetData(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint32 blocksize, uint16 * srcbuff, uint16 * destbuff)
*   @brief Initiate SPI Transmits and receive Data using Interrupt mode.
*   @param[in] spi           - Spi module base address
*   @param[in] dataconfig_t    - Spi DAT1 register configuration
*   @param[in] blocksize    - number of data
*   @param[in] srcbuff        - Pointer to the source data ( 16 bit).
*   @param[in] destbuff        - Pointer to the destination data ( 16 bit).
*
*   Initiate SPI Transmits and receive Data using Interrupt mode..
*/
/* SourceId : SPI_SourceId_008 */
/* DesignId : SPI_DesignId_010 */
/* Requirements : HL_CONQ_SPI_SR17 */
void spiSendAndGetData(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint32 blocksize, uint16 * srcbuff, uint16 * destbuff)
{

/* USER CODE BEGIN (17) */
/* USER CODE END */

    uint32 index = (spi == spiREG1) ? 0U :((spi==spiREG2) ? 1U : ((spi==spiREG3) ? 2U:((spi==spiREG4) ? 3U:4U)));

    g_spiPacket_t[index].tx_length       = blocksize;
    g_spiPacket_t[index].rx_length       = blocksize;
    g_spiPacket_t[index].txdata_ptr      = srcbuff;
    g_spiPacket_t[index].rxdata_ptr      = destbuff;
    g_spiPacket_t[index].g_spiDataFormat = *dataconfig_t;
    g_spiPacket_t[index].tx_data_status  = SPI_PENDING;
    g_spiPacket_t[index].rx_data_status  = SPI_PENDING;

    spi->INT0 |= 0x0300U;

/* USER CODE BEGIN (18) */
/* USER CODE END */
}

/** @fn SpiDataStatus_t SpiTxStatus(spiBASE_t *spi)
*   @brief Get the status of the SPI Transmit data block.
*   @param[in] spi           - Spi module base address
*
*   @return Spi Transmit block data status.
*
*   Get the status of the SPI Transmit data block.
*/
/* SourceId : SPI_SourceId_009 */
/* DesignId : SPI_DesignId_013 */
/* Requirements : HL_CONQ_SPI_SR20 */
SpiDataStatus_t SpiTxStatus(spiBASE_t *spi)
{

/* USER CODE BEGIN (19) */
/* USER CODE END */

    uint32 index = (spi == spiREG1) ? 0U :((spi==spiREG2) ? 1U : ((spi==spiREG3) ? 2U:((spi==spiREG4) ? 3U:4U)));
    return(g_spiPacket_t[index].tx_data_status);
}

/* USER CODE BEGIN (20) */
/* USER CODE END */

/** @fn SpiDataStatus_t SpiRxStatus(spiBASE_t *spi)
*   @brief Get the status of the SPI Receive data block.
*   @param[in] spi           - Spi module base address
*
*   @return Spi Receive block data status.
*
*   Get the status of the SPI Receive data block.
*/
/* SourceId : SPI_SourceId_010 */
/* DesignId : SPI_DesignId_014 */
/* Requirements : HL_CONQ_SPI_SR21 */
SpiDataStatus_t SpiRxStatus(spiBASE_t *spi)
{

/* USER CODE BEGIN (21) */
/* USER CODE END */

    uint32 index = (spi == spiREG1) ? 0U :((spi==spiREG2) ? 1U : ((spi==spiREG3) ? 2U:((spi==spiREG4) ? 3U:4U)));
    return(g_spiPacket_t[index].rx_data_status);
}

/* USER CODE BEGIN (22) */
/* USER CODE END */

/** @fn void spiEnableLoopback(spiBASE_t *spi, loopBackType_t Loopbacktype)
*   @brief Enable Loopback mode for self test
*   @param[in] spi        - spi module base address
*   @param[in] Loopbacktype  - Digital or Analog
*
*   This function enables the Loopback mode for self test.
*/
/* SourceId : SPI_SourceId_011 */
/* DesignId : SPI_DesignId_011 */
/* Requirements : HL_CONQ_SPI_SR18 */
void spiEnableLoopback(spiBASE_t *spi, loopBackType_t Loopbacktype)
{
/* USER CODE BEGIN (23) */
/* USER CODE END */

    /* Clear Loopback incase enabled already */
    spi->IOLPKTSTCR = 0U;

    /* Enable Loopback either in Analog or Digital Mode */
    spi->IOLPKTSTCR = (uint32)0x00000A00U
                    | (uint32)((uint32)Loopbacktype << 1U);

/* USER CODE BEGIN (24) */
/* USER CODE END */
}

/* USER CODE BEGIN (25) */
/* USER CODE END */

/** @fn void spiDisableLoopback(spiBASE_t *spi)
*   @brief Enable Loopback mode for self test
*   @param[in] spi        - spi module base address
*
*   This function disable the Loopback mode.
*/
/* SourceId : SPI_SourceId_012 */
/* DesignId : SPI_DesignId_012 */
/* Requirements : HL_CONQ_SPI_SR19 */
void spiDisableLoopback(spiBASE_t *spi)
{
/* USER CODE BEGIN (26) */
/* USER CODE END */

    /* Disable Loopback Mode */
    spi->IOLPKTSTCR = 0x00000500U;

/* USER CODE BEGIN (27) */
/* USER CODE END */
}

/* USER CODE BEGIN (28) */
/* USER CODE END */

/** @fn spiEnableNotification(spiBASE_t *spi, uint32 flags)
*   @brief Enable interrupts
*   @param[in] spi   - spi module base address
*   @param[in] flags - Interrupts to be enabled, can be ored value of:
*/
/* SourceId : SPI_SourceId_013 */
/* DesignId : SPI_DesignId_003 */
/* Requirements : HL_CONQ_SPI_SR10 */
void spiEnableNotification(spiBASE_t *spi, uint32 flags)
{
/* USER CODE BEGIN (29) */
/* USER CODE END */

    spi->INT0 = (spi->INT0 & 0xFFFF0000U) | flags;

/* USER CODE BEGIN (30) */
/* USER CODE END */
}

/* USER CODE BEGIN (31) */
/* USER CODE END */

/** @fn spiDisableNotification(spiBASE_t *spi, uint32 flags)
*   @brief Enable interrupts
*   @param[in] spi   - spi module base address
*   @param[in] flags - Interrupts to be enabled, can be ored value of:
*/
/* SourceId : SPI_SourceId_014 */
/* DesignId : SPI_DesignId_004 */
/* Requirements : HL_CONQ_SPI_SR11 */
void spiDisableNotification(spiBASE_t *spi, uint32 flags)
{
/* USER CODE BEGIN (32) */
/* USER CODE END */

    spi->INT0 = (spi->INT0 & (~(flags)));

/* USER CODE BEGIN (33) */
/* USER CODE END */
}

/** @fn void spi1GetConfigValue(spi_config_reg_t *config_reg, config_value_type_t type)
*   @brief Get the initial or current values of the configuration registers
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
/* SourceId : SPI_SourceId_015 */
/* DesignId : SPI_DesignId_015 */
/* Requirements : HL_CONQ_SPI_SR24 */
void spi1GetConfigValue(spi_config_reg_t *config_reg, config_value_type_t type)
{
    if (type == InitialValue)
    {
        config_reg->CONFIG_GCR1  = SPI1_GCR1_CONFIGVALUE;
        config_reg->CONFIG_INT0  = SPI1_INT0_CONFIGVALUE;
        config_reg->CONFIG_LVL   = SPI1_LVL_CONFIGVALUE;
        config_reg->CONFIG_PC0   = SPI1_PC0_CONFIGVALUE;
        config_reg->CONFIG_PC1   = SPI1_PC1_CONFIGVALUE;
        config_reg->CONFIG_PC6   = SPI1_PC6_CONFIGVALUE;
        config_reg->CONFIG_PC7   = SPI1_PC7_CONFIGVALUE;
        config_reg->CONFIG_PC8   = SPI1_PC8_CONFIGVALUE;
        config_reg->CONFIG_DELAY = SPI1_DELAY_CONFIGVALUE;
        config_reg->CONFIG_FMT0  = SPI1_FMT0_CONFIGVALUE;
        config_reg->CONFIG_FMT1  = SPI1_FMT1_CONFIGVALUE;
        config_reg->CONFIG_FMT2  = SPI1_FMT2_CONFIGVALUE;
        config_reg->CONFIG_FMT3  = SPI1_FMT3_CONFIGVALUE;
    }
    else
    {
    /*SAFETYMCUSW 134 S MR:12.2 <APPROVED> "LDRA Tool issue" */
        config_reg->CONFIG_GCR1  = spiREG1->GCR1;
        config_reg->CONFIG_INT0  = spiREG1->INT0;
        config_reg->CONFIG_LVL   = spiREG1->LVL;
        config_reg->CONFIG_PC0   = spiREG1->PC0;
        config_reg->CONFIG_PC1   = spiREG1->PC1;
        config_reg->CONFIG_PC6   = spiREG1->PC6;
        config_reg->CONFIG_PC7   = spiREG1->PC7;
        config_reg->CONFIG_PC8   = spiREG1->PC8;
        config_reg->CONFIG_DELAY = spiREG1->DELAY ;
        config_reg->CONFIG_FMT0  = spiREG1->FMT0;
        config_reg->CONFIG_FMT1  = spiREG1->FMT1;
        config_reg->CONFIG_FMT2  = spiREG1->FMT2;
        config_reg->CONFIG_FMT3  = spiREG1->FMT3;
    }
}

/** @fn void spi2GetConfigValue(spi_config_reg_t *config_reg, config_value_type_t type)
*   @brief Get the initial or current values of the configuration registers
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
/* SourceId : SPI_SourceId_016 */
/* DesignId : SPI_DesignId_015 */
/* Requirements : HL_CONQ_SPI_SR25 */
void spi2GetConfigValue(spi_config_reg_t *config_reg, config_value_type_t type)
{
    if (type == InitialValue)
    {
        config_reg->CONFIG_GCR1  = SPI2_GCR1_CONFIGVALUE;
        config_reg->CONFIG_INT0  = SPI2_INT0_CONFIGVALUE;
        config_reg->CONFIG_LVL   = SPI2_LVL_CONFIGVALUE;
        config_reg->CONFIG_PC0   = SPI2_PC0_CONFIGVALUE;
        config_reg->CONFIG_PC1   = SPI2_PC1_CONFIGVALUE;
        config_reg->CONFIG_PC6   = SPI2_PC6_CONFIGVALUE;
        config_reg->CONFIG_PC7   = SPI2_PC7_CONFIGVALUE;
        config_reg->CONFIG_PC8   = SPI2_PC8_CONFIGVALUE;
        config_reg->CONFIG_DELAY = SPI2_DELAY_CONFIGVALUE;
        config_reg->CONFIG_FMT0  = SPI2_FMT0_CONFIGVALUE;
        config_reg->CONFIG_FMT1  = SPI2_FMT1_CONFIGVALUE;
        config_reg->CONFIG_FMT2  = SPI2_FMT2_CONFIGVALUE;
        config_reg->CONFIG_FMT3  = SPI2_FMT3_CONFIGVALUE;
    }
    else
    {
    /*SAFETYMCUSW 134 S MR:12.2 <APPROVED> "LDRA Tool issue" */
        config_reg->CONFIG_GCR1  = spiREG2->GCR1;
        config_reg->CONFIG_INT0  = spiREG2->INT0;
        config_reg->CONFIG_LVL   = spiREG2->LVL;
        config_reg->CONFIG_PC0   = spiREG2->PC0;
        config_reg->CONFIG_PC1   = spiREG2->PC1;
        config_reg->CONFIG_PC6   = spiREG2->PC6;
        config_reg->CONFIG_PC7   = spiREG2->PC7;
        config_reg->CONFIG_PC8   = spiREG2->PC8;
        config_reg->CONFIG_DELAY = spiREG2->DELAY ;
        config_reg->CONFIG_FMT0  = spiREG2->FMT0;
        config_reg->CONFIG_FMT1  = spiREG2->FMT1;
        config_reg->CONFIG_FMT2  = spiREG2->FMT2;
        config_reg->CONFIG_FMT3  = spiREG2->FMT3;
    }
}

/** @fn void spi3GetConfigValue(spi_config_reg_t *config_reg, config_value_type_t type)
*   @brief Get the initial or current values of the configuration registers
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
/* SourceId : SPI_SourceId_017 */
/* DesignId : SPI_DesignId_015 */
/* Requirements : HL_CONQ_SPI_SR26 */
void spi3GetConfigValue(spi_config_reg_t *config_reg, config_value_type_t type)
{
    if (type == InitialValue)
    {
        config_reg->CONFIG_GCR1  = SPI3_GCR1_CONFIGVALUE;
        config_reg->CONFIG_INT0  = SPI3_INT0_CONFIGVALUE;
        config_reg->CONFIG_LVL   = SPI3_LVL_CONFIGVALUE;
        config_reg->CONFIG_PC0   = SPI3_PC0_CONFIGVALUE;
        config_reg->CONFIG_PC1   = SPI3_PC1_CONFIGVALUE;
        config_reg->CONFIG_PC6   = SPI3_PC6_CONFIGVALUE;
        config_reg->CONFIG_PC7   = SPI3_PC7_CONFIGVALUE;
        config_reg->CONFIG_PC8   = SPI3_PC8_CONFIGVALUE;
        config_reg->CONFIG_DELAY = SPI3_DELAY_CONFIGVALUE;
        config_reg->CONFIG_FMT0  = SPI3_FMT0_CONFIGVALUE;
        config_reg->CONFIG_FMT1  = SPI3_FMT1_CONFIGVALUE;
        config_reg->CONFIG_FMT2  = SPI3_FMT2_CONFIGVALUE;
        config_reg->CONFIG_FMT3  = SPI3_FMT3_CONFIGVALUE;
    }
    else
    {
    /*SAFETYMCUSW 134 S MR:12.2 <APPROVED> "LDRA Tool issue" */
        config_reg->CONFIG_GCR1  = spiREG3->GCR1;
        config_reg->CONFIG_INT0  = spiREG3->INT0;
        config_reg->CONFIG_LVL   = spiREG3->LVL;
        config_reg->CONFIG_PC0   = spiREG3->PC0;
        config_reg->CONFIG_PC1   = spiREG3->PC1;
        config_reg->CONFIG_PC6   = spiREG3->PC6;
        config_reg->CONFIG_PC7   = spiREG3->PC7;
        config_reg->CONFIG_PC8   = spiREG3->PC8;
        config_reg->CONFIG_DELAY = spiREG3->DELAY ;
        config_reg->CONFIG_FMT0  = spiREG3->FMT0;
        config_reg->CONFIG_FMT1  = spiREG3->FMT1;
        config_reg->CONFIG_FMT2  = spiREG3->FMT2;
        config_reg->CONFIG_FMT3  = spiREG3->FMT3;
    }
}

/** @fn void spi4GetConfigValue(spi_config_reg_t *config_reg, config_value_type_t type)
*   @brief Get the initial or current values of the configuration registers
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
/* SourceId : SPI_SourceId_018 */
/* DesignId : SPI_DesignId_015 */
/* Requirements : HL_CONQ_SPI_SR27 */
void spi4GetConfigValue(spi_config_reg_t *config_reg, config_value_type_t type)
{
    if (type == InitialValue)
    {
        config_reg->CONFIG_GCR1  = SPI4_GCR1_CONFIGVALUE;
        config_reg->CONFIG_INT0  = SPI4_INT0_CONFIGVALUE;
        config_reg->CONFIG_LVL   = SPI4_LVL_CONFIGVALUE;
        config_reg->CONFIG_PC0   = SPI4_PC0_CONFIGVALUE;
        config_reg->CONFIG_PC1   = SPI4_PC1_CONFIGVALUE;
        config_reg->CONFIG_PC6   = SPI4_PC6_CONFIGVALUE;
        config_reg->CONFIG_PC7   = SPI4_PC7_CONFIGVALUE;
        config_reg->CONFIG_PC8   = SPI4_PC8_CONFIGVALUE;
        config_reg->CONFIG_DELAY = SPI4_DELAY_CONFIGVALUE;
        config_reg->CONFIG_FMT0  = SPI4_FMT0_CONFIGVALUE;
        config_reg->CONFIG_FMT1  = SPI4_FMT1_CONFIGVALUE;
        config_reg->CONFIG_FMT2  = SPI4_FMT2_CONFIGVALUE;
        config_reg->CONFIG_FMT3  = SPI4_FMT3_CONFIGVALUE;
    }
    else
    {
    /*SAFETYMCUSW 134 S MR:12.2 <APPROVED> "LDRA Tool issue" */
        config_reg->CONFIG_GCR1  = spiREG4->GCR1;
        config_reg->CONFIG_INT0  = spiREG4->INT0;
        config_reg->CONFIG_LVL   = spiREG4->LVL;
        config_reg->CONFIG_PC0   = spiREG4->PC0;
        config_reg->CONFIG_PC1   = spiREG4->PC1;
        config_reg->CONFIG_PC6   = spiREG4->PC6;
        config_reg->CONFIG_PC7   = spiREG4->PC7;
        config_reg->CONFIG_PC8   = spiREG4->PC8;
        config_reg->CONFIG_DELAY = spiREG4->DELAY ;
        config_reg->CONFIG_FMT0  = spiREG4->FMT0;
        config_reg->CONFIG_FMT1  = spiREG4->FMT1;
        config_reg->CONFIG_FMT2  = spiREG4->FMT2;
        config_reg->CONFIG_FMT3  = spiREG4->FMT3;
    }
}

/** @fn void spi5GetConfigValue(spi_config_reg_t *config_reg, config_value_type_t type)
*   @brief Get the initial or current values of the configuration registers
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
/* SourceId : SPI_SourceId_019 */
/* DesignId : SPI_DesignId_015 */
/* Requirements : HL_CONQ_SPI_SR28 */
void spi5GetConfigValue(spi_config_reg_t *config_reg, config_value_type_t type)
{
    if (type == InitialValue)
    {
        config_reg->CONFIG_GCR1  = SPI5_GCR1_CONFIGVALUE;
        config_reg->CONFIG_INT0  = SPI5_INT0_CONFIGVALUE;
        config_reg->CONFIG_LVL   = SPI5_LVL_CONFIGVALUE;
        config_reg->CONFIG_PC0   = SPI5_PC0_CONFIGVALUE;
        config_reg->CONFIG_PC1   = SPI5_PC1_CONFIGVALUE;
        config_reg->CONFIG_PC6   = SPI5_PC6_CONFIGVALUE;
        config_reg->CONFIG_PC7   = SPI5_PC7_CONFIGVALUE;
        config_reg->CONFIG_PC8   = SPI5_PC8_CONFIGVALUE;
        config_reg->CONFIG_DELAY = SPI5_DELAY_CONFIGVALUE;
        config_reg->CONFIG_FMT0  = SPI5_FMT0_CONFIGVALUE;
        config_reg->CONFIG_FMT1  = SPI5_FMT1_CONFIGVALUE;
        config_reg->CONFIG_FMT2  = SPI5_FMT2_CONFIGVALUE;
        config_reg->CONFIG_FMT3  = SPI5_FMT3_CONFIGVALUE;
    }
    else
    {
    /*SAFETYMCUSW 134 S MR:12.2 <APPROVED> "LDRA Tool issue" */
        config_reg->CONFIG_GCR1  = spiREG5->GCR1;
        config_reg->CONFIG_INT0  = spiREG5->INT0;
        config_reg->CONFIG_LVL   = spiREG5->LVL;
        config_reg->CONFIG_PC0   = spiREG5->PC0;
        config_reg->CONFIG_PC1   = spiREG5->PC1;
        config_reg->CONFIG_PC6   = spiREG5->PC6;
        config_reg->CONFIG_PC7   = spiREG5->PC7;
        config_reg->CONFIG_PC8   = spiREG5->PC8;
        config_reg->CONFIG_DELAY = spiREG5->DELAY ;
        config_reg->CONFIG_FMT0  = spiREG5->FMT0;
        config_reg->CONFIG_FMT1  = spiREG5->FMT1;
        config_reg->CONFIG_FMT2  = spiREG5->FMT2;
        config_reg->CONFIG_FMT3  = spiREG5->FMT3;
    }
}



/* USER CODE BEGIN (38) */
/* USER CODE END */

/** @fn void mibspi1HighLevelInterrupt(void)
*   @brief Level 0 Interrupt for SPI1
*/
#pragma CODE_STATE(mibspi1HighLevelInterrupt, 32)
#pragma INTERRUPT(mibspi1HighLevelInterrupt, IRQ)
/* SourceId : SPI_SourceId_021 */
/* DesignId : SPI_DesignId_016 */
/* Requirements : HL_CONQ_SPI_SR22, HL_CONQ_SPI_SR23, HL_CONQ_SPI_SR29  */
void mibspi1HighLevelInterrupt(void)
{

/* USER CODE BEGIN (39) */
/* USER CODE END */

    uint32 flags = (spiREG1->FLG & 0x0000FFFFU) & (~spiREG1->LVL & 0x035FU);
    uint32 vec = spiREG1->INTVECT0;

/* USER CODE BEGIN (40) */
/* USER CODE END */

    switch(vec)
    {

    case 0x24U: /* Receive Buffer Full Interrupt */
             {
                uint16 *destbuff;
                destbuff = g_spiPacket_t[0U].rxdata_ptr;

                *destbuff = (uint16)spiREG1->BUF;
                /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
                g_spiPacket_t[0U].rxdata_ptr++;
                g_spiPacket_t[0U].rx_length--;

                if(g_spiPacket_t[0U].rx_length == 0U)
                {
                    spiREG1->INT0 = (spiREG1->INT0 & 0x0000FFFFU) & (~(uint32)0x0100U);
                    g_spiPacket_t[0U].rx_data_status = SPI_COMPLETED;
                    spiEndNotification(spiREG1);
                }
                break;
             }

    case 0x28U: /* Transmit Buffer Empty Interrupt */
             {
                 volatile uint32 SpiBuf;
                 uint32 Chip_Select_Hold = 0U;
                 uint32 WDelay = (g_spiPacket_t[0U].g_spiDataFormat.WDEL) ? 0x04000000U : 0U;
                 SPIDATAFMT_t DataFormat = g_spiPacket_t[0U].g_spiDataFormat.DFSEL;
                 uint8 ChipSelect = g_spiPacket_t[0U].g_spiDataFormat.CSNR;
                 uint16 Tx_Data = *g_spiPacket_t[0U].txdata_ptr;

                 g_spiPacket_t[0U].tx_length--;

                 if(g_spiPacket_t[0U].tx_length == 0U)
                 {
                    Chip_Select_Hold = 0U;
                 }
                 else
                 {
                    Chip_Select_Hold = (g_spiPacket_t[0U].g_spiDataFormat.CS_HOLD) ? 0x10000000U : 0U;
                 }

                 spiREG1->DAT1 = ((uint32)DataFormat << 24U) |
                                 ((uint32)ChipSelect << 16U) |
                                 (WDelay)           |
                                 (Chip_Select_Hold) |
                                 (uint32)Tx_Data;

                 /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
                 g_spiPacket_t[0U].txdata_ptr++;
                 /* Dummy Receive read if no RX Interrupt enabled */
                 if(((spiREG1->INT0 & 0x0000FFFFU)& 0x0100U) == 0U)
                 {
                     if((spiREG1->FLG & 0x00000100U) == 0x00000100U)
                     {
                         SpiBuf = spiREG1->BUF;
                     }
                 }

                 if(g_spiPacket_t[0U].tx_length == 0U)
                 {
                    spiREG1->INT0 = (spiREG1->INT0 & 0x0000FFFFU) & (~(uint32)0x0200U); /* Disable Interrupt */
                    g_spiPacket_t[0U].tx_data_status = SPI_COMPLETED;
                    spiEndNotification(spiREG1);
                }
                break;
             }

    default: /* Clear Flags and return  */
             spiREG1->FLG = flags;
             spiNotification(spiREG1, flags & 0xFFU);
             break;
    }

/* USER CODE BEGIN (41) */
/* USER CODE END */
}


/* USER CODE BEGIN (46) */
/* USER CODE END */

/** @fn void spi2HighLevelInterrupt(void)
*   @brief Level 0 Interrupt for SPI2
*/
#pragma CODE_STATE(spi2HighLevelInterrupt, 32)
#pragma INTERRUPT(spi2HighLevelInterrupt, IRQ)
/* SourceId : SPI_SourceId_023 */
/* DesignId : SPI_DesignId_016 */
/* Requirements : HL_CONQ_SPI_SR22, HL_CONQ_SPI_SR23, HL_CONQ_SPI_SR31  */
void spi2HighLevelInterrupt(void)
{

/* USER CODE BEGIN (47) */
/* USER CODE END */

    uint32 flags = (spiREG2->FLG & 0x0000FFFFU) & (~spiREG2->LVL & 0x035FU);
    uint32 vec = spiREG2->INTVECT0;

/* USER CODE BEGIN (48) */
/* USER CODE END */

    switch(vec)
    {

    case 0x24U: /* Receive Buffer Full Interrupt */
             {
                uint16 *destbuff;
                destbuff = g_spiPacket_t[1U].rxdata_ptr;

                *destbuff = (uint16)spiREG2->BUF;
                /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
                g_spiPacket_t[1U].rxdata_ptr++;
                g_spiPacket_t[1U].rx_length--;

                if(g_spiPacket_t[1U].rx_length == 0U)
                {
                    spiREG2->INT0 = (spiREG2->INT0 & 0x0000FFFFU) & (~(uint32)0x0100U);
                    g_spiPacket_t[1U].rx_data_status = SPI_COMPLETED;
                    spiEndNotification(spiREG2);
                }
                break;
             }

    case 0x28U: /* Transmit Buffer Empty Interrupt */
             {
                 volatile uint32 SpiBuf;
                 uint32 Chip_Select_Hold = 0U;
                 uint32 WDelay = (g_spiPacket_t[1U].g_spiDataFormat.WDEL) ? 0x04000000U : 0U;

                 SPIDATAFMT_t DataFormat = g_spiPacket_t[1U].g_spiDataFormat.DFSEL;
                 uint8 ChipSelect = g_spiPacket_t[1U].g_spiDataFormat.CSNR;
                 uint16 Tx_Data = *g_spiPacket_t[1U].txdata_ptr;

                 g_spiPacket_t[1U].tx_length--;

                 if(g_spiPacket_t[1U].tx_length == 0U)
                 {
                    Chip_Select_Hold = 0U;
                 }
                 else
                 {
                    Chip_Select_Hold = (g_spiPacket_t[1U].g_spiDataFormat.CS_HOLD) ? 0x10000000U : 0U;
                 }

                 spiREG2->DAT1 = ((uint32)DataFormat << 24U) |
                                 ((uint32)ChipSelect << 16U) |
                                 (WDelay)           |
                                 (Chip_Select_Hold) |
                                 (uint32)Tx_Data;

                 /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
                 g_spiPacket_t[1U].txdata_ptr++;
                 /* Dummy Receive read if no RX Interrupt enabled */
                 if(((spiREG2->INT0 & 0x0000FFFFU)& 0x0100U) == 0U)
                 {
                     if((spiREG2->FLG & 0x00000100U) == 0x00000100U)
                     {
                         SpiBuf = spiREG2->BUF;
                     }
                 }

                 if(g_spiPacket_t[1U].tx_length == 0U)
                 {
                    spiREG2->INT0 = (spiREG2->INT0 & 0x0000FFFFU) & (~(uint32)0x0200U); /* Disable Interrupt */
                    g_spiPacket_t[1U].tx_data_status = SPI_COMPLETED;
                    spiEndNotification(spiREG2);
                 }
                break;
             }

    default: /* Clear Flags and return  */
             spiREG2->FLG = flags;
             spiNotification(spiREG2, flags & 0xFFU);
             break;
    }

/* USER CODE BEGIN (49) */
/* USER CODE END */
}


/* USER CODE BEGIN (54) */
/* USER CODE END */

/** @fn void mibspi3HighInterruptLevel(void)
*   @brief Level 0 Interrupt for SPI3
*/
#pragma CODE_STATE(mibspi3HighInterruptLevel, 32)
#pragma INTERRUPT(mibspi3HighInterruptLevel, IRQ)

/* SourceId : SPI_SourceId_025 */
/* DesignId : SPI_DesignId_016 */
/* Requirements : HL_CONQ_SPI_SR22, HL_CONQ_SPI_SR23, HL_CONQ_SPI_SR33  */
void mibspi3HighInterruptLevel(void)
{

/* USER CODE BEGIN (55) */
/* USER CODE END */

    uint32 flags = (spiREG3->FLG & 0x0000FFFFU) & (~spiREG3->LVL & 0x035FU);
    uint32 vec = spiREG3->INTVECT0;

/* USER CODE BEGIN (56) */
/* USER CODE END */

    switch(vec)
    {

    case 0x24U: /* Receive Buffer Full Interrupt */
             {
                uint16 *destbuff;
                destbuff = g_spiPacket_t[2U].rxdata_ptr;

                *destbuff = (uint16)spiREG3->BUF;
                /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
                g_spiPacket_t[2U].rxdata_ptr++;
                g_spiPacket_t[2U].rx_length--;

                if(g_spiPacket_t[2U].rx_length == 0U)
                {
                    spiREG3->INT0 = (spiREG3->INT0 & 0x0000FFFFU) & (~(uint32)0x0100U);
                    g_spiPacket_t[2U].rx_data_status = SPI_COMPLETED;
                    spiEndNotification(spiREG3);
                }
                break;
             }

    case 0x28U: /* Transmit Buffer Empty Interrupt */
             {
                 volatile uint32 SpiBuf;
                 uint32 Chip_Select_Hold = 0U;
                 uint32 WDelay = (g_spiPacket_t[2U].g_spiDataFormat.WDEL) ? 0x04000000U: 0U;
                 SPIDATAFMT_t DataFormat = g_spiPacket_t[2U].g_spiDataFormat.DFSEL;
                 uint8 ChipSelect = g_spiPacket_t[2U].g_spiDataFormat.CSNR;
                 uint16 Tx_Data = *g_spiPacket_t[2U].txdata_ptr;

                 g_spiPacket_t[2U].tx_length--;

                 if(g_spiPacket_t[2U].tx_length == 0U)
                 {
                    Chip_Select_Hold = 0U;
                 }
                 else
                 {
                    Chip_Select_Hold = (g_spiPacket_t[2U].g_spiDataFormat.CS_HOLD) ? 0x10000000U : 0U;
                 }

                 spiREG3->DAT1 = ((uint32)DataFormat << 24U) |
                                 ((uint32)ChipSelect << 16U) |
                                 (WDelay)           |
                                 (Chip_Select_Hold) |
                                 (uint32)Tx_Data;

                 /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
                 g_spiPacket_t[2U].txdata_ptr++;
                 /* Dummy Receive read if no RX Interrupt enabled */
                 if(((spiREG3->INT0 & 0x0000FFFFU)& 0x0100U) == 0U)
                 {
                     if((spiREG3->FLG & 0x00000100U) == 0x00000100U)
                     {
                         SpiBuf = spiREG3->BUF;
                     }
                 }

                 if(g_spiPacket_t[2U].tx_length == 0U)
                 {
                    spiREG3->INT0 = (spiREG3->INT0 & 0x0000FFFFU) & (~(uint32)0x0200U); /* Disable Interrupt */
                    g_spiPacket_t[2U].tx_data_status = SPI_COMPLETED;
                    spiEndNotification(spiREG3);
                }
                break;
             }

    default: /* Clear Flags and return  */
             spiREG3->FLG = flags;
             spiNotification(spiREG3, flags & 0xFFU);
             break;
    }

/* USER CODE BEGIN (57) */
/* USER CODE END */
}


/* USER CODE BEGIN (62) */
/* USER CODE END */

/** @fn void mibspi4HighLevelInterrupt(void)
*   @brief Level 0 Interrupt for SPI4
*/
#pragma CODE_STATE(mibspi4HighLevelInterrupt, 32)
#pragma INTERRUPT(mibspi4HighLevelInterrupt, IRQ)

/* SourceId : SPI_SourceId_027 */
/* DesignId : SPI_DesignId_016 */
/* Requirements : HL_CONQ_SPI_SR22, HL_CONQ_SPI_SR23, HL_CONQ_SPI_SR35  */
void mibspi4HighLevelInterrupt(void)
{

/* USER CODE BEGIN (63) */
/* USER CODE END */

    uint32 flags = (spiREG4->FLG & 0x0000FFFFU) & (~spiREG4->LVL & 0x035FU);
    uint32 vec = spiREG4->INTVECT0;

/* USER CODE BEGIN (64) */
/* USER CODE END */

    switch(vec)
    {

    case 0x24U: /* Receive Buffer Full Interrupt */
             {
               uint16 *destbuff;
                destbuff = g_spiPacket_t[3U].rxdata_ptr;

                *destbuff = (uint16)spiREG4->BUF;
                /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
                g_spiPacket_t[3U].rxdata_ptr++;
                g_spiPacket_t[3U].rx_length--;

                if(g_spiPacket_t[3U].rx_length == 0U)
                {
                    spiREG4->INT0 = (spiREG4->INT0 & 0x0000FFFFU) & (~(uint32)0x0100U);
                    g_spiPacket_t[3U].rx_data_status = SPI_COMPLETED;
                    spiEndNotification(spiREG4);
                }
                break;
             }

    case 0x28U: /* Transmit Buffer Empty Interrupt */
             {
                 volatile uint32 SpiBuf;
                 uint32 Chip_Select_Hold = 0U;
                 uint32 WDelay = (g_spiPacket_t[3U].g_spiDataFormat.WDEL) ? 0x04000000U: 0U;
                 SPIDATAFMT_t DataFormat = g_spiPacket_t[3U].g_spiDataFormat.DFSEL;
                 uint8 ChipSelect = g_spiPacket_t[3U].g_spiDataFormat.CSNR;
                 uint16 Tx_Data = *g_spiPacket_t[3U].txdata_ptr;

                 g_spiPacket_t[3U].tx_length--;

                 if(g_spiPacket_t[3U].tx_length == 0U)
                 {
                    Chip_Select_Hold = 0U;
                 }
                 else
                 {
                    Chip_Select_Hold = (g_spiPacket_t[3U].g_spiDataFormat.CS_HOLD) ? 0x10000000U : 0U;
                 }

                 spiREG4->DAT1 = ((uint32)DataFormat << 24U) |
                                 ((uint32)ChipSelect << 16U) |
                                 (WDelay)           |
                                 (Chip_Select_Hold) |
                                 (uint32)Tx_Data;

                 /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
                 g_spiPacket_t[3U].txdata_ptr++;
                 /* Dummy Receive read if no RX Interrupt enabled */
                 if(((spiREG4->INT0 & 0x0000FFFFU)& 0x0100U) == 0U)
                 {
                     if((spiREG4->FLG & 0x00000100U) == 0x00000100U)
                     {
                         SpiBuf = spiREG4->BUF;
                     }
                 }

                 if(g_spiPacket_t[3U].tx_length == 0U)
                 {
                    spiREG4->INT0 = (spiREG4->INT0 & 0x0000FFFFU) & (~(uint32)0x0200U); /* Disable Interrupt */
                    g_spiPacket_t[3U].tx_data_status = SPI_COMPLETED;
                    spiEndNotification(spiREG4);
                 }
                break;
             }

    default: /* Clear Flags and return  */
             spiREG4->FLG = flags;
             spiNotification(spiREG4, flags & 0xFFU);
             break;
    }

/* USER CODE BEGIN (65) */
/* USER CODE END */
}


/* USER CODE BEGIN (70) */
/* USER CODE END */

/** @fn void mibspi5HighLevelInterrupt(void)
*   @brief Level 0 Interrupt for SPI5
*/
#pragma CODE_STATE(mibspi5HighLevelInterrupt, 32)
#pragma INTERRUPT(mibspi5HighLevelInterrupt, IRQ)

/* SourceId : SPI_SourceId_029 */
/* DesignId : SPI_DesignId_016 */
/* Requirements : HL_CONQ_SPI_SR22, HL_CONQ_SPI_SR23, HL_CONQ_SPI_SR37  */
void mibspi5HighLevelInterrupt(void)
{

/* USER CODE BEGIN (71) */
/* USER CODE END */

    uint32 flags = (spiREG5->FLG & 0x0000FFFFU) & (~spiREG5->LVL & 0x035FU);
    uint32 vec = spiREG5->INTVECT0;


/* USER CODE BEGIN (72) */
/* USER CODE END */

    switch(vec)
    {

    case 0x24U: /* Receive Buffer Full Interrupt */
             {
                uint16 *destbuff;
                destbuff = g_spiPacket_t[4U].rxdata_ptr;

                *destbuff = (uint16)spiREG5->BUF;
                /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
                g_spiPacket_t[4U].rxdata_ptr++;
                g_spiPacket_t[4U].rx_length--;

                if(g_spiPacket_t[4U].rx_length == 0U)
                {
                    spiREG5->INT0 = (spiREG5->INT0 & 0x0000FFFFU) & (~(uint32)0x0100U);
                    g_spiPacket_t[4U].rx_data_status = SPI_COMPLETED;
                    spiEndNotification(spiREG5);
                }
                break;
             }

    case 0x28U: /* Transmit Buffer Empty Interrupt */
             {
                 volatile uint32 SpiBuf;
                 uint32 Chip_Select_Hold = 0U;
                 uint32 WDelay = (g_spiPacket_t[4U].g_spiDataFormat.WDEL) ? 0x04000000U: 0U;
                 SPIDATAFMT_t DataFormat = g_spiPacket_t[4U].g_spiDataFormat.DFSEL;
                 uint8 ChipSelect = g_spiPacket_t[4U].g_spiDataFormat.CSNR;
                 uint16 Tx_Data = *g_spiPacket_t[4U].txdata_ptr;

                 g_spiPacket_t[4U].tx_length--;

                 if(g_spiPacket_t[4U].tx_length == 0U)
                 {
                    Chip_Select_Hold = 0U;
                 }
                 else
                 {
                    Chip_Select_Hold = (g_spiPacket_t[4U].g_spiDataFormat.CS_HOLD) ? 0x10000000U : 0U;
                 }

                 spiREG5->DAT1 = ((uint32)DataFormat << 24U) |
                                 ((uint32)ChipSelect << 16U) |
                                 (WDelay)           |
                                 (Chip_Select_Hold) |
                                 (uint32)Tx_Data;

                 /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
                 g_spiPacket_t[4U].txdata_ptr++;
                 /* Dummy Receive read if no RX Interrupt enabled */
                 if(((spiREG5->INT0 & 0x0000FFFFU)& 0x0100U) == 0U)
                 {
                     if((spiREG5->FLG & 0x00000100U) == 0x00000100U)
                     {
                         SpiBuf = spiREG5->BUF;
                     }
                 }

                 if(g_spiPacket_t[4U].tx_length == 0U)
                 {
                    spiREG5->INT0 = (spiREG5->INT0 & 0x0000FFFFU) & (~(uint32)0x0200U); /* Disable Interrupt */
                    g_spiPacket_t[4U].tx_data_status = SPI_COMPLETED;
                    spiEndNotification(spiREG5);
                }
                break;
             }

    default: /* Clear Flags and return  */
             spiREG5->FLG = flags;
             spiNotification(spiREG5, flags & 0xFFU);
             break;
    }

/* USER CODE BEGIN (73) */
/* USER CODE END */
}

/*----------------------------------------------------------------------------\
|   Private Function Implementations                                          |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   End of fw_spi.c module                                                    |
\----------------------------------------------------------------------------*/

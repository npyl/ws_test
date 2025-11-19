/*----------------------------------------------------------------------------\
|          Copyright(c) 2024 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : fw_adc.c Module File.                                      |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 9 …·Ì 2024                                                 |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   <Module Description>                                                      |
|                                                                             |
\----------------------------------------------------------------------------*/

/* Channel assignments of the microcontroller ADC cores
 *
 *                                               ________________________
 *              AD1EXT_SEL[4:0] <---------------|                        |
 *                   AD1EXT_ENA <---------------|                        |
 *                       AD1EVT <-------------->|                        |
 *                   AD1…N[7:0] --------------->|                        |
 *                ______________                |                        |
 *               |              |               |                        |
 *               | AD1IN[15:8]  |               |                        |
 *               |              |-------.------>|                        |
 *               | AD2IN[15:8]  |       |       |                        |
 *               |______________|       |       |                        |
 *                                      |       |         ADC1           |
 *                ______________        |       |                        |
 *               |              |       |       |      AD1IN[31:0]       |
 *               | AD1IN[23:16] |       |       |                        |
 *               |              |---.---------->|                        |
 *               | AD2IN[7:0]   |   |   |       |                        |
 *               |______________|   |   |       |                        |
 *                                  |   |       |                        |
 *                 AD1IN[31:24] --------------->|                        |
 *                                  |   |       |________________________|
 *                                  |   |
 *                                  |   |        ________________________
 *                                  |    ------>|                        |
 *                                  |           |                        |
 *                                   ---------->|                        |
 *                                              |         ADC2           |
 *                       AD2EVT <-------------->|                        |
 *                 AD2IN[24:16] --------------->|      AD2IN[24:0]       |
 *                                              |                        |
 *                                              |________________________|
 *
 * Notes:
 *   The TMS570LC4357 has two ADC cores: ADC1 and ADC2
 *   ADC1 supports 32 channels
 *   ADC2 supports 25 channels of which 16 channels are shared with ADC1
 *   When using both ADC1 and ADC2 on a shared channel, the sample windows must be identical
 *   The reference voltages, as well as operating supply and ground, are shared between the two ADC cores
 *
 * Warnings:
 *   Selecting continuous conversion mode for all three groups:
 *     All three conversion groups cannot be configured to be in a continuous conversion mode. If
 *     the application configures the group mode control registers to enable continuous conversion
 *     mode for all three groups, then Group2 will be automatically be configured to be in a
 *     single conversion sequence mode.
 *
 * Design Decisions:
 *   Will use non-shared channels of each ADC core:
 *     ADC1 core non-shared channels: AD1IN[ 7:0 ] and AD1IN[ 31:24 ]
 *     ADC2 core non-shared channels: AD2IN[ 24:16 ]
 *   Will trigger an initial start of conversion in the ADC initialization function.  Subsequent start of conversion
 *     triggers will be done using a periodic OS task
 *   Will use End Of Conversion interrupt for all ADC groups and update the Data Manager with the converged ADC values
 *   Will not use magnitude threshold detection
 *   Will not use ADC EVT output pin
 *   Implemented custom ADC initialization function in order to read application configuration rather than use HALCoGen
 *   Implemented custom ADC start conversion function to support the ADC initialization function
 *   Used HALCoGen only to create the End Of Conversion interrupt handlers as they must be named as expected by the HAL
 *
 */

/*----------------------------------------------------------------------------\
|   Compiler Controls                                                         |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

#include "HL_adc.h"

#include "data_manager.h"
#include "fw_adc.h"

/*----------------------------------------------------------------------------\
|   Private Type Definitions                                                  |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Private Constant Definitions                                              |
\----------------------------------------------------------------------------*/

/* ADC cores configuration */
static const S_ADC_UNIT_CONFIG adc_unit_config_defs[] =
{
    {
        .id = eADC_UNIT_1,                          /* Leave as is */
        .ram_parity = FALSE,
        .ps = 7U,
        .group_config =
        {
            {
                .group = eADC_EVENT_GROUP,          /* Leave as is */
                .resolution = eADC_12_BIT,
                .ch_id_in_conv_rslts  = TRUE,
                .continuous_conv = FALSE,
                .sampling_cap_discharge = FALSE,
                .conv_end_interrupt_en = TRUE,      /* Leave as is, otherwise must poll for end of conversion */
            },
            {
                .group = eADC_GROUP_1,              /* Leave as is */
                .resolution = eADC_12_BIT,
                .ch_id_in_conv_rslts = TRUE,
                .continuous_conv = FALSE,
                .sampling_cap_discharge = FALSE,
                .conv_end_interrupt_en = TRUE,      /* Leave as is, otherwise must poll for end of conversion */
            },
            {
                .group = eADC_GROUP_2,              /* Leave as is */
                .resolution = eADC_12_BIT,
                .ch_id_in_conv_rslts = TRUE,
                .continuous_conv = FALSE,
                .sampling_cap_discharge = FALSE,
                .conv_end_interrupt_en = TRUE,      /* Leave as is, otherwise must poll for end of conversion */
            },
        },
        .enabled = TRUE,
    },
    {
        .id = eADC_UNIT_2,                          /* Leave as is */
        .ram_parity = FALSE,
        .ps = 7U,
        .group_config =
        {
            {
                .group = eADC_EVENT_GROUP,          /* Leave as is */
                .resolution = eADC_12_BIT,
                .ch_id_in_conv_rslts = TRUE,
                .continuous_conv = FALSE,
                .sampling_cap_discharge = FALSE,
                .conv_end_interrupt_en = TRUE,      /* Leave as is, otherwise must poll for end of conversion */
            },
            {
                .group = eADC_GROUP_1,              /* Leave as is */
                .resolution = eADC_12_BIT,
                .ch_id_in_conv_rslts = TRUE,
                .continuous_conv = FALSE,
                .sampling_cap_discharge = FALSE,
                .conv_end_interrupt_en = TRUE,      /* Leave as is, otherwise must poll for end of conversion */
            },
            {
                .group = eADC_GROUP_2,              /* Leave as is */
                .resolution = eADC_12_BIT,
                .ch_id_in_conv_rslts = TRUE,
                .continuous_conv = FALSE,
                .sampling_cap_discharge = FALSE,
                .conv_end_interrupt_en = TRUE,      /* Leave as is, otherwise must poll for end of conversion */
            },
        },
        .enabled = FALSE,
    },
};

/* ADC channels configuration */
static const S_ADC_CHANNEL_CONFIG adc_channel_config_defs[] =
{
    {
        .label = "HDK LDR",
        .id = eADC1_9,
        .unit = eADC_UNIT_1,
        .group = eADC_GROUP_1,
        .ch = 9u,
        .enabled = TRUE,
    },
};

/* s_adc_FiFoSize is used as constant table for channel selection */
static const uint32 s_adc_FiFoSize[ eADC_UNIT_MAX ][ eADC_GROUP_MAX ] =
{
    {
        eADC_FIFO_16,                               /* ADC1 - EVENT GROUP */
        eADC_FIFO_16,                               /* ADC1 - GROUP1 */
        eADC_FIFO_16,                               /* ADC1 - GROUP2 */
    },
    {
        eADC_FIFO_16,                               /* ADC2 - EVENT GROUP */
        eADC_FIFO_16,                               /* ADC2 - GROUP1 */
        eADC_FIFO_16,                               /* ADC2 - GROUP1 */
    },
};

/* s_adc_Select is used as constant table for channel selection */
static uint32 s_adc_Select[ eADC_UNIT_MAX ][ eADC_GROUP_MAX ] =
{
     {
        0U,                                         /* ADC1 - EVENT GROUP */
        0U,                                         /* ADC1 - GROUP1 */
        0U,                                         /* ADC1 - GROUP2 */
     },
     {
        0U,                                         /* ADC2 - EVENT GROUP */
        0U,                                         /* ADC2 - GROUP1 */
        0U,                                         /* ADC2 - GROUP1 */
     },
};

/*----------------------------------------------------------------------------\
|   Private Data Declarations                                                 |
\----------------------------------------------------------------------------*/

static int adc_raw[ eADC_CHANNEL_MAX ];

/*----------------------------------------------------------------------------\
|   Public Constant Declarations                                              |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Private Function Declarations                                             |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Public Function Implementations                                           |
\----------------------------------------------------------------------------*/

void adcNotification(adcBASE_t *adc, uint32 group)
{
    adcData_t data;
    volatile uint32 count;

    count = adcGetData( adc, group, &data );
}

void adc_start_conversion( adcBASE_t *adc, uint32 group )
{
    uint32 index = ( adc == adcREG1) ? 0U : 1U;

    /* Setup FiFo size */
    adc->GxINTCR[ group ] = s_adc_FiFoSize[ index ][ group ];

    /* Start ADC group conversion */
    adc->GxSEL[ group ] = s_adc_Select[ index ][ group ];
}

void adc_read_channels( void )
{
    int i;
    const S_ADC_UNIT_CONFIG * const p_adc_unit = adcGetUnitConfig();
    const S_ADC_CHANNEL_CONFIG * const p_adc_ch = adcGetChannelConfig();
    S_DM_ANALOGUE_INPUTS *ptr_dm_analogue_inputs = ( S_DM_ANALOGUE_INPUTS * ) dmAnalogueInputsAccess();

    for ( i = 0; i < eADC_CHANNEL_MAX; i++ )
    {
        if ( p_adc_ch[ i ].enabled == TRUE )
        {
            /* TODO Read ADC channel */
            ptr_dm_analogue_inputs->ptr_analogue_inputs->adc_raw[ i ] = adc_raw[ p_adc_ch[ i ].ch ];
        }
    }
}

void adc_init( void )
{
    int i, j;
    adcBASE_t * p_adc[ eADC_UNIT_MAX ] = { adcREG1, adcREG2 };
    S_DM_DATABASE *ptr_dm_database = ( S_DM_DATABASE * ) dmFullDataAccess();
    const S_ADC_UNIT_CONFIG * const p_adc_unit = adcGetUnitConfig();
    const S_ADC_CHANNEL_CONFIG * const p_adc_ch = adcGetChannelConfig();

    /* Setup ADC channels array */
    for ( i = 0; i < eADC_CHANNEL_MAX; i++ )
    {
        if ( p_adc_ch[ i ].enabled == TRUE )                            /* Check ADC enabled */
        {
            s_adc_Select[ p_adc_ch[ i ].unit ][ p_adc_ch[ i ].group ] |= ( uint32 ) ( ( uint32 ) 1U << p_adc_ch[ i ].ch );
        }
    }

    /* Setup ADC units */
    for ( i = 0; i < eADC_UNIT_MAX; i++ )
    {
        if ( p_adc_unit[ i ].enabled == TRUE )
        {
            /* Reset ADC core */
            p_adc[ i ]->RSTCR = 1U;
            p_adc[ i ]->RSTCR = 0U;

            /* Enable 12-BIT ADC */
            p_adc[ i ]->OPMODECR |= 0x80000000U;

            /* Setup prescaler */
            p_adc[ i ]->CLOCKCR = p_adc_unit[ i ].ps;

            /* Setup memory boundaries */
            p_adc[ i ]->BNDCR = ( uint32 ) ( ( uint32 ) 8U << 16U ) | ( 8U + 8U );
            p_adc[ i ]->BNDEND = ( p_adc[ i ]->BNDEND & 0xFFFF0000U ) | ( 2U );

            for ( j = 0; j < eADC_GROUP_MAX; j++ )
            {
                /* Setup data format */
                p_adc[ i ]->GxMODECR[ j ] |= ( uint32 ) p_adc_unit[ i ].group_config[ j ].resolution;

                /* Enable/Disable channel id in conversion result */
                if ( p_adc_unit[ i ].group_config[ j ].ch_id_in_conv_rslts == TRUE )
                {

                    p_adc[ i ]->GxMODECR[ j ] |= ( uint32 ) 0x00000020U;
                }

                /* Enable/Disable continuous conversion */
                if ( p_adc_unit[ i ].group_config[ j ].continuous_conv == TRUE )
                {
                    p_adc[ i ]->GxMODECR[ j ] |= ( uint32 ) 0x00000002U;
                }

                /* Setup event group hardware trigger:
                 *   Setup hardware trigger edge
                 *   Setup hardware trigger source
                 * Setup event group sample window
                 * Setup event group sample discharge:
                 *   Setup discharge prescaler
                 *   Enable/Disable discharge
                 */
                switch ( p_adc_unit[ i ].group_config[ j ].group )
                {
                    case eADC_EVENT_GROUP:
                        p_adc[ i ]->EVSRC = ( uint32 ) 0x00000000U | ( uint32 ) ADC1_EVENT;
                        p_adc[ i ]->EVSAMP = 1U;
                        p_adc[ i ]->EVSAMPDISEN = ( uint32 ) ( ( uint32 ) 0U << 8U );
                        ( p_adc_unit[ i ].group_config[ j ].sampling_cap_discharge == TRUE ) ?
                                ( p_adc[ i ]->EVSAMPDISEN |= ( uint32 ) 0x00000001U ) :
                                ( p_adc[ i ]->EVSAMPDISEN |= ( uint32 ) 0x00000000U );
                        break;
                    case eADC_GROUP_1:
                        p_adc[ i ]->G1SRC = ( uint32 ) 0x00000000U | ( uint32 ) ADC1_EVENT;
                        p_adc[ i ]->G1SAMP = 1U;
                        p_adc[ i ]->G1SAMPDISEN = ( uint32 ) ( ( uint32 ) 0U << 8U );
                        ( p_adc_unit[ i ].group_config[ j ].sampling_cap_discharge == TRUE ) ?
                                ( p_adc[ i ]->G1SAMPDISEN |= ( uint32 ) 0x00000001U ) :
                                ( p_adc[ i ]->G1SAMPDISEN |= ( uint32 ) 0x00000000U );
                        break;
                    case eADC_GROUP_2:
                        p_adc[ i ]->G2SRC = ( uint32 ) 0x00000000U | ( uint32 ) ADC1_EVENT;
                        p_adc[ i ]->G2SAMP = 1U;
                        p_adc[ i ]->G2SAMPDISEN = ( uint32 ) ( ( uint32 ) 0U << 8U );
                        ( p_adc_unit[ i ].group_config[ j ].sampling_cap_discharge == TRUE ) ?
                                ( p_adc[ i ]->G2SAMPDISEN |= ( uint32 ) 0x00000001U ) :
                                ( p_adc[ i ]->G2SAMPDISEN |= ( uint32 ) 0x00000000U );
                        break;
                    default:
                        break;
                }

                /* Enable End Of Conversion notification interrupt for ADC group */
                if ( p_adc_unit[ i ].group_config[ j ].conv_end_interrupt_en == TRUE )
                {
                    adcEnableNotification( p_adc[ i ], j );
                }
            }

            /* ADC EVT pin output value */
            p_adc[ i ]->EVTOUT = 0U;

            /* ADC EVT pin direction */
            p_adc[ i ]->EVTDIR = 0U;

            /* ADC EVT pin open drain */
            p_adc[ i ]->EVTPDR = 0U;

            /* ADC EVT pin pullup / pulldown selection */
            p_adc[ i ]->EVTPSEL = 0U;

            /* ADC EVT pin pullup / pulldown enable */
            p_adc[ i ]->EVTDIS = 0U;

            /* Enable ADC module */
            p_adc[ i ]->OPMODECR |= 0x80140001U;

            /* Wait for buffer initialization complete */
            while ( ( ( p_adc[ i ]->BNDEND & 0xFFFF0000 ) >> 16U ) != 0U )
            {
                ;
            }

            /* Setup parity */
            ( p_adc_unit[ i ].ram_parity == TRUE ) ? ( p_adc[ i ]->PARCR = 0x0000000AU ) : ( p_adc[ i ]->PARCR = 0x00000005U );

            for ( j = 0; j < eADC_GROUP_MAX; j++ )
            {
                /* Start ADC group channels conversion */
                adc_start_conversion( p_adc[ i ], j );
            }

        }
    }
}

const S_ADC_UNIT_CONFIG * const adcGetUnitConfig( void )
{
    /* Return the address of the ADC channel configuration data */
    return adc_unit_config_defs;
}

const S_ADC_CHANNEL_CONFIG * const adcGetChannelConfig( void )
{
    /* Return the address of the ADC channel configuration data */
    return adc_channel_config_defs;
}

/*----------------------------------------------------------------------------\
|   Private Function Implementations                                          |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   End of fw_adc.c module                                                    |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|          Copyright(c) 2024 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : fw_adc.h Header File.                                      |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 9 …·Ì 2024                                                 |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   <Module Header Description>                                               |
|                                                                             |
\----------------------------------------------------------------------------*/

#ifndef fw_adc_H
#define fw_adc_H

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

#include "HL_adc.h"

#include "fw_types.h"

/*----------------------------------------------------------------------------\
|   Public Type Definitions                                                   |
\----------------------------------------------------------------------------*/

typedef enum
{
    eADC_UNIT_1 = 0,
    eADC_UNIT_2,
    eADC_UNIT_MAX,
} E_ADC_UNIT;

typedef enum
{
    eADC1_9 = 0,
    eADC_CHANNEL_MAX,
} E_ADC_ID;

typedef enum
{
    eADC_12_BIT = 0x00000000U, /**< Alias for 12 bit data resolution */
    eADC_10_BIT = 0x00000100U, /**< Alias for 10 bit data resolution */
    eADC_8_BIT  = 0x00000200U  /**< Alias for 8 bit data resolution  */
} E_ADC_RESOLUTION;

typedef enum
{
    eADC_FIFO_8 = 8,
    eADC_FIFO_16 = 16,
    eADC_FIFO_32 = 32,
    eADC_FIFO_64 = 64,
    eADC_FIFO_MAX = 4,
} E_ADC_FIFO_SIZE;

typedef enum
{
    eADC_EVENT_GROUP = 0,
    eADC_GROUP_1,
    eADC_GROUP_2,
    eADC_GROUP_MAX,
} E_ADC_GROUP;

typedef struct
{
    E_ADC_GROUP                 group;
    E_ADC_RESOLUTION            resolution;
    BOOLEAN                     ch_id_in_conv_rslts;
    BOOLEAN                     continuous_conv;
    BOOLEAN                     sampling_cap_discharge;
    BOOLEAN                     conv_end_interrupt_en;
} E_ADC_GROUP_CONFIG;

typedef struct
{
    E_ADC_UNIT                  id;
    BOOLEAN                     ram_parity;
    U32                         ps;
    E_ADC_GROUP_CONFIG          group_config[ eADC_GROUP_MAX ];
    BOOLEAN                     enabled;
} S_ADC_UNIT_CONFIG;

typedef struct
{
    CHAR                        label[ 20u ];
    E_ADC_ID                    id;
    E_ADC_UNIT                  unit;
    E_ADC_GROUP                 group;
    U8                          ch;
    BOOLEAN                     enabled;
} S_ADC_CHANNEL_CONFIG;

/*----------------------------------------------------------------------------\
|   Public Constant Declarations                                              |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Public Function Declarations                                              |
\----------------------------------------------------------------------------*/

void adc_init( void );
void adc_start_conversion( adcBASE_t *adc, uint32 group );
void adc_read_channels( void );
const S_ADC_CHANNEL_CONFIG * const adcGetChannelConfig( void );
const S_ADC_UNIT_CONFIG * const adcGetUnitConfig( void );

/*----------------------------------------------------------------------------\
|   End of fw_adc.h header file                                               |
\----------------------------------------------------------------------------*/

#endif  /* fw_adc_H */

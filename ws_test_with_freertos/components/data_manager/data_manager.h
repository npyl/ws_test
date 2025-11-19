/*----------------------------------------------------------------------------\
|          Copyright(c) 2024 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : data_manager.h Header File.                                |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 8 …·Ì 2024                                                 |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   <Module Header Description>                                               |
|                                                                             |
\----------------------------------------------------------------------------*/

#ifndef data_manager_H
#define data_manager_H

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

#include "fw_adc.h"
#include "fw_dio.h"

/*----------------------------------------------------------------------------\
|   Public Type Definitions                                                   |
\----------------------------------------------------------------------------*/

/*
 * Dataset structures for the database manager
 */
typedef struct
{
    S_DIO_INPUT_PIN_DEF     digital_inputs[ eDIO_NUM_INPUT_PINS ];
    S_DIO_OUTPUT_PIN_DEF    digital_outputs[ eDIO_NUM_OUTPUT_PINS ];
} S_DIGITAL_IO;

typedef struct
{
    int                     adc_raw[ eADC_CHANNEL_MAX ];
} S_ANALOGUE_INPUTS;

/*
 * Data Manager superset definition
 *   Include all structures for which we want data management
 */
typedef struct
{
    S_DIGITAL_IO            digital_io;
    S_ANALOGUE_INPUTS       analogue_inputs;
} S_DM_DATABASE;

/*
 * Transport packaging structures
 */
typedef struct
{
    S_DIGITAL_IO            *ptr_digital_io;
} S_DM_DIGITAL_IO;

typedef struct
{
    S_ANALOGUE_INPUTS       *ptr_analogue_inputs;
} S_DM_ANALOGUE_INPUTS;

/*----------------------------------------------------------------------------\
|   Public Function Declarations                                              |
\----------------------------------------------------------------------------*/

void dmDataManagerInit( void );
S_DM_DATABASE *dmFullDataAccess( void );
S_DM_DIGITAL_IO *dmDigitalsAccess( void );
S_DM_ANALOGUE_INPUTS *dmAnalogueInputsAccess( void );

/*----------------------------------------------------------------------------\
|   End of data_manager.h header file                                         |
\----------------------------------------------------------------------------*/

#endif  /* data_manager_H */

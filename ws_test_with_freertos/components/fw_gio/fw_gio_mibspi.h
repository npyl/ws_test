/*----------------------------------------------------------------------------\
|          Copyright(c) 2025 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : fw_gio_mibspi.h Header File.                               |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 23 Feb 2025                                                |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   MIBSPI pin configuration and handling module header                       |
|                                                                             |
\----------------------------------------------------------------------------*/

#ifndef fw_gio_mibspi_H
#define fw_gio_mibspi_H

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

#include "HL_hal_stdtypes.h"
#include "HL_reg_mibspi.h"

#include "fw_globals.h"

/*----------------------------------------------------------------------------\
|   Public Type Definitions                                                   |
\----------------------------------------------------------------------------*/

typedef enum
{
    eMIBSPI1 = 0u,
    eMIBSPI2,
    eMIBSPI3,
    eMIBSPI4,
    eMIBSPI5,
    eMIBSPI_MAX_MODULES,
} E_MIBSPI_MODULE;

typedef enum
{
    eMIBSPI_INPUT = 0u,
    eMIBSPI_OUTPUT,
} E_MIBSPI_DIRECTION;

/*
 * MIBSPI input pin IDs
 */
typedef enum
{
    /* Define input pins here when needed */
    eMIBSPI_NUM_INPUT_PINS,
} E_MIBSPI_INPUT_PIN_ID;

/*
 * MIBSPI output pin IDs
 */
typedef enum
{
    eOUT_PIN_W8_DemodEna,
    eOUT_PIN_V8_GainActive2,
    eOUT_PIN_W9_GainActive1,
    eMIBSPI_NUM_OUTPUT_PINS,
} E_MIBSPI_OUTPUT_PIN_ID;

/*
 * Single MIBSPI input pin definition structure
 */
typedef struct
{
    CHAR                  label[32u];      /* Label of MIBSPI pin */
    E_MIBSPI_INPUT_PIN_ID id;              /* ID of MIBSPI pin */
    E_MIBSPI_MODULE       module;          /* MIBSPI module number */
    mibspiBASE_t*         base;            /* Pointer to MIBSPI base registers */
    uint8                 pin;             /* Pin number within module */
    E_PULL_UP_DOWN        pull;            /* Pull-up/down configuration */
    BOOLEAN               pull_enable;     /* Pull-up/down enable */
    E_MIBSPI_DIRECTION    direction;       /* Pin direction */
    BOOLEAN               value;           /* Initial/current value */
    BOOLEAN               enabled;         /* Pin enabled flag */
} S_MIBSPI_INPUT_PIN_DEF;

/*
 * Single MIBSPI output pin definition structure
 */
typedef struct
{
    CHAR                   label[32u];      /* Label of MIBSPI pin */
    E_MIBSPI_OUTPUT_PIN_ID id;              /* ID of MIBSPI pin */
    E_MIBSPI_MODULE        module;          /* MIBSPI module number */
    mibspiBASE_t*          base;            /* Pointer to MIBSPI base registers */
    uint8                  pin;             /* Pin number within module */
    BOOLEAN                open_drain;      /* Open drain configuration */
    E_MIBSPI_DIRECTION     direction;       /* Pin direction */
    BOOLEAN                value;           /* Initial/current value */
    BOOLEAN                enabled;         /* Pin enabled flag */
} S_MIBSPI_OUTPUT_PIN_DEF;

/*----------------------------------------------------------------------------\
|   Public Function Declarations                                              |
\----------------------------------------------------------------------------*/

void mibspiHandlerInit( void );
BOOLEAN mibspiHandlerInitInputPins( void );
BOOLEAN mibspiHandlerInitOutputPins( void );
void set_mibspi_output( E_MIBSPI_OUTPUT_PIN_ID pin_id, BOOLEAN value );
BOOLEAN read_mibspi_input( E_MIBSPI_INPUT_PIN_ID pin_id );
void read_mibspi_inputs( void );
const S_MIBSPI_INPUT_PIN_DEF* const mibspiConfigGetDIConfig( void );
const S_MIBSPI_OUTPUT_PIN_DEF* const mibspiConfigGetDOConfig( void );

/*----------------------------------------------------------------------------\
|   End of fw_gio_mibspi.h header file                                        |
\----------------------------------------------------------------------------*/

#endif  /* fw_gio_mibspi_H */

/*----------------------------------------------------------------------------\
|          Copyright(c) 2025 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : fw_gio_het.h Header File.                                  |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 23 Feb 2025                                                |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   HET pin configuration and handling module header                          |
|                                                                             |
\----------------------------------------------------------------------------*/

#ifndef fw_gio_het_H
#define fw_gio_het_H

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

#include "HL_hal_stdtypes.h"
#include "HL_reg_het.h"
#include "HL_gio.h"

#include "fw_globals.h"
#include "fw_types.h"

/*----------------------------------------------------------------------------\
|   Public Type Definitions                                                   |
\----------------------------------------------------------------------------*/

typedef enum
{
    eHET1 = 0u,
    eHET2,
    eHET_MAX_MODULES,
} E_HET_MODULE;

typedef enum
{
    eHET_INPUT = 0u,
    eHET_OUTPUT,
} E_HET_DIRECTION;

/*
 * HET input pin IDs
 */
typedef enum
{
    eHET_NUM_INPUT_PINS,
} E_HET_INPUT_PIN_ID;

/*
 * Single HET input pin definition.
 * This structure defines a single HET input pin
 */
typedef struct
{
    CHAR                label[32u];     /* Label of pin */
    E_HET_INPUT_PIN_ID  id;             /* ID of pin */
    E_HET_MODULE        module;         /* HET module */
    gioPORT_t*          port;           /* Direct pointer to HET port registers */
    uint8               pin;            /* Pin number within module */
    E_PULL_UP_DOWN      pull;           /* Pull-up/down configuration */
    BOOLEAN             pull_enable;    /* Pull-up/down enable */
    E_HET_DIRECTION     direction;      /* Pin direction */
    BOOLEAN             value;          /* Initial/current value */
    BOOLEAN             enabled;        /* Pin enabled flag */
} S_HET_INPUT_PIN_DEF;

/*
 * HET output pin IDs
 */
typedef enum
{
    eOUT_PIN_A13_RS485REb = 0u,
    eOUT_PIN_J17_VPOS5019,
    eOUT_PIN_N1_PowSWDT,
    eOUT_PIN_V6_FLTBYPASSSW,
#if 1
    eOUT_LedLeftBottom,
#endif
    eHET_NUM_OUTPUT_PINS,
} E_HET_OUTPUT_PIN_ID;

/*
 * Single HET output pin definition.
 * This structure defines a single HET output pin
 */
typedef struct
{
    CHAR                label[32u];     /* Label of pin */
    E_HET_OUTPUT_PIN_ID id;             /* ID of pin */
    E_HET_MODULE        module;         /* HET module */
    gioPORT_t*          port;           /* Direct pointer to HET port registers */
    uint8               pin;            /* Pin number within module */
    BOOLEAN             open_drain;     /* Open drain configuration */
    E_HET_DIRECTION     direction;      /* Pin direction */
    BOOLEAN             value;          /* Initial/current value */
    BOOLEAN             enabled;        /* Pin enabled flag */
} S_HET_OUTPUT_PIN_DEF;

/*----------------------------------------------------------------------------\
|   Public Function Declarations                                              |
\----------------------------------------------------------------------------*/

void hetHandlerInit( void );
BOOLEAN hetHandlerInitInputPins( void );
BOOLEAN hetHandlerInitOutputPins( void );
void set_het_output( E_HET_OUTPUT_PIN_ID pin_id, BOOLEAN value );
BOOLEAN read_het_input( E_HET_INPUT_PIN_ID pin_id );
void read_het_inputs( void );
const S_HET_INPUT_PIN_DEF* const hetConfigGetDIConfig( void );
const S_HET_OUTPUT_PIN_DEF* const hetConfigGetDOConfig( void );

/*----------------------------------------------------------------------------\
|   End of fw_gio_het.h header file                                           |
\----------------------------------------------------------------------------*/

#endif  /* fw_gio_het_H */

/*----------------------------------------------------------------------------\
|          Copyright(c) 2025 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : fw_gio_dmm.h Header File.                                  |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 25 Feb 2025                                                |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   DMM pin configuration and handling module header                          |
|                                                                             |
\----------------------------------------------------------------------------*/

#ifndef fw_gio_dmm_H
#define fw_gio_dmm_H

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

#include "HL_dmm.h"
#include "HL_hal_stdtypes.h"
#include "fw_types.h"
#include "fw_globals.h"

/*----------------------------------------------------------------------------\
|   Public Type Definitions                                                   |
\----------------------------------------------------------------------------*/

typedef enum
{
    eDMM_INPUT = 0u,
    eDMM_OUTPUT,
} E_DMM_DIRECTION;

typedef enum
{
    eDMM_SYNC = 0u,
    eDMM_CLK,
    eDMM_DATA0,
    eDMM_DATA1,
    eDMM_DATA2,
    eDMM_DATA3,
    eDMM_DATA4,
    eDMM_DATA5,
    eDMM_DATA6,
    eDMM_DATA7,
    eDMM_DATA8,
    eDMM_DATA9,
    eDMM_DATA10,
    eDMM_DATA11,
    eDMM_DATA12,
    eDMM_DATA13,
    eDMM_DATA14,
    eDMM_DATA15,
    eDMM_ENA,
    eDMM_PIN_MAX
} E_DMM_PIN;

/*
 * DMM input pin IDs
 */
typedef enum
{
    eINP_PIN_H16_AISGDIR,
    eDMM_NUM_INPUT_PINS,
} E_DMM_INPUT_PIN_ID;

/*
 * DMM output pin IDs
 */
typedef enum
{
    eOUT_PIN_H17_RS485DE,
    eDMM_NUM_OUTPUT_PINS,
} E_DMM_OUTPUT_PIN_ID;

/*
 * Single DMM input pin definition structure
 */
typedef struct
{
    CHAR               label[32u];      /* Label of DMM pin */
    E_DMM_INPUT_PIN_ID id;              /* ID of DMM pin */
    gioPORT_t*         base;            /* Pointer to DMM base registers */
    E_DMM_PIN          pin;             /* Pin number within module */
    E_PULL_UP_DOWN     pull;            /* Pull-up/down configuration */
    BOOLEAN            pull_enable;     /* Pull-up/down enable */
    E_DMM_DIRECTION    direction;       /* Pin direction */
    BOOLEAN            value;           /* Initial/current value */
    BOOLEAN            enabled;         /* Pin enabled flag */
} S_DMM_INPUT_PIN_DEF;

/*
 * Single DMM output pin definition structure
 */
typedef struct
{
    CHAR                label[32u];      /* Label of DMM pin */
    E_DMM_OUTPUT_PIN_ID id;              /* ID of DMM pin */
    gioPORT_t*          base;            /* Pointer to DMM base registers */
    E_DMM_PIN           pin;             /* Pin number within module */
    BOOLEAN             open_drain;      /* Open drain configuration */
    E_DMM_DIRECTION     direction;       /* Pin direction */
    BOOLEAN             value;           /* Initial/current value */
    BOOLEAN             enabled;         /* Pin enabled flag */
} S_DMM_OUTPUT_PIN_DEF;

/*----------------------------------------------------------------------------\
|   Public Function Declarations                                              |
\----------------------------------------------------------------------------*/

void dmmHandlerInit( void );
BOOLEAN dmmHandlerInitInputPins( void );
BOOLEAN dmmHandlerInitOutputPins( void );
void set_dmm_output(E_DMM_OUTPUT_PIN_ID pin_id, BOOLEAN value );
BOOLEAN read_dmm_input(E_DMM_INPUT_PIN_ID pin_id );
void read_dmm_inputs(void);
const S_DMM_INPUT_PIN_DEF* const dmmConfigGetDIConfig( void );
const S_DMM_OUTPUT_PIN_DEF* const dmmConfigGetDOConfig( void );

/*----------------------------------------------------------------------------\
|   End of fw_gio_dmm.h header file                                           |
\----------------------------------------------------------------------------*/

#endif  /* fw_gio_dmm_H */

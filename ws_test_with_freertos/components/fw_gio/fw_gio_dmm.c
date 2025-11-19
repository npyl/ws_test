/*----------------------------------------------------------------------------\
|          Copyright(c) 2025 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : fw_gio_dmm.c Module File.                                  |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 25 Feb 2025                                                |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   DMM pin configuration and handling module                                 |
|                                                                             |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

#include "fw_types.h"
#include "svc.h"
#include "fw_globals.h"
#include "fw_gio_dmm.h"

/*----------------------------------------------------------------------------\
|   Private Type Definitions                                                  |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Private Constant Definitions                                              |
\----------------------------------------------------------------------------*/

static const S_DMM_INPUT_PIN_DEF dmm_input_pin_defs[ eDMM_NUM_INPUT_PINS ] =
{
    /* DMM Input Pins */
    {
        /* H16, AISG Data Dir Input, DATA14 */
        .label = "RAISG Data Dir Input",
        .id = eINP_PIN_H16_AISGDIR,
        .base = dmmPORT,
        .pin = eDMM_DATA14,
        .pull = ePULL_NONE,
        .pull_enable = FALSE,
        .direction = eDMM_INPUT,
        .value = FALSE,
        .enabled = TRUE,
    },
};

static const S_DMM_OUTPUT_PIN_DEF dmm_output_pin_defs[ eDMM_NUM_OUTPUT_PINS ] =
{
    /* DMM Output Pins */
    {
        /* H17, RS485 DE */
        .label = "RS485 DE",
        .id = eOUT_PIN_H17_RS485DE,
        .base = dmmPORT,
        .pin = eDMM_DATA10,
        .open_drain = FALSE,
        .direction = eDMM_OUTPUT,
        .value = FALSE,
        .enabled = TRUE,
    },
};

/*----------------------------------------------------------------------------\
|   Private Function Declarations                                             |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Public Function Implementations                                           |
\----------------------------------------------------------------------------*/

void dmmHandlerInit( void )
{
    int i;
    uint32 pinMask = 0;

    /* Create input pin mask */
    for ( i = 0; i < eDMM_NUM_INPUT_PINS; i++ )
    {
        if ( dmm_input_pin_defs[ i ].enabled )
        {
            pinMask |= ( 1U << dmm_input_pin_defs[ i ].pin );
        }
    }

    /* Create output pin mask */
    for ( i = 0; i < eDMM_NUM_OUTPUT_PINS; i++ )
    {
        if ( dmm_output_pin_defs[ i ].enabled )
        {
            pinMask |= ( 1U << dmm_output_pin_defs[ i ].pin );
        }
    }

    /* Set PC0 to 0 for all pins we're going to use as GPIO */
    dmmREG->PC0 &= ~pinMask;

    /* Initialize pin configurations using direct port access */
    dmmHandlerInitInputPins();
    dmmHandlerInitOutputPins();
}

BOOLEAN dmmHandlerInitInputPins( void )
{
    int i;
    BOOLEAN ok = TRUE;
    const S_DMM_INPUT_PIN_DEF* const ptr_cfg = dmmConfigGetDIConfig();

    for ( i = 0; i < eDMM_NUM_INPUT_PINS; i++ )
    {
        if ( ptr_cfg[ i ].enabled == TRUE )
        {
            /* Set pin direction to input */
            ptr_cfg[ i ].base->DIR &= ~( 1U << ptr_cfg[ i ].pin );

            /* Configure pull-up/down if needed */
            /* Note: This may need adjustment based on how pull configuration works in GIO */
        }
    }

    return ok;
}

BOOLEAN dmmHandlerInitOutputPins( void )
{
    int i;
    BOOLEAN ok = TRUE;
    const S_DMM_OUTPUT_PIN_DEF* const ptr_cfg = dmmConfigGetDOConfig();

    for ( i = 0; i < eDMM_NUM_OUTPUT_PINS; i++ )
    {
        if ( ptr_cfg[ i ].enabled == TRUE )
        {
            /* Set pin direction to output */
            ptr_cfg[ i ].base->DIR |= ( 1U << ptr_cfg[ i ].pin );

            /* Set initial output value */
            if ( ptr_cfg[ i ].value == TRUE )
            {
                ptr_cfg[ i ].base->DSET = ( 1U << ptr_cfg[ i ].pin );  /* Set high */
            }
            else
            {
                ptr_cfg[ i ].base->DCLR = ( 1U << ptr_cfg[ i ].pin );  /* Set low */
            }

            /* Note: Open drain configuration may need to be handled differently */
        }
    }

    return ok;
}

void set_dmm_output( E_DMM_OUTPUT_PIN_ID pin_id, BOOLEAN value )
{
    const S_DMM_OUTPUT_PIN_DEF* const ptr_cfg = dmmConfigGetDOConfig();
    register uint32_t u32Mode = 0x13ul;

    if ((ptr_cfg[pin_id].enabled == TRUE) &&
        (pin_id < eDMM_NUM_OUTPUT_PINS) &&
        (ptr_cfg[pin_id].direction == eDMM_OUTPUT))
    {
        u32Mode = switchCpuMode( u32Mode );
        ASSERT( 0x1Ful == u32Mode );

        if (value == TRUE)
        {
            ptr_cfg[pin_id].base->DSET = (1U << ptr_cfg[pin_id].pin);
        }
        else
        {
            ptr_cfg[pin_id].base->DCLR = (1U << ptr_cfg[pin_id].pin);
        }
        switchToUserMode();
    }
}

BOOLEAN read_dmm_input( E_DMM_INPUT_PIN_ID pin_id )
{
    BOOLEAN value = FALSE;
    const S_DMM_INPUT_PIN_DEF* const ptr_cfg = dmmConfigGetDIConfig();

    if ( ( ptr_cfg[ pin_id ].enabled == TRUE ) &&
         ( pin_id < eDMM_NUM_INPUT_PINS ) &&
         ( ptr_cfg[ pin_id ].direction == eDMM_INPUT ) )
    {
        /* Read input value using GIO data in register */
        value = ( ptr_cfg[ pin_id ].base->DIN & ( 1U << ptr_cfg[ pin_id ].pin ) ) ? TRUE : FALSE;
    }

    return value;
}

void read_dmm_inputs( void )
{
    int i;
    const S_DMM_INPUT_PIN_DEF* const ptr_cfg = dmmConfigGetDIConfig();

    for ( i = 0; i < eDMM_NUM_INPUT_PINS; i++ )
    {
        if ( ptr_cfg[ i ].enabled == TRUE )
        {
            read_dmm_input( ptr_cfg[ i ].id );
        }
    }
}

const S_DMM_INPUT_PIN_DEF* const dmmConfigGetDIConfig( void )
{
    return dmm_input_pin_defs;
}

const S_DMM_OUTPUT_PIN_DEF* const dmmConfigGetDOConfig( void )
{
    return dmm_output_pin_defs;
}

/*----------------------------------------------------------------------------\
|   Private Function Implementations                                          |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   End of fw_gio_dmm.c module                                                |
\----------------------------------------------------------------------------*/

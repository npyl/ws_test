/*----------------------------------------------------------------------------\
|          Copyright(c) 2025 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : fw_gio_het.c Module File.                                  |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 23 Feb 2025                                                |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   HET pin configuration and handling module                                 |
|                                                                             |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

#include "HL_het.h"

#include "fw_gio_het.h"

/*----------------------------------------------------------------------------\
|   Private Type Definitions                                                  |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Private Constant Definitions                                              |
\----------------------------------------------------------------------------*/

/* Pin configuration definitions */
static const S_HET_INPUT_PIN_DEF het_input_pin_defs[eHET_NUM_INPUT_PINS] =
{
    /* HETx Input Pins */
};

static const S_HET_OUTPUT_PIN_DEF het_output_pin_defs[eHET_NUM_OUTPUT_PINS] =
{
    /* HETx Output Pins */
    {
        /* A13, RS485REb */
        .label = "RS485REb",
        .id = eOUT_PIN_A13_RS485REb,
        .module = eHET1,
        .port = hetPORT1,
        .pin = PIN_HET_17,
        .open_drain = FALSE,
        .direction = eHET_OUTPUT,
        .value = FALSE,
        .enabled = TRUE,
    },
    {
        /* J17, Positive Supply ADRF5019 */
        .label = "ADRF5019",
        .id = eOUT_PIN_J17_VPOS5019,
        .module = eHET1,
        .port = hetPORT1,
        .pin = PIN_HET_31,
        .open_drain = FALSE,
        .direction = eHET_OUTPUT,
        .value = FALSE,
        .enabled = TRUE,
    },
    {
        /* N1, PowerSWDT */
        .label = "PowerSWDT",
        .id = eOUT_PIN_N1_PowSWDT,
        .module = eHET1,
        .port = hetPORT1,
        .pin = PIN_HET_15,
        .open_drain = FALSE,
        .direction = eHET_OUTPUT,
        .value = FALSE,
        .enabled = TRUE,
    },
    {
        /* V6, FaultBypassSW */
        .label = "FaultBypassSW",
        .id = eOUT_PIN_V6_FLTBYPASSSW,
        .module = eHET1,
        .port = hetPORT1,
        .pin = PIN_HET_5,
        .open_drain = FALSE,
        .direction = eHET_OUTPUT,
        .value = FALSE,
        .enabled = TRUE,
    },
#if 1
    {
        .label = "eOUT_LedLeftBottom",
        .id = eOUT_LedLeftBottom,
        .module = eHET1,
        .port = hetPORT1,
        .pin = PIN_HET_31,
        .open_drain = TRUE,
        .direction = eHET_OUTPUT,
        .value = FALSE,
        .enabled = TRUE,
    },
#endif
};

/*----------------------------------------------------------------------------\
|   Private Data Declarations                                                 |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Private Function Declarations                                             |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Public Function Implementations                                           |
\----------------------------------------------------------------------------*/

void hetHandlerInit( void )
{
    /* Initialize pin configurations */
    hetHandlerInitInputPins();
    hetHandlerInitOutputPins();
}

BOOLEAN hetHandlerInitInputPins( void )
{
    int i;
    BOOLEAN ok = TRUE;
    const S_HET_INPUT_PIN_DEF* const ptr_cfg = hetConfigGetDIConfig();

    for ( i = 0; i < eHET_NUM_INPUT_PINS; i++ )
    {
        if ( ptr_cfg[ i ].enabled == TRUE )
        {
            /* Set pin direction to input */
            ptr_cfg[ i ].port->DIR &= ~( 1U << ptr_cfg[ i ].pin );

            /* Configure pull-up/down */
            if ( ptr_cfg[ i ].pull_enable == TRUE )
            {
                ptr_cfg[ i ].port->PULDIS &= ~( 1U << ptr_cfg[ i ].pin ); /* Pull enable */

                if (ptr_cfg[ i ].pull == ePULL_DOWN )
                {
                    ptr_cfg[ i ].port->PSL &= ~( 1U << ptr_cfg[ i ].pin ); /* Pull down */
                }
                else if ( ptr_cfg[ i ].pull == ePULL_UP )
                {
                    ptr_cfg[ i ].port->PSL |= ( 1U << ptr_cfg[ i ].pin );  /* Pull up */
                }
            }
            else
            {
                ptr_cfg[ i ].port->PULDIS |= ( 1U << ptr_cfg[ i ].pin );  /* Pull disable */
            }
        }
    }

    return ok;
}

BOOLEAN hetHandlerInitOutputPins( void )
{
    int i;
    BOOLEAN ok = TRUE;
    const S_HET_OUTPUT_PIN_DEF* const ptr_cfg = hetConfigGetDOConfig();

    for ( i = 0; i < eHET_NUM_OUTPUT_PINS; i++ )
    {
        if ( ptr_cfg[ i ].enabled == TRUE )
        {
            /* Set pin direction to output */
            ptr_cfg[ i ].port->DIR |= ( 1U << ptr_cfg[ i ].pin );

            /* Configure open drain */
            if ( ptr_cfg[ i ].open_drain == TRUE )
            {
                ptr_cfg[ i ].port->PDR &= ~( 1U << ptr_cfg[ i ].pin );  /* Open drain */
            }
            else
            {
                ptr_cfg[ i ].port->PDR |= ( 1U << ptr_cfg[ i ].pin);   /* Push-pull */
            }

                            /* Set initial output value */
            if (ptr_cfg[ i ].value == TRUE )
            {
                ptr_cfg[ i ].port->DSET = ( 1U << ptr_cfg[ i ].pin );  /* Set high atomically */
            }
            else
            {
                ptr_cfg[ i ].port->DCLR = ( 1U << ptr_cfg[ i ].pin ); /* Set low atomically */
            }
        }
    }

    return ok;
}

void set_het_output( E_HET_OUTPUT_PIN_ID pin_id, BOOLEAN value )
{
    const S_HET_OUTPUT_PIN_DEF* const ptr_cfg = hetConfigGetDOConfig();

    if ( ( ptr_cfg[ pin_id ].enabled == TRUE ) &&
         ( pin_id < eHET_NUM_OUTPUT_PINS ) &&
         ( ptr_cfg[ pin_id ].direction == eHET_OUTPUT ) )
    {
        if ( value == TRUE )
        {
            ptr_cfg[ pin_id ].port->DSET = ( 1U << ptr_cfg[ pin_id ].pin );  /* Set high atomically */
        }
        else
        {
            ptr_cfg[ pin_id ].port->DCLR = ( 1U << ptr_cfg[ pin_id ].pin );  /* Set low atomically */
        }
    }
}

BOOLEAN read_het_input( E_HET_INPUT_PIN_ID pin_id )
{
    BOOLEAN value = FALSE;
    const S_HET_INPUT_PIN_DEF* const ptr_cfg = hetConfigGetDIConfig();

    if ( ( ptr_cfg[ pin_id ].enabled == TRUE ) &&
         ( pin_id < eHET_NUM_INPUT_PINS ) &&
         ( ptr_cfg[ pin_id ].direction == eHET_INPUT ) )
    {
        value = ( ptr_cfg[ pin_id ].port->DIN & ( 1U << ptr_cfg[ pin_id ].pin ) ) ? TRUE : FALSE;
    }

    return value;
}

void read_het_inputs( void )
{
    int i;
    const S_HET_INPUT_PIN_DEF* const ptr_cfg = hetConfigGetDIConfig();

    for ( i = 0; i < eHET_NUM_INPUT_PINS; i++ )
    {
        if ( ptr_cfg[ i ].enabled == TRUE )
        {
            read_het_input( ptr_cfg[ i ].id );
        }
    }
}

const S_HET_INPUT_PIN_DEF* const hetConfigGetDIConfig( void )
{
    /* Return the address of the HET Inputs configuration data */
    return het_input_pin_defs;
}

const S_HET_OUTPUT_PIN_DEF* const hetConfigGetDOConfig( void )
{
    /* Return the address of the HET Outputs configuration data */
    return het_output_pin_defs;
}

/*----------------------------------------------------------------------------\
|   Private Function Implementations                                          |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   End of fw_gio_het.c module                                                |
\----------------------------------------------------------------------------*/

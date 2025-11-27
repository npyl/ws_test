/*----------------------------------------------------------------------------\
|          Copyright(c) 2025 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : fw_gio_mibspi.c Module File.                               |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 23 Feb 2025                                                |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   MIBSPI pin configuration and handling module                              |
|                                                                             |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

#include "HL_mibspi.h"

#include "fw_types.h"
#include "fw_gio_mibspi.h"

/*----------------------------------------------------------------------------\
|   Private Type Definitions                                                  |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Private Constant Definitions                                              |
\----------------------------------------------------------------------------*/

static const S_MIBSPI_INPUT_PIN_DEF mibspi_input_pin_defs[ eMIBSPI_NUM_INPUT_PINS ] =
{
    /* MIBSPI Input Pins - Add when needed */
};

static const S_MIBSPI_OUTPUT_PIN_DEF mibspi_output_pin_defs[ eMIBSPI_NUM_OUTPUT_PINS ] =
{
    /* MIBSPI Output Pins */
    {
        /* W8, SIMO, DemodEnable */
        .label = "DemodEna",
        .id = eOUT_PIN_W8_DemodEna,
        .module = eMIBSPI3,
        .base = mibspiREG3,
        .pin = PIN_SIMO,
        .open_drain = FALSE,
        .direction = eMIBSPI_OUTPUT,
        .value = FALSE,
        .enabled = TRUE,
    },
    {
        /* V8, SOMI, GainActive2 */
        .label = "GainActive2",
        .id = eOUT_PIN_V8_GainActive2,
        .module = eMIBSPI3,
        .base = mibspiREG3,
        .pin = PIN_SOMI,
        .open_drain = FALSE,
        .direction = eMIBSPI_OUTPUT,
        .value = FALSE,
        .enabled = TRUE,
    },
    {
        /* W9, ENABLE, GainActive1 */
        .label = "GainActive1",
        .id = eOUT_PIN_W9_GainActive1,
        .module = eMIBSPI3,
        .base = mibspiREG3,
        .pin = PIN_ENA,
        .open_drain = FALSE,
        .direction = eMIBSPI_OUTPUT,
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

void mibspiHandlerInit( void )
{
    /* Initialize pin configurations */
    mibspiHandlerInitInputPins();
    mibspiHandlerInitOutputPins();
}

BOOLEAN mibspiHandlerInitInputPins( void )
{
    int i;
    BOOLEAN ok = TRUE;
    const S_MIBSPI_INPUT_PIN_DEF* const ptr_cfg = mibspiConfigGetDIConfig();

    for ( i = 0; i < eMIBSPI_NUM_INPUT_PINS; i++ )
    {
        if ( ptr_cfg[ i ].enabled == TRUE )
        {
            /* Configure pin as GPIO (not functional MIBSPI) by clearing PC0 */
            ptr_cfg[ i ].base->PC0 &= ~( 1U << ptr_cfg[ i ].pin );

            /* Set pin direction to input */
            ptr_cfg[ i ].base->PC1 &= ~( 1U << ptr_cfg[ i ].pin );

            /* Configure pull-up/down if supported by MIBSPI */
            if ( ptr_cfg[ i ].pull_enable == TRUE )
            {
                /* PC6 - Pull disable register (1 = disable, 0 = enable) */
                ptr_cfg[ i ].base->PC6 &= ~( 1U << ptr_cfg[ i ].pin );  /* Pull enable */

                if ( ptr_cfg[ i ].pull == ePULL_DOWN )
                {
                    /* PC7 - Pull selection register (0 = down, 1 = up) */
                    ptr_cfg[ i ].base->PC7 &= ~( 1U << ptr_cfg[ i ].pin );  /* Pull down */
                }
                else if ( ptr_cfg[ i ].pull == ePULL_UP )
                {
                    /* PC7 - Pull selection register (0 = down, 1 = up) */
                    ptr_cfg[ i ].base->PC7 |= ( 1U << ptr_cfg[ i ].pin );   /* Pull up */
                }
            }
            else
            {
                /* PC6 - Pull disable register (1 = disable, 0 = enable) */
                ptr_cfg[ i ].base->PC6 |= ( 1U << ptr_cfg[ i ].pin );   /* Pull disable */
            }
        }
    }

    return ok;
}

BOOLEAN mibspiHandlerInitOutputPins( void )
{
    int i;
    BOOLEAN ok = TRUE;
    const S_MIBSPI_OUTPUT_PIN_DEF* const ptr_cfg = mibspiConfigGetDOConfig();

    for ( i = 0; i < eMIBSPI_NUM_OUTPUT_PINS; i++ )
    {
        if ( ptr_cfg[ i ].enabled == TRUE )
        {
            /* Configure pin as GPIO (not functional MIBSPI) by clearing PC0 */
            ptr_cfg[ i ].base->PC0 &= ~( 1U << ptr_cfg[ i ].pin );

            /* Set pin direction to output */
            ptr_cfg[ i ].base->PC1 |= ( 1U << ptr_cfg[ i ].pin );

            /* Configure open drain if supported by MIBSPI */
            if ( ptr_cfg[ i ].open_drain == TRUE )
            {
                /* PC3 - Open drain register (1 = open drain, 0 = push-pull) */
                ptr_cfg[ i ].base->PC3 |= ( 1U << ptr_cfg[ i ].pin );   /* Open drain */
            }
            else
            {
                /* PC3 - Open drain register (1 = open drain, 0 = push-pull) */
                ptr_cfg[ i ].base->PC3 &= ~( 1U << ptr_cfg[ i ].pin );  /* Push-pull */
            }

            /* Set initial output value - use PC4 to set and PC5 to clear - atomic operations */
            if ( ptr_cfg[ i ].value == TRUE )
            {
                ptr_cfg[ i ].base->PC4 = ( 1U << ptr_cfg[ i ].pin );    /* Set high atomically */
            }
            else
            {
                ptr_cfg[ i ].base->PC5 = ( 1U << ptr_cfg[ i ].pin );    /* Set low atomically */
            }
        }
    }

    return ok;
}

void set_mibspi_output( E_MIBSPI_OUTPUT_PIN_ID pin_id, BOOLEAN value )
{
    const S_MIBSPI_OUTPUT_PIN_DEF* const ptr_cfg = mibspiConfigGetDOConfig();

    if ( ( ptr_cfg[ pin_id ].enabled == TRUE ) &&
         ( pin_id < eMIBSPI_NUM_OUTPUT_PINS ) &&
         ( ptr_cfg[ pin_id ].direction == eMIBSPI_OUTPUT ) )
    {
        if ( value == TRUE )
        {
            /* Use PC4 to set high atomically */
            ptr_cfg[ pin_id ].base->PC4 = ( 1U << ptr_cfg[ pin_id ].pin );
        }
        else
        {
            /* Use PC5 to set low atomically */
            ptr_cfg[ pin_id ].base->PC5 = ( 1U << ptr_cfg[ pin_id ].pin );
        }
    }
}

BOOLEAN read_mibspi_input( E_MIBSPI_INPUT_PIN_ID pin_id )
{
    BOOLEAN value = FALSE;
    const S_MIBSPI_INPUT_PIN_DEF* const ptr_cfg = mibspiConfigGetDIConfig();

    if ( ( ptr_cfg[ pin_id ].enabled == TRUE ) &&
         ( pin_id < eMIBSPI_NUM_INPUT_PINS ) &&
         ( ptr_cfg[ pin_id ].direction == eMIBSPI_INPUT ) )
    {
        /* Use PC2 to read pin status */
        value = ( ptr_cfg[ pin_id ].base->PC2 & ( 1U << ptr_cfg[ pin_id ].pin ) ) ? TRUE : FALSE;
    }

    return value;
}

void read_mibspi_inputs( void )
{
    int i;
    const S_MIBSPI_INPUT_PIN_DEF* const ptr_cfg = mibspiConfigGetDIConfig();

    for ( i = 0; i < eMIBSPI_NUM_INPUT_PINS; i++ )
    {
        if ( ptr_cfg[ i ].enabled == TRUE )
        {
            read_mibspi_input( ptr_cfg[ i ].id );
        }
    }
}

const S_MIBSPI_INPUT_PIN_DEF* const mibspiConfigGetDIConfig( void )
{
    return mibspi_input_pin_defs;
}

const S_MIBSPI_OUTPUT_PIN_DEF* const mibspiConfigGetDOConfig( void )
{
    return mibspi_output_pin_defs;
}

/*----------------------------------------------------------------------------\
|   Private Function Implementations                                          |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   End of fw_gio_mibspi.c module                                             |
\----------------------------------------------------------------------------*/

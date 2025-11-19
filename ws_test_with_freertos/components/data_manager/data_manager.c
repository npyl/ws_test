/*----------------------------------------------------------------------------\
|          Copyright(c) 2024 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : data_manager.c Module File.                                |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 8 …·Ì 2024                                                 |
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

#include "data_manager.h"

/*----------------------------------------------------------------------------\
|   Private Type Definitions                                                  |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Private Constant Definitions                                              |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Private Data Declarations                                                 |
\----------------------------------------------------------------------------*/

/* Data structures. */
static S_DM_DATABASE                dm_database;

/* Instantiate the Data Manager Transport structures. */
static S_DM_DIGITAL_IO              dm_digitals_dataset;
static S_DM_ANALOGUE_INPUTS         dm_analogue_inputs_dataset;

/*----------------------------------------------------------------------------\
|   Public Constant Declarations                                              |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Private Function Declarations                                             |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Public Function Implementations                                           |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Private Function Implementations                                          |
\----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------\
|                                                                             |
|   Procedure           : dmDataManagerInit                                   |
|                                                                             |
|   Description         : This function initialises the Hercules              |
|                         Board Data Manager.                                 |
|                                                                             |
|                                                                             |
|   Inputs              : None.                                               |
|                                                                             |
|   Outputs             : None.                                               |
|                                                                             |
|   Return              : None.                                               |
|                                                                             |
|   Warnings            : Must be called by initialisation process, before    |
|                         the operating system starts to run.                 |
|                                                                             |
\----------------------------------------------------------------------------*/

void dmDataManagerInit( void )
{
    /* TODO Initialize non-volatile data here */

    /* TODO HIGH Assign all structure pointers to transport structures */
    dm_digitals_dataset.ptr_digital_io = &dm_database.digital_io;
    dm_analogue_inputs_dataset.ptr_analogue_inputs = &dm_database.analogue_inputs;
}

/*----------------------------------------------------------------------------\
|                                                                             |
|   Procedure           : dmFullDataAccess                                    |
|                                                                             |
|   Description         : This function returns a pointer to the total        |
|                         structure                                           |
|                                                                             |
|   Inputs              : None.                                               |
|                                                                             |
|   Outputs             : None.                                               |
|                                                                             |
|   Return              : Pointer to the database access structure.           |
|                                                                             |
|   Warnings            : None.                                               |
|                                                                             |
\----------------------------------------------------------------------------*/

S_DM_DATABASE *dmFullDataAccess( void )
{
    return &dm_database;
}

/*----------------------------------------------------------------------------\
|                                                                             |
|   Procedure           : dmDigitalssAccess                                   |
|                                                                             |
|   Description         : This function returns a pointer to the              |
|                         Digitals                                            |
|                                                                             |
|   Inputs              : None.                                               |
|                                                                             |
|   Outputs             : None.                                               |
|                                                                             |
|   Return              : Pointer to the database access structure.           |
|                                                                             |
|   Warnings            : None.                                               |
|                                                                             |
\----------------------------------------------------------------------------*/

S_DM_DIGITAL_IO *dmDigitalsAccess( void )
{
    return &dm_digitals_dataset;
}

/*----------------------------------------------------------------------------\
|                                                                             |
|   Procedure           : dmAnalogueInputsAccess                              |
|                                                                             |
|   Description         : This function returns a pointer to the              |
|                         Analogue Inputs                                     |
|                                                                             |
|   Inputs              : None.                                               |
|                                                                             |
|   Outputs             : None.                                               |
|                                                                             |
|   Return              : Pointer to the database access structure.           |
|                                                                             |
|   Warnings            : None.                                               |
|                                                                             |
\----------------------------------------------------------------------------*/

S_DM_ANALOGUE_INPUTS *dmAnalogueInputsAccess( void )
{
    return &dm_analogue_inputs_dataset;
}

/*----------------------------------------------------------------------------\
|   End of data_manager.c module                                              |
\----------------------------------------------------------------------------*/

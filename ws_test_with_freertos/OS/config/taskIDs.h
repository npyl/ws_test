/*----------------------------------------------------------------------------\
|          Copyright(c) 2024 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : taskIDs.h Header File.                                     |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 7 …·Ì 2024                                                 |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   <Module Header Description>                                               |
|                                                                             |
\----------------------------------------------------------------------------*/

#ifndef taskIDs_H
#define taskIDs_H

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Public Type Definitions                                                   |
\----------------------------------------------------------------------------*/

/*
 * Task IDs
 */
typedef enum
{
    /*
     * Include here the application specific Task IDs.
     * MUST be in same order as in TaskParamsList and TaskConfigList.
     */
    E_TASKID_C0_GK = 0,
    E_TASKID_C0_2MS,
    E_TASKID_C0_10MS,
    E_TASKID_C0_25MS,
    E_TASKID_C0_50MS,
    E_TASKID_C0_100MS,
    E_TASKID_C0_500MS,
    E_TASKID_C0_1000MS,
    E_TASKID_C0_2000MS,
    E_TASKID_C0_5000MS,
    E_TASKID_C0_UART_GK,
    E_TASKID_C0_I2C_GK,
    E_TASKID_C0_SPI_GK,
    E_TASKID_MAX
} E_TASKID;

/*----------------------------------------------------------------------------\
|   Public Constant Declarations                                              |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Public Data Declarations                                                  |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Public Function Declarations                                              |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   End of taskIDs.h header file                                              |
\----------------------------------------------------------------------------*/

#endif  /* taskIDs_H */

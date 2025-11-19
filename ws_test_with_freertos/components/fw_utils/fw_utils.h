/*----------------------------------------------------------------------------\
|          Copyright(c) 2025 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : fw_utils.h Header File.                                    |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 26 …·Ì 2025                                                |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   <Module Header Description>                                               |
|                                                                             |
\----------------------------------------------------------------------------*/

#ifndef fw_utils_H
#define fw_utils_H

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Public Type Definitions                                                   |
\----------------------------------------------------------------------------*/

typedef volatile struct rtiBase
{
    volatile uint32_t GCTRL;      // 0x0000: Global Control Register
    volatile uint32_t TBCTRL;     // 0x0004: Timebase Control Register
    volatile uint32_t CAPCTRL;    // 0x0008: Capture Control Register
    volatile uint32_t COMPCTRL;   // 0x000C: Compare Control Register
    struct
    {
        volatile uint32_t FRCx;    // 0x0010, 0x0030: Free Running Counter x
        volatile uint32_t UCx;     // 0x0014, 0x0034: Up Counter x
        volatile uint32_t CPUCx;   // 0x0018, 0x0038: Compare Up Counter x
        volatile uint32_t reserved; // 0x001C, 0x003C: Reserved
    } CNT[2];
} rtiBASE_t;

/*----------------------------------------------------------------------------\
|   Public Constant Declarations                                              |
\----------------------------------------------------------------------------*/

#define rtiREG1 ( ( rtiBASE_t * ) 0xFFFFFC00 )

/*----------------------------------------------------------------------------\
|   Public Function Declarations                                              |
\----------------------------------------------------------------------------*/

void delayMicroseconds( uint32_t microseconds );

/*----------------------------------------------------------------------------\
|   End of fw_utils.h header file                                             |
\----------------------------------------------------------------------------*/

#endif  /* fw_utils_H */

/*----------------------------------------------------------------------------\
|          Copyright(c) 2024 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : fw_types.h Header File.                                    |
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

#ifndef fw_types_H
#define fw_types_H

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Public Constant Definitions                                               |
\----------------------------------------------------------------------------*/

#ifndef FALSE
    #define FALSE        0
#endif

#ifndef TRUE
    #define TRUE         1
#endif

#ifndef NULL
    #define NULL         0
#endif

#ifndef ERROR
    #define ERROR       -1
#endif

/*----------------------------------------------------------------------------\
|   Public Type Definitions                                                   |
\----------------------------------------------------------------------------*/

typedef unsigned char           BYTE;
typedef unsigned short          WORD;
typedef unsigned char           BOOLEAN;
typedef unsigned char           U8;
typedef signed char             S8;
typedef char                    CHAR;
typedef unsigned short          U16;
typedef signed short            S16;
typedef unsigned long           U32;
typedef signed long             S32;
typedef unsigned long long      U64;
typedef signed long long        S64;
typedef float                   FLOAT;
typedef double                  DOUBLE;

/* Added for general purpose application */
typedef signed char             portInt8Type;
typedef void *                  portTaskHandleType;
typedef signed long             portBaseType;
typedef unsigned long           portUnsignedBaseType;

/*----------------------------------------------------------------------------\
|   Public Function Declarations                                              |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   End of fw_types.h header file                                             |
\----------------------------------------------------------------------------*/

#endif  /* fw_types_H */

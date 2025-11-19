/*----------------------------------------------------------------------------\
|          Copyright(c) 2024 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : fw_globals.h Header File.                                  |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 16 Ιαν 2024                                                |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   <Module Header Description>                                               |
|                                                                             |
\----------------------------------------------------------------------------*/

#ifndef fw_globals_H
#define fw_globals_H

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------\
|   Public Type Definitions                                                   |
\----------------------------------------------------------------------------*/

typedef enum
{
    ePULL_NONE = 0u,
    ePULL_UP,
    ePULL_DOWN,
} E_PULL_UP_DOWN;

/*----------------------------------------------------------------------------\
|   Public Constant Declarations                                              |
\----------------------------------------------------------------------------*/

#define portQUEUE_OVERHEAD_BYTES    ( 96UL )

/* Development kit LEDs */
#define LEFT_TOP            29u
#define LEFT_MIDDLE         18u
#define LEFT_BOTTOM         25u
#define RIGHT_TOP           17u
#define RIGHT_MIDDLE        31u
#define RIGHT_BOTTOM        0u
#define LED_5               5u
#define LED_27              27u

#ifdef ARCH_LITTLE_ENDIAN_

#define LSB16( x )          ( ( INT8U ) ( ( ( x ) >> 8 ) & 0x00FFU ) )
#define MSB16( x )          ( ( INT8U ) ( ( x ) & 0x00FFU ) )

#define LSB32( x )          ( ( INT8U ) ( ( x >> 16 ) & 0x0000FFFFU ) )
#define MSB32( x )          ( ( INT8U ) ( x & 0xFFFFU ) )

#define MSB32_0( x )        ( ( INT8U ) ( ( ( x ) >> 24 ) & ( 0xFF ) ) )
#define MSB32_1( x )        ( ( INT8U ) ( ( ( x ) >> 16 ) & ( 0xFF ) ) )
#define MSB32_2( x )        ( ( INT8U ) ( ( ( x ) >> 8 ) & ( 0xFF ) ) )
#define MSB32_3( x )        ( ( INT8U ) ( ( x ) & ( 0xFF ) ) )

#else

/* MSB/LSB 16 & 32 bits */
#define MSB16( x )          ( ( INT8U ) ( ( ( x ) >> 8 ) & 0x00FFU ) )
#define LSB16( x )          ( ( INT8U ) ( ( x ) & 0x00FFU ) )

#define MSB32( x )          ( ( INT8U ) ( ( x >> 16 ) & 0x0000FFFFU ) )
#define LSB32( x )          ( ( INT8U ) ( x & 0xFFFFU ) )

#define MSB32_3( x )        ( ( INT8U ) ( ( ( x ) >> 24 ) & ( 0xFF ) ) )
#define MSB32_2( x )        ( ( INT8U ) ( ( ( x ) >> 16 ) & ( 0xFF ) ) )
#define MSB32_1( x )        ( ( INT8U ) ( ( ( x ) >> 8 ) & ( 0xFF ) ) )
#define MSB32_0( x )        ( ( INT8U ) ( ( x ) & ( 0xFF ) ) )

#endif

/* *************************************************************************
*  BASIC TYPE DEFINTIONS
************************************************************************* */

typedef signed char         INT8;
typedef unsigned char       INT8U;
typedef signed short        INT16;
typedef unsigned short      INT16U;
typedef signed long int     INT32;
typedef unsigned long int   INT32U;

typedef signed char *       PINT8;
typedef unsigned char *     PINT8U;
typedef signed short  *     PINT16;
typedef unsigned short *    PINT16U;
typedef signed long int *   PINT32;
typedef unsigned long int * PINT32U;

/* Declares a two byte 'structure', containing 'LSB', and 'MSB' - does not actually create it though */
/* Change sequence for Little vs Big endian */
#ifdef ARCH_LITTLE_ENDIAN

struct wordparts
{
    INT8U LSB;
    INT8U MSB;
};

#else

struct wordparts
{
    INT8U MSB;
    INT8U LSB;
};

#endif

#ifdef ARCH_LITTLE_ENDIAN

struct wordparts4
{
    INT8U Byte0;    /* LSB */
    INT8U Byte1;
    INT8U Byte2;
    INT8U Byte3;    /* MSB */
};

#else

struct wordparts4
{
    INT8U Byte3;    /* MSB */
    INT8U Byte2;
    INT8U Byte1;
    INT8U Byte0;    /* LSB */
};

#endif

typedef union
{
    INT16U word16;
    struct wordparts bytes;
} type_INT16U;

typedef union
{
    INT32U word32;
    struct wordparts4 bytes;
} type_INT32U;

/*----------------------------------------------------------------------------\
|   Public Function Declarations                                              |
\----------------------------------------------------------------------------*/

void toggle_led( int led, int * val );

/*----------------------------------------------------------------------------\
|   End of fw_globals.h header file                                           |
\----------------------------------------------------------------------------*/

#endif  /* fw_globals_H */

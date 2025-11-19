/*----------------------------------------------------------------------------\
|          Copyright(c) 2024 Jerry Pylarinos.                                 |
|          This program is protected by copyright and information             |
|          contained therein is confidential. The program may not be          |
|          copied and the information may not be used or disclosed            |
|          except with the written permission of the proprietor(s).           |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   Module Name  : fw_pwm.h Header File.                                      |
|                                                                             |
|   Author       : Jerry Pylarinos                                            |
|                                                                             |
|   Date         : 25 …·Ì 2024                                                |
|                                                                             |
|-----------------------------------------------------------------------------|
|                                                                             |
|   <Module Header Description>                                               |
|                                                                             |
\----------------------------------------------------------------------------*/

#ifndef fw_pwm_H
#define fw_pwm_H

/*----------------------------------------------------------------------------\
|   Header Files                                                              |
\----------------------------------------------------------------------------*/

#include "fw_types.h"

/*----------------------------------------------------------------------------\
|   Public Type Definitions                                                   |
\----------------------------------------------------------------------------*/

typedef enum
{
    ePWM_A = 0,
    ePWM_B,
    ePWM_MAX,
} E_PWM;

typedef enum
{
    eETPWM_0 = 0,
    eETPWM_1,
    eETPWM_2,
    eETPWM_3,
    eETPWM_4,
    eETPWM_5,
    eETPWM_6,
    eETPWM_MAX,
} E_ETPWM;

typedef struct
{
    E_PWM       id;                 /* ePWM_A .. ePWM_B */
    CHAR        label[ 32u ];
    BOOLEAN     polarity;           /* HIGH / LOW */
    U8          duty;               /* 0 .. 100 % */
    U16         period;             /* ns */
    BOOLEAN     delay_en;           /* Delay enable / disable */
    BOOLEAN     invert_polarity;    /* TRUE / FALSE */
} S_PWM_CH;

typedef struct
{
    E_ETPWM     id;
    CHAR        label[ 32u ];
    E_PWM       rise_delay_source;  /* ePWM_A .. ePWM_B */
    U8          rise_delay;         /* ns */
    E_PWM       fall_delay_source;  /* ePWM_A .. ePWM_B */
    U8          fall_delay;         /* ns */
    S_PWM_CH    ch[ ePWM_MAX ];     /* PWM channel */
    BOOLEAN     enabled;
} S_ETPMW_CONFIG;

/*----------------------------------------------------------------------------\
|   Public Constant Declarations                                              |
\----------------------------------------------------------------------------*/

/* Maps ETPWM enum to register */
#define ETPWM( x )      ( ( ( x ) == eETPWM_0 ) ? etpwmREG1 : \
                          ( ( x ) == eETPWM_1 ) ? etpwmREG2 : \
                          ( ( x ) == eETPWM_2 ) ? etpwmREG3 : \
                          ( ( x ) == eETPWM_3 ) ? etpwmREG4 : \
                          ( ( x ) == eETPWM_4 ) ? etpwmREG5 : \
                          ( ( x ) == eETPWM_5 ) ? etpwmREG6 : etpwmREG7 )

/* Maps register to ETPWM enum */
#define ETPWM_IDX( x )  ( ( ( x ) == etpwmREG1 ) ? eETPWM_0 : \
                          ( ( x ) == etpwmREG2 ) ? eETPWM_1 : \
                          ( ( x ) == etpwmREG3 ) ? eETPWM_2 : \
                          ( ( x ) == etpwmREG4 ) ? eETPWM_3 : \
                          ( ( x ) == etpwmREG5 ) ? eETPWM_4 : \
                          ( ( x ) == etpwmREG6 ) ? eETPWM_5 : eETPWM_6 )

/*----------------------------------------------------------------------------\
|   Public Function Declarations                                              |
\----------------------------------------------------------------------------*/

const S_ETPMW_CONFIG * const etpwmtGetConfig( void );
void etpwm_init( void );

/*----------------------------------------------------------------------------\
|   End of fw_pwm.h header file                                               |
\----------------------------------------------------------------------------*/

#endif  /* fw_pwm_H */

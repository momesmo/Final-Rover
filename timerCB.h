#ifndef _TIMERCB_H    /* Guard against multiple inclusion */
#define _TIMERCB_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"

#include "timers.h"

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif
    
#define MOTOR_TIMER_PERIOD 100
    
    
    TimerHandle_t motorTMR;

    void vEncoderCallbackFunction( TimerHandle_t xTimer );
    void createAndStartMotorTimer();


    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _TIMERCB_H */

/* *****************************************************************************
 End of File
 */

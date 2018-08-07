#ifndef _SENSOR_H
#define _SENSOR_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"

#include "queues.h"
#include "debug.h"
#include "timers.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END
    
    //globals for i2c
    int readControl = 0;
    int firstEntry = 0;
    char cmdBuff[1];
    unsigned char sensorValues[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    enum states {ENABLE, COLLECT, TASKS, SEND};
    enum states s = ENABLE;
    char enBuff[2];
    
    TimerHandle_t sensorTMR;
    TimerHandle_t ledTMR;
    DRV_HANDLE handle;
    DRV_I2C_BUFFER_HANDLE bufHandle;
    
    void vSensorCallbackFunction( TimerHandle_t xTimer );
    void createAndStartSensorTimer();
    void vLEDCallbackFunction( TimerHandle_t xTimer );
    void createAndStartLEDTimer();
    
    //i2c functions
    void enableSensor();
    void readFromRGBRegisters();
    
    void SENSOR_Initialize ( void );
    void SENSOR_Tasks( void );


#endif /* _SENSOR_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */


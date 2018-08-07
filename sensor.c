#include "sensor.h"
#include "system/common/sys_buffer.h"

    
void vSensorCallbackFunction( TimerHandle_t xTimer ){
    BaseType_t timerHigherPriorityTaskWoken = pdFALSE;
    dbgOutputEvent(SENSORCALLBACK_START);
    
    readFromRGBRegisters();
   
    unsigned int high = sensorValues[1];
    unsigned int low = sensorValues[0];
    high = high << 8;
    unsigned int c = high + low;

    high = sensorValues[3];
    low = sensorValues[2];
    high = high << 8;
    unsigned int r = high + low;

    high = sensorValues[5];
    low = sensorValues[4];
    high = high << 8;
    unsigned int g = high + low;

    high = sensorValues[7];
    low = sensorValues[6];
    high = high << 8;
    unsigned int b = high + low;

    sensorQueueSendFromISR(c*r, c*g, c*b, &timerHigherPriorityTaskWoken);


    dbgOutputEvent(SENSORCALLBACK_END);
    portEND_SWITCHING_ISR(timerHigherPriorityTaskWoken);
}

void createAndStartSensorTimer(){
    sensorTMR = xTimerCreate
            ("Sensor_TMR", /*Text name, not used by RTOS*/
            pdMS_TO_TICKS(125), /*Timer period in MS, converts to ticks*/
            pdTRUE, /*Timer auto-reloads when expired*/
            (void * ) 0, /*Stores the count of times timer expires, starts 0*/
            vSensorCallbackFunction); /*Name of callback function*/
    if(sensorTMR == NULL){
        haltAll(ERRORCODE);
    }else{
        if(xTimerStart(sensorTMR, 0) != pdPASS){
            haltAll(ERRORCODE);
        }
    }
}

void vLEDCallbackFunction( TimerHandle_t xTimer ){
//    dbgOutputEvent(LEDCALLBACK_START);
    static int state = 1;
    switch(state){
        case 0:
            PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_3);
            state = 1;
            break;
        case 1:
            PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_3);
            state = 0;
            break;
    }
//    dbgOutputEvent(LEDCALLBACK_END);
}

void createAndStartLEDTimer(){
    ledTMR = xTimerCreate
            ("LED_TMR", /*Text name, not used by RTOS*/
            pdMS_TO_TICKS(500), /*Timer period in MS, converts to ticks*/
            pdTRUE, /*Timer auto-reloads when expired*/
            (void * ) 0, /*Stores the count of times timer expires, starts 0*/
            vLEDCallbackFunction); /*Name of callback function*/
    if(ledTMR == NULL){
        haltAll(ERRORCODE);
    }else{
        if(xTimerStart(ledTMR, 0) != pdPASS){
            haltAll(ERRORCODE);
        }
    }
}

void readFromRGBRegisters(){
    dbgOutputEvent(START_R_READ);
    
    readControl = 0;
    if(SYS_STATUS_READY != DRV_I2C_Status(sysObj.drvI2C0))
        dbgOutputEvent(I2C_DRV_NOT_READY);
    handle = DRV_I2C_Open(DRV_I2C_INDEX_0, DRV_IO_INTENT_WRITE);
    cmdBuff[0] = 0xB4;
    int slaveAdd;
    slaveAdd = 0x29<<1;
    bufHandle = DRV_I2C_TransmitThenReceive(handle, slaveAdd, cmdBuff, 1, sensorValues, 8, NULL);
    DRV_I2C_Close(handle);
    
    dbgOutputEvent(STOP_R_READ);
}

void enableSensor()
{
    dbgOutputEvent(START_ENABLE_SENSOR);
    if(SYS_STATUS_READY != DRV_I2C_Status(sysObj.drvI2C0))
        dbgOutputEvent(I2C_DRV_NOT_READY);
    handle = DRV_I2C_Open(DRV_I2C_INDEX_0, DRV_IO_INTENT_WRITE);
    enBuff[0] = 0x80;
    enBuff[1] = 0x03;
    int slaveAdd;
    slaveAdd = 0x29<<1;
    if(DRV_I2C_Transmit(handle, slaveAdd, enBuff, 2, NULL) == (DRV_I2C_BUFFER_HANDLE)NULL){
        dbgOutputEvent(0x91);
        haltAll(ERRORCODE);
    }
    DRV_I2C_Close(handle);
    dbgOutputEvent(SENSOR_ENABLED);
    
}

void SENSOR_Initialize ( void )
{
    dbgOutputEvent(SENSOR_INIT);
    sensorQueueCreate();
    motorQueueCreate();
    rxQueueCreate();
    txQueueCreate();
    feedbackQueueCreate();
    
//    enableSensor();
}

void SENSOR_Tasks ( void )
{
    dbgOutputEvent(SENSOR_TASK_START);
    SensorMsg sm;
    
    
    //TODO change back to 0
    int case_begin = 0;
    while(1){
        sm = sensorQueueRecv();
        switch(case_begin){
            case 0: // Waiting for message from RX
                if(sm.source == rx && sm.signal == 'g'){
                    dbgOutputEvent(SENSOR_RX_MSG);
                    enableSensor();
                    createAndStartSensorTimer();
                    createAndStartLEDTimer();
                    case_begin = 1;
                }
                break;
            case 1: // Process sensor data
                if(sm.source == sensor){
                    dbgOutputEvent(SENSOR_SENSOR_MSG);
                    feedbackQueueSendFromSensor(sm.r, sm.g, sm.b);
                }else if(sm.source == rx){
                }
                break;
        }
    }
}

 

/*******************************************************************************
 End of File
 */

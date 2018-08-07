#include "timerCB.h"

void vEncoderCallbackFunction( TimerHandle_t xTimer ){
    BaseType_t timerHigherPriorityTaskWoken = pdFALSE;
    dbgOutputEvent(MOTORCALLBACK_START);
    
    
    motorQueueSendFromISR((int) DRV_TMR2_CounterValueGet(), (int) DRV_TMR1_CounterValueGet(), &timerHigherPriorityTaskWoken);
    
    dbgOutputEvent(MOTORCALLBACK_END);
    portEND_SWITCHING_ISR(timerHigherPriorityTaskWoken);
}

void createAndStartMotorTimer(){
    motorTMR = xTimerCreate
            ("Motor_TMR", /*Text name, not used by RTOS*/
            pdMS_TO_TICKS(MOTOR_TIMER_PERIOD), /*Timer period in MS, converts to ticks*/
            pdTRUE, /*Timer auto-reloads when expired*/
            (void * ) 0, /*Stores the count of times timer expires, starts 0*/
            vEncoderCallbackFunction); /*Name of callback function*/
    if(motorTMR == NULL){
        haltAll(ERRORCODE);
    }else{
        if(xTimerStart(motorTMR, 0) != pdPASS){
            haltAll(ERRORCODE);
        }
    }
}

/* *****************************************************************************
 End of File
 */

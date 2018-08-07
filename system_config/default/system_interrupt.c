#include "system/common/sys_common.h"
#include "sensor.h"
#include "motor.h"
#include "rx.h"
#include "tx.h"
#include "system_definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector Functions
// *****************************************************************************
// *****************************************************************************


void IntHandlerDrvI2CInstance0(void)
{
    DRV_I2C_Tasks(sysObj.drvI2C0);
 
}

 
   

 
 
 

 




void IntHandlerDrvUsartInstance0(void)
{
    BaseType_t txHigherPriorityTaskWoken = pdFALSE;
    BaseType_t rxHigherPriorityTaskWoken = pdFALSE;
    
    dbgOutputEvent(ISR_START);//ISR_START
    //Tx and Rx ISR
    //SYS_INT_SourceStatusGet(INT_SOURCE_USART_1_RECEIVE/TRANSMIT)
    if(SYS_INT_SourceStatusGet(INT_SOURCE_USART_1_RECEIVE))
    {
        dbgOutputEvent(RX_ISR_START);
        static ByteArray ba;
        static int recvCount = 0;
        //handle receive
        //check if receive buffer is empty
        if(PLIB_USART_ReceiverDataIsAvailable(USART_ID_1)){
            dbgOutputEvent(RX_DATA_AVAILABLE);
            ba.c[recvCount] = PLIB_USART_ReceiverByteReceive(USART_ID_1);
//            dbgOutputVal(ba.c[recvCount]);
            if(ba.c[recvCount] == '}'){
                dbgOutputEvent(RX_END);
                rxQueueSendFromISR(ba, &rxHigherPriorityTaskWoken);
                dbgOutputEvent(RX_MSG_SENT_FROM_ISR);
                recvCount = -1;
            }
            recvCount++;
        }
        PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_RECEIVE);
    }
    if(SYS_INT_SourceStatusGet(INT_SOURCE_USART_1_TRANSMIT))
    {
        //handle transfer
//        dbgOutputEvent(TX_ISR_START);
        //set up for state machine
        enum States{stillSending, startSending};
        static enum States s = startSending;
        static ByteArray array;
        static int count = 0;
        switch(s){
            case(startSending):
//                dbgOutputEvent(TX_ENTER_StartSending);
                if(xQueueIsQueueEmptyFromISR(TxQueue) == pdFALSE){
                    array = txQueueRecvFromISR(&txHigherPriorityTaskWoken);//receive byte array from queue
                    dbgOutputEvent(TX_MSG_RECV);
                    s = stillSending;
                }
                else{
                    SYS_INT_SourceDisable(INT_SOURCE_USART_1_TRANSMIT);
                    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
                    dbgOutputEvent(TX_INTS_DISABLED);
                    break;
                }
            case(stillSending):
                //write buffer[count] to UART
                dbgOutputEvent(TX_ENTER_StillSending);
//                dbgOutputVal(array.c[count]);
                if(!PLIB_USART_TransmitterBufferIsFull(USART_ID_1)){
                    PLIB_USART_TransmitterByteSend(USART_ID_1, array.c[count]);
                    dbgOutputEvent(TX_BYTE_WRITTEN);
                    //check if count has reached end of buffer
                    if(array.c[count] == '}')
                    {
                        dbgOutputEvent(TX_BUFFER_WRITTEN);
                        s = startSending;//set state to start sending.
                        count = 0;
                        array.c[0] = 0;
                    }else{
                        //increment count
                        count++;
                    }
                }
				break;
            default:
                haltAll(ERRORCODE);
                break;
        }
    }
    portEND_SWITCHING_ISR(txHigherPriorityTaskWoken);
    portEND_SWITCHING_ISR(rxHigherPriorityTaskWoken);
}
 
 
 

 

 

 

 

 
 
 

void IntHandlerDrvTmrInstance0(void)
{
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_2);
}
void IntHandlerDrvTmrInstance1(void)
{
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_3);
}
void IntHandlerDrvTmrInstance2(void)
{
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_4);
}
 
/*******************************************************************************
 End of File
*/

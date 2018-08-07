#include "rx.h"

void RX_Initialize ( void )
{
    dbgOutputEvent(RX_INIT);
    SYS_INT_SourceEnable(INT_SOURCE_USART_1_RECEIVE);
}

void RX_Tasks ( void )
{
    dbgOutputEvent(RX_TASK_START);
    ByteArray ba;
    int once = 1;
    sensorQueueSendFromRX('g');
    motorQueueSendFromRX(-1, 0);
//    aQueueSendFromRX(55);
    while(1){
        dbgOutputEvent(RX_WHILE);
//        aQueueSendFromRX(55);
        ba = rxQueueRecv();
        switch(once){
            case 0:
                if(ba.c[1] == 'G' && ba.c[2] == 'O'){
                    dbgOutputEvent(RX_GO_RECV);
//                    aQueueSendFromRX(55);
                    sensorQueueSendFromRX('g');
                    motorQueueSendFromRX(-1, 0);
                    once = 1;
                }
                break;
            case 1:
                if(jRead_int(ba.c, "{'MSG_TYPE'", NULL) == 0){ //ACK
                    dbgOutputEvent(RX_GOT_ACK);
                    feedbackQueueSendFromRX(jRead_int(ba.c, "{'MSG_ID'", NULL));
                }else if(jRead_int(ba.c, "{'MSG_TYPE'", NULL) == 1){ //COMMAND
                    dbgOutputEvent(RX_SENDING_COMMAND);
                    if(jRead_int(ba.c, "{'COMMAND'", NULL) < 70){
                        motorQueueSendFromRX(jRead_int(ba.c, "{'COMMAND'", NULL), 0);
                    }else if(jRead_int(ba.c, "{'COMMAND'", NULL) >= 70){
                        motorQueueSendFromRX(jRead_int(ba.c, "{'COMMAND'", NULL), jRead_int(ba.c, "{'VALUE'", NULL));
                    }
                }
                break;
        }
   }
}

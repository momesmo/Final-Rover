#include "motor.h"

void triggerCheck(){
    if(!(PLIB_PORTS_Read(PORTS_ID_0, PORT_CHANNEL_G) & (1 << 6))){
        haltAll(ERRORCODE);
    }
}

void motorControlRIGHT(int ctrl){
    if(ctrl == 1){ // FORWARD
        PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_14);        
    }else if(ctrl == -1){ // BACKWARD
        PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_14);
    }
}

void motorControlLEFT(int ctrl){
    if(ctrl == 1){ // FORWARD
        PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_1);
    }else if(ctrl == -1){ // BACKWARD
        PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_1);
    }
}

void calculateTicks(unsigned int curr_left, unsigned int curr_right, unsigned int prev_left, unsigned int prev_right){
    int diff_left = (curr_left >= prev_left) ? curr_left - prev_left : 
        curr_left - prev_left + 60000;
    int diff_right = (curr_right >= prev_right) ? curr_right - prev_right : 
        curr_right - prev_right + 60000;
    mData.ticksLeft = diff_left * (1000/MOTOR_TIMER_PERIOD) / 6;
    mData.ticksRight = diff_right * (1000/MOTOR_TIMER_PERIOD) / 6;
    
    leftPI.recVel = mData.ticksLeft;
    rightPI.recVel = mData.ticksRight;
}

void setMovement(char dir){
    if(dir == 1){ //FORWARD - 1
        dbgOutputEvent(MOTOR_FORWARD);
        motorControlLEFT(1);
        motorControlRIGHT(1);
        setVel(SET_VEL);
        mData.move = 'F';
        mData.ack = 1;
    }
    if(dir == 2){ //BACKWARD - 2
        dbgOutputEvent(MOTOR_BACK);
        motorControlLEFT(-1);
        motorControlRIGHT(-1);
        setVel(SET_VEL);
        mData.move = 'B';
        mData.ack = 1;
    }
    if(dir == 3){ //LEFT - 3
        dbgOutputEvent(MOTOR_LEFT);
        motorControlLEFT(-1);
        motorControlRIGHT(1);
        setVel(SET_VEL);
        mData.move = 'L';
        mData.ack = 1;
    }
    if(dir == 4){ //RIGHT - 4
        dbgOutputEvent(MOTOR_RIGHT);
        motorControlLEFT(1);
        motorControlRIGHT(-1);
        setVel(SET_VEL);
        mData.move = 'R';
        mData.ack = 1;
    }
    if(dir == 5){ //STOP - 5
        dbgOutputEvent(MOTOR_STOP);
        setVel(0);
        mData.move = 'S';
        clearPID();
        mData.ack = 1;
    }
    if(dir == 6){ //SMALL LEFT - 6
        motorControlLEFT(-1);
        motorControlRIGHT(1);
        setVel(SET_VEL_SMALL);
        mData.move = 'l';
        mData.ack = 1;
    }
    if(dir == 7){ //SMALL RIGHT - 7
        motorControlLEFT(1);
        motorControlRIGHT(-1);
        setVel(SET_VEL_SMALL);
        mData.move = 'r';
        mData.ack = 1;
    }
    if(dir == 10){ //30 velocity - 10
        dbgOutputEvent(SET_ENCODER_30);
        leftPI.setVel = 30;
        rightPI.setVel = 30;
        mData.move = '3';
    }
    if(dir == 11){ //40 velocity - 11
        dbgOutputEvent(SET_ENCODER_40);
        leftPI.setVel = 40;
        rightPI.setVel = 40;
        mData.move = '4';
    }
    if(dir == 12){ //50 velocity - 12
        dbgOutputEvent(SET_ENCODER_50);
        leftPI.setVel = 50;
        rightPI.setVel = 50;
        mData.move = '5';
    }
    if(dir == 13){ //60 velocity - 13
        dbgOutputEvent(SET_ENCODER_60);
        leftPI.setVel = 60;
        rightPI.setVel = 60;
        mData.move = '6';
    }
    if(dir == 14){ //70 velocity - 14
        dbgOutputEvent(SET_ENCODER_70);
        leftPI.setVel = 70;
        rightPI.setVel = 70;
        mData.move = '7';
    }
    if(dir == 15){ //80 velocity - 15
        dbgOutputEvent(SET_ENCODER_80);
        leftPI.setVel = 80;
        rightPI.setVel = 80;
        mData.move = '8';
    }
}

void setVel(int vel){
    leftPI.setVel = vel;
    rightPI.setVel = vel;
}

void setPIDVal(int ctrl_var, int val){
    if(ctrl_var == 70){ // left kd
        leftPI.kd = val;
    }
    if(ctrl_var == 71){ // right kd
        rightPI.kd = val;
    }
    if(ctrl_var == 80){ // left kp
        leftPI.kp = val;
    }
    if(ctrl_var == 81){ // right kp
        rightPI.kp = val;
    }
    if(ctrl_var == 90){ // left ki
        leftPI.ki = val;
    }
    if(ctrl_var == 91){ // right ki
        rightPI.ki = val;
    }
    if(ctrl_var == 99){ // INIT VALUES
        leftPI.kp = 3500;
        leftPI.ki = 110;
        leftPI.kd = 25000;
        rightPI.kp = 3500;
        rightPI.ki = 110;
        rightPI.kd = 25000;
    }
}

void clearPID(){
    leftPI.ms = left;
    leftPI.setVel = 0;
    leftPI.recVel = 0;
    leftPI.err = 0;
    leftPI.lastErr = 0;
    leftPI.iT = 0;
    leftPI.pT = 0;
    leftPI.dT = 0;
    leftPI.kp = LEFT_KP;
    leftPI.ki = LEFT_KI;
    leftPI.kd = LEFT_KD;
    leftPI.pwm = 0;
    memset(itLeft, 0, sizeof itLeft);
    
    rightPI.ms = right;
    rightPI.setVel = 0;
    rightPI.recVel = 0;
    rightPI.err = 0;
    rightPI.lastErr = 0;
    rightPI.iT = 0;
    rightPI.pT = 0;
    rightPI.dT = 0;
    rightPI.kp = RIGHT_KP;
    rightPI.ki = RIGHT_KI;
    rightPI.kd = RIGHT_KD;
    rightPI.pwm = 0;
    memset(itRight, 0, sizeof itRight);
}

void controlPID(MotorPI * mp){
    int d = 0;
    static unsigned int iL = -1;
    static unsigned int iR = -1;
    mp->lastErr = mp->err;
    mp->err = mp->setVel - mp->recVel;
    mp->pT = mp->kp * mp->err;
    int i = mp->ki * mp->err;
    mp->dT = mp->kd * (mp->err - mp->lastErr);
    if(mp->ms == right){
        iR++;
        d = itRight[iR % NUM_INTEGRAL_READINGS];
        itRight[iR % NUM_INTEGRAL_READINGS] = i;
    }else if(mp->ms == left){
        iL++;
        d = itLeft[iL % NUM_INTEGRAL_READINGS];
        itLeft[iL % NUM_INTEGRAL_READINGS] = i;
    }
    mp->iT = mp->iT + i - d;
    mp->pwm += ((mp->pT + mp->iT + mp->dT) / 1000);
    
    if(mp->pwm > 15000){
        mp->pwm = 15000;
    }
    if(mp->pwm < 2000 || mData.move == 'S'){
        if(mp->ms == right){
            DRV_OC0_PulseWidthSet(0);
        }else if(mp->ms == left){
            DRV_OC1_PulseWidthSet(0);
        }
    }else if(mp->ms == right){
        DRV_OC0_PulseWidthSet(mp->pwm);
    }else if(mp->ms == left){
        DRV_OC1_PulseWidthSet(mp->pwm);
    }
}

void MOTOR_Initialize ( void )
{
    dbgOutputEvent(MOTOR_INIT);
    dbgOutputEvent(MOTOR_INIT);
    mData.dirCommand = 0;
    mData.prev_encoderLeft = 0;
    mData.prev_encoderRight = 0;
    mData.move = '0';
    mData.ack = 0;
    
    clearPID();
    
//    aQueueCreate();
    DRV_OC0_Enable();
    DRV_OC1_Enable();
    DRV_TMR0_Start();
    DRV_TMR1_Start();
    DRV_TMR2_Start();
}

void MOTOR_Tasks ( void )
{
    unsigned int curr_left = 0;
    unsigned int curr_right = 0;
    MotorMsg motorRecvMsg;
    int case_begin = 0;
    
    while(1){
        dbgOutputEvent(MOTOR_WHILE);
        motorRecvMsg = motorQueueRecvBlocking();
        switch(case_begin){
            case 0:
                if(motorRecvMsg.source == rx && motorRecvMsg.ctrl == -1){
                    dbgOutputEvent(MOTOR_RX_MSG);
                    createAndStartMotorTimer();
                    case_begin = 1;
                }
                break;
            case 1:
                if(motorRecvMsg.source == encoderISR){
                    dbgOutputEvent(MOTOR_ENCODER_MSG);
                    curr_left = motorRecvMsg.encoderLeft;
                    curr_right = motorRecvMsg.encoderRight;
                    calculateTicks(curr_left, curr_right, mData.prev_encoderLeft, mData.prev_encoderRight);
                    mData.prev_encoderLeft = curr_left;
                    mData.prev_encoderRight = curr_right;
                    controlPID(&leftPI);
                    controlPID(&rightPI);
                }
                if(motorRecvMsg.source == rx){
                    dbgOutputEvent(MOTOR_CONTROL_MSG);
                    if(motorRecvMsg.ctrl < 70){
                        mData.dirCommand = motorRecvMsg.ctrl;
                        setMovement(mData.dirCommand);
                    }else if(motorRecvMsg.ctrl < 100){
                        setPIDVal(motorRecvMsg.ctrl, motorRecvMsg.val);
                    }else if(motorRecvMsg.ctrl >= 100){
                        setVel(motorRecvMsg.val);
                    }
                }
                feedbackQueueSendFromMotor(mData.ack, mData.ticksLeft, mData.ticksRight, mData.move, leftPI.err, leftPI.kp, leftPI.ki, leftPI.kd, rightPI.kp, rightPI.ki, rightPI.kd, leftPI.pwm);
                mData.ack = 0;
                break;
        }
        triggerCheck();
    }
}

/*******************************************************************************
 End of File
 */

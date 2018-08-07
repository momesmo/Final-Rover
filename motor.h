#ifndef _MOTOR_H
#define _MOTOR_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"

#include "queues.h"
#include "debug.h"
#include "timerCB.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END 
    
#define MOTOR_TIMER_PERIOD 100
#define NUM_INTEGRAL_READINGS 50
#define FORWARD_COUNT 400
#define RIGHT_COUNT 550
#define LEFT_COUNT 550
#define BACKWARD_COUNT 400
#define SMALL_LEFT_COUNT 100
#define SMALL_RIGHT_COUNT 100
#define SET_VEL 80
#define SET_VEL_SMALL 20

#define LEFT_KP 3500
#define LEFT_KI 110
#define LEFT_KD 25000
    
#define RIGHT_KP 3500
#define RIGHT_KI 110
#define RIGHT_KD 25000
    
    typedef struct motorData {
        char dirCommand;
        unsigned int prev_encoderLeft;
        unsigned int prev_encoderRight;
        int ticksLeft;
        int ticksRight;
        int ack;
        char move;
    } MotorData;
    
    typedef enum {left, right}MotorSide;
    typedef struct motorPI {
        MotorSide ms;
        int setVel;
        int recVel;
        int err;
        int lastErr;
        int iT;
        int pT;
        int dT;
        int ki;
        int kp;
        int kd;
        int pwm;
    } MotorPI;
    
    MotorData mData;
    MotorPI leftPI;
    MotorPI rightPI;
    int itLeft[NUM_INTEGRAL_READINGS];
    int itRight[NUM_INTEGRAL_READINGS];
    
    void triggerCheck();
    
    void motorControlLEFT(int ctrl);
    void motorControlRIGHT(int ctrl);
    void calculateTicks(unsigned int curr_left, unsigned int curr_right, unsigned int prev_left, unsigned int prev_right);
    
    void setMovement(char dir);
    void setVel(int vel);
    void setPIDVal(int ctrl_var, int val);
    void clearPID();
    void controlPID(MotorPI * mp);
    
    void MOTOR_Initialize ( void );
    void MOTOR_Tasks( void );


#endif /* _MOTOR_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */


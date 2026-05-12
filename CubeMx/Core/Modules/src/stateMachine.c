#include "stateMachine.h"
#include <stdio.h>
#include "spindleMotor.h"

volatile millingMachine_struct millingMachine = {INITIAL,.xpos = 0.0f,.ypos = 0.0f, .zpos = 0.0f};

void millingMachineInit(void){
    spindleMotorInit();
    //! initialisierung der linearmotoren für x,y,z achse
    //! initialisierung der kommunikation mit GUI
}


void checkStateMachine(void){
    //hier wird der aktuelle state abgefragt und ausgeführt
    // switch(millingMachine.currentState){
    //     case initialState:
                
    //         break;
    //     case configZaxisAndSpeed:
 
    //         break;

    //     case idle:

    //         break;

    //     case milling:

    //         break;

    //     case paused:

    //         break;

    //     case failed:

    //         break;
    // }
}


//! die action funktionen evtl. in files auslagern, dass state machine nicht zu groß wird?
void initialAction(void){
}

void configAction(void){
} 

void idleAction(void){
}  

void workingAction(void){
}  

void pausedAction(void){
}  

void emergencyStopAction(void){
}
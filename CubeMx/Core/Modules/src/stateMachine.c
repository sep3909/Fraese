#include "stateMachine.h"
#include <stdio.h>
#include "spindleMotor.h"

millingMachine_struct millingMachine = {initialState,.xpos = 0.0f,.ypos = 0.0f, .zpos = 0.0f};

void millingMachineInit(void){
    spindleMotorInit();
    //! initialisierung der linearmotoren für x,y,z achse
    //! initialisierung der kommunikation mit GUI
}


void checkStateMachine(void){
    //hier wird der aktuelle state abgefragt und ausgeführt
    switch(millingMachine.currentState){
        case initialState:
                
            break;
        case configZaxisAndSpeed:

            break;

        case idle:

            break;

        case milling:

            break;

        case paused:

            break;

        case emergencyStop:

            break;
    }
}


//! die action funktionen evtl. in files auslagern, dass state machine nicht zu groß wird?
void initialStateAction(void){
}

void configZaxisAndSpeedAction(void){
} 

void idleAction(void){
}  

void workingAction(void){
}  

void pausedAction(void){
}  

void emergencyStopAction(void){
}
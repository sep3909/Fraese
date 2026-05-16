#include "stateMachine.h"
#include "stepper.h"

extern bool end_reached;
#define config_speed 5

void toZero_X (){
    while(end_reached == false){
        MoveTo(&motorX, motorX.current_pos -1, config_speed)
    }
    MoveTo(&motorX, motorX.current_pos + 800, config_speed);
    end_reached = false;
    motorX.current_pos = 0;
    millingMachine.state = SET_Y;
}

void toZero_Y (){
    while(end_reached == false){
        MoveTo(&motorY, motorY.current_pos -1, config_speed)
    }
    MoveTo(&motorX, motorX.current_pos + 800, config_speed);
    end_reached = false;
    motorY.current_pos = 0;
    millingMachine.state = SET_Z;
}

void toZero_Z (){
    while(end_reached == false){
        MoveTo(&motorZ, motorZ.current_pos - 1, config_speed)
    }
    MoveTo(&motorX, motorX.current_pos + 800, config_speed);
    end_reached = false;
    motorZ.current_pos = 0;
    millingMachine.state = CONFIG;
}
#include "stateMachine.h"
#include "motion.h"
#include "stdbool.h"
#include <stdint.h>
#include "config.h"

extern bool end_reached;

void toZero_X (){
    while(end_reached == false){
        MoveTo(&motorX, motorX.current_pos +1, stepperConfigSpeed);
    }
    MoveTo(&motorX, motorX.current_pos + mm2steps(offsetToZeroX), stepperConfigSpeed);
    end_reached = false;
    }

void toZero_Y (){
    while(end_reached == false){
        MoveTo(&motorY, motorY.current_pos +1, stepperConfigSpeed);
    }
    MoveTo(&motorY, motorY.current_pos + mm2steps(offsetToZeroY), stepperConfigSpeed);
    end_reached = false;
}

void toZero_Z (){
    while(end_reached == false){
        MoveTo(&motorZ, motorZ.current_pos + 1, stepperConfigSpeed);

    }
    MoveTo(&motorZ, motorZ.current_pos + mm2steps(offsetToZeroZ), stepperConfigSpeed);
    end_reached = false;
}
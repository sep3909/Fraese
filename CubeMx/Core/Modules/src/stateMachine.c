#include <stdint.h>
#include <stdio.h>
#include "stateMachine.h"
#include "motion.h"
#include "stepper.h"
#include "toZero.h"
#include "spindleMotor.h"
#include "data.h"
#include "config.h"
#include "globals.h"
#include "temperature.h"
#include "stm32f4xx_hal.h"
#include <stdlib.h>

volatile millingMachine_t millingMachine = {INITIAL,0,0};
volatile millingMachineStates_Enum stateTransitionFlag[2] = {INITIAL, INITIAL};    //{old state, new state}

void millingMachineInit(void){
    // Initialisiert Stepper, Spindelmotor und Temperaturmessung
    Stepper_Init();
    spindleMotorInit();
    temp_init();
}


void updateStateMachine(void){

    // Flag ist default auf INITAL, da von INITAL nur Übergang auf SETX stattfindet, dort keine Aktion nötig
    stateTransition(stateTransitionFlag[0], stateTransitionFlag[1]);
    
    // Prüfen des states, entsprechende Aktion ausführen
     switch(millingMachine.state){
        case INITIAL:
            initialAction();
        break;
        case SET_X:
            set_x_Action();
        break;
        case SET_Y:
            set_y_Action();
        break;
        case SET_Z:
            set_z_Action();
        break;
        case CONFIG:
            configAction();
        break;
        case TRANSFER:
            transferAction();
        break;
        case READY:
            readyAction();
        break;
        case MILLING:
            millingAction();
        break;
        case DRILLING:
            drillingAction();
        break;
        case FAIL_SAFE:
            FailSafeAction();
        break;
        case PAUSED:
            pausedAction();
        break;
        // case OVERHEATED:
        //     overheatedAction();
        // break;
        case FINISHED:
        break;
    }
}

void initialAction(void){
    // no Action yet
    //todo in state Transistion
    millingMachine.currentShapeIdx =0;
} 

void set_x_Action(void){
    toZero_X();                         //auf endanschlag fahren
    motorX.current_pos = 0;             //Position auf 0 setzen
    millingMachine.state = SET_Y;       //Weiter mit SET_Y
}

void set_y_Action(void){
    toZero_Y();
    motorY.current_pos = 0;
    send_ack();                         //ack an GUI -> config kann starten        
    millingMachine.state = CONFIG;
}

void set_z_Action(void){
    toZero_Z();
    motorZ.current_pos = 0;
    millingMachine.state = SET_X;

}

void configAction(void){
    // spindleMotorStart();
    if(bewegung !=0){
        MoveTo(&motorZ, motorZ.current_pos + mm2steps(bewegung), stepperConfigSpeed);
        bewegung =0;
        send_ack();
    }
} 

void transferAction(void){
    // no Action yet
}

void millingAction(void){
    if(millingMachine.currentShapeIdx < number_of_shapes){
        spindleMotorStart();
        nextShape(millingMachine.currentShapeIdx);
        millingMachine.currentShapeIdx ++;
    }
    else{
        spindleMotorStop();
        send_finished();
        millingMachine.state = FINISHED;
    }
}  

void drillingAction(void){
    if(millingMachine.currentShapeIdx < number_of_shapes){
        spindleMotorStart();
        nextHole(millingMachine.currentShapeIdx);
        millingMachine.currentShapeIdx ++;
    }
    else{
        spindleMotorStop();
        HAL_Delay(10);
        send_finished();
        millingMachine.state = FINISHED;
    }
}

void pausedAction(void){
    // no Action during pause
}  

void FailSafeAction(void){
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
    HAL_Delay(500);
}

void readyAction(void){
    // vorschub in geegnete Form für Stepperansteuerung umrechnen ,ist in mm/min angegeben
    millingMachine.speedForSteppers = SPEED_CODE_MM_PER_S_TO_CODE(Vorschub);
    hBridgeSetDutyCycle(Drehzahl/200);
}

void overheatedAction(void){
    // spindleMotorStop();
}

void nextShape(uint16_t idx){
    // Wenn alle Formen gefräst sind -> keine Aktion mehr möglich
    if(idx >= number_of_shapes){
        return;
    }

    // Tiefe auslesen, für alle Formen relevant
    //todo depth in motion auch auf long?? ist auf double
    long depth = mm2steps(milling_queue[idx].t);

    switch(milling_queue[idx].type){
        case KREIS:{
            // auslesen des Befehls
            uint32_t radius = abs((uint32_t)(mm2steps(milling_queue[idx].geo.kreis.r)));
            long x = mm2steps(milling_queue[idx].geo.kreis.x);
            long y = -mm2steps(milling_queue[idx].geo.kreis.y);
            Motion_Circle(x, y, radius, millingMachine.speedForSteppers, depth);
        }break;

        case RECHTECK:{
            // auslesen des Befehls
            long x1 = mm2steps(milling_queue[idx].geo.rechteck.x1);
            long x2 = mm2steps(milling_queue[idx].geo.rechteck.x2);
            long y1 = -mm2steps(milling_queue[idx].geo.rechteck.y1);
            long y2 = -mm2steps(milling_queue[idx].geo.rechteck.y2);
            //Ausführen des Befehls
            Motion_Rectangle(x1, y1, x2, y2, millingMachine.speedForSteppers, depth);
        }break;

        case DREIECK:{
            // auslesen des Befehls
            long x1 = mm2steps(milling_queue[idx].geo.dreieck.x1);
            long x2 = mm2steps(milling_queue[idx].geo.dreieck.x2);
            long x3 = mm2steps(milling_queue[idx].geo.dreieck.x3);
            long y1 = -mm2steps(milling_queue[idx].geo.dreieck.y1);
            long y2 = -mm2steps(milling_queue[idx].geo.dreieck.y2);
            long y3 = -mm2steps(milling_queue[idx].geo.dreieck.y3);
            //Ausführen des Befehls
            Motion_Triangle(x1, y1, x2, y2, x3, y3, millingMachine.speedForSteppers, depth);
        }break;

        case LINIE:{
            // auslesen des Befehls
            long xStart = mm2steps(milling_queue[idx].geo.linie.x1);
            long xEnd = mm2steps(milling_queue[idx].geo.linie.x2);
            long yStart = -mm2steps(milling_queue[idx].geo.linie.y1);
            long yEnd = -mm2steps(milling_queue[idx].geo.linie.y2);
            //Ausführen des Befehls
            Motion_Line(xStart, yStart, xEnd, yEnd, millingMachine.speedForSteppers, depth);
        }break;

        default:{
        // no Action
        }break;
    }
}

void nextHole(uint16_t idx){
    // Wenn alle Formen gefräst sind -> keine Aktion mehr möglich
    if(idx > number_of_shapes){
        return;
    }

    if(milling_queue[idx].type == PUNKT){
        long depth = mm2steps(milling_queue[idx].t);
        long x = mm2steps(milling_queue[idx].geo.punkt.x1);
        long y = -mm2steps(milling_queue[idx].geo.punkt.y1);
        // Hinfahren zum Bohrloch erfolgt schneller
        MoveTo(&motorX, x, stepperConfigSpeed);
        MoveTo(&motorY, y, stepperConfigSpeed);
        MoveTo(&motorZ, depth, millingMachine.speedForSteppers);
        MoveTo(&motorZ, OFFSET_Z, millingMachine.speedForSteppers);
    }
}

long mm2steps(float mm){
    //! Viertelschritt
    return -mm*100;
}

 float steps2mm(int32_t steps){
    //! Viertelschritt
    return steps/100.0f;
}

void stateTransition(millingMachineStates_Enum oldState, millingMachineStates_Enum newState){

    if(newState == oldState){
        return;
    }

    //*Aktion für sauberen Übergang bei Event e3 (stop von GUI)
    //* MILLING -> INITIAL
    //* DRILLING -> INITIAL
    if((oldState == MILLING || oldState == DRILLING) && newState == INITIAL){
        stateTransitionFlag[0]  = stateTransitionFlag[1];
        // MoveTo(&motorZ, -OFFSET_Z, millingMachine.speedForSteppers);
        spindleMotorStop();
        //todo auf 0,0,0 zurückfahren??
        return;
    }

    //* Aktion für Übergang von CONFIG -> TRANSFER
    //* Ausschalten des Spindelmotors
    if(oldState == CONFIG && newState == TRANSFER){
        stateTransitionFlag[0]  = stateTransitionFlag[1];
        MoveTo(&motorZ, -OFFSET_Z, stepperConfigSpeed);
        // spindleMotorStop();
        return;
    }

    // //* Aktion für Pause -> z achse hochfahren und Spindelmotor stoppen
    // if((oldState == MILLING || oldState == DRILLING) && newState ==READY){
    //     MoveTo(&motorZ, -OFFSET_Z, millingMachine.speedForSteppers);
    //     spindleMotorStop();
    //     stateTransitionFlag[0]  = stateTransitionFlag[1];
    //     return;
    // }

    // //* Aktion für pause -> drilling/milling: spindelMotor starten, ins werkstück einfahren
    // if(oldState == READY && (newState == DRILLING || newState == MILLING)){
    //     spindleMotorStart();
    //     MoveTo(&motorZ, mm2steps(milling_queue[millingMachine.currentShapeIdx].t) , millingMachine.speedForSteppers);
    //     stateTransitionFlag[0]  = stateTransitionFlag[1];
    //     return;
    // }

    //* am Ende immer old state = new state
    stateTransitionFlag[0]  = stateTransitionFlag[1];
};
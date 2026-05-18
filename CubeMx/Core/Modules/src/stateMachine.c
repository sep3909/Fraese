#include "stateMachine.h"
#include <stdint.h>
#include <stdio.h>
#include "spindleMotor.h"
#include "data.h"
#include "globals.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "motion.h"
#include "config.h"
#include "stm32f4xx_hal_pwr.h"
#include "toZero.h"

volatile millingMachine_t millingMachine = {INITIAL,.xpos = 0.0f,.ypos = 0.0f, .zpos = 0.0};

void millingMachineInit(void){
    Stepper_Init();
    spindleMotorInit();
}


void checkStateMachine(void){
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
    }
}


//! die action funktionen evtl. in files auslagern, dass state machine nicht zu groß wird?

void set_x_Action(void){
    toZero_X();                         //auf endanschlag fahren
    motorX.current_pos = 0;             //Position auf 0 setzen
    millingMachine.state = SET_Y;       //Weiter mit SET_Y
}

void set_y_Action(void){
    //TODO: stepper langsam fahren bis interrupt
    //TODO: wieder xx steps zurück auf defined 0
    //TODO: position auf 0 setzen
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET); //! orangene LED an für Test
    HAL_Delay(2000);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
    millingMachine.state = SET_Z;
}

void set_z_Action(void){
    //TODO: stepper langsam fahren bis interrupt
    //TODO: wieder xx steps zurück auf defined 0
    //TODO: position auf 0 setzen
    millingMachine.state = CONFIG;
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET); //! red led
    HAL_Delay(2000);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
    //TODO gui muss länger warten
    send_ack();                                                                         //ack an GUI -> config kann starten
}

void configAction(void){
    if(bewegung !=0){
        MoveTo(&motorZ, calculateSteps(bewegung), millingMachine.speed);
        bewegung =0;
        send_ack();
    }
}

void initialAction(void){
}  

void transferAction(void){
}

void millingAction(void){
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15); //! blaue LED an für
    HAL_Delay(100);
}  

void drillingAction(void){
}

void pausedAction(void){
}  

void FailSafeAction(void){
}

void readyAction(void){
    uint32_t speedForSteppers = SPEED_CODE_MM_PER_S_TO_CODE(Vorschub);
}


uint32_t calculateSteps(float mm){
    //! Viertelschritt
    return mm*100;
}
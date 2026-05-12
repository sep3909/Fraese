#include "stateMachine.h"
#include <stdio.h>
#include "spindleMotor.h"
#include "data.h"
#include "globals.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

volatile millingMachine_t millingMachine = {INITIAL,.xpos = 0.0f,.ypos = 0.0f, .zpos = 0.0};

void millingMachineInit(void){
    spindleMotorInit();
    //! initialisierung der linearmotoren für x,y,z achse
    //! initialisierung der kommunikation mit GUI
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
        case PAUSED:
            pausedAction();
            break;
        case FAIL_SAFE:
            FailSafeAction();
            break;
    }
}


//! die action funktionen evtl. in files auslagern, dass state machine nicht zu groß wird?
void set_x_Action(void){
    //TODO: stepper langsam fahren bis interrupt
    //TODO: wieder xx steps zurück auf defined 0
    //TODO: position auf 0 setzen
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET); //!grüne LED an für Test
    HAL_Delay(2000);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
    millingMachine.state = SET_Y;
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
    // send_ack();                                                                         //ack an GUI -> config kann starten
}

void configAction(void){
    if(bewegung !=0){
        //TODO: z achse bewegen;
        bewegung =0;
        //TODO gui muss länger warten
        // send_ack();
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
}
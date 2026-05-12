#ifndef STATEMACHINE_H
#define STATEMACHINE_H
#include "SpindleMotor.h"
// states for state Machine
typedef enum {
    INITIAL,           //initial -> warten auf e11 (fräsen oder bohren)
    CONFIGURATION,                 //konfiguration der z achse und spindle speed
    IDLE,                   // ready to work, befehle empfangen
    MILL,                //während der fräsens
    DRILL,          //während des bohrens
    PAUSED,                 // fräse wurde über GUI pasuiert
    FAIL_SAFE           
} stateMachineStates;

typedef struct millingMachine_struct{
    stateMachineStates volatile state;   //aktueller state der Fräse
    float xpos;                                 // x position -> Linearmotor
    float ypos;                                 // y position -> Linearmotor
    float zpos;                                 // z position -> Linearmotor
} millingMachine_struct;

void millingMachineInit(void);  // Initialisierung der Fräse, initialisiert auch motoren, usw in main
void checkStateMachine(void); // hier wird der aktuelle state abgefragt und ausgeführt, wird in main in while loop aufgerufen
#endif // STATEMACHINE_H
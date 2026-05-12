#include "SpindleMotor.h"
// states for state Machine
typedef enum {
    initialState,           //initial -> warten auf e11 (fräsen oder bohren)
    configZaxisAndSpeed,    //konfiguration der z achse und spindle speed
    idle,                   // ready to work, befehle empfangen
    milling,                //während der fräsens
    paused,                 // fräse wurde über GUI pasuiert
    emergencyStop           
} stateMachineStates;

typedef struct millingMachine_struct{
    stateMachineStates currentState;    //aktueller state der Fräse
    float xpos;                         // x position -> Linearmotor
    float ypos;                         // y position -> Linearmotor
    float zpos;                         // z position -> Linearmotor
} millingMachine_struct;

extern millingMachine_struct millingMachine; // globale Variable für den Zustand der Fräse, damit in allen files zugreifbar

void millingMachineInit(void);  // Initialisierung der Fräse, initialisiert auch motoren, usw in main
void checkStateMachine(void); // hier wird der aktuelle state abgefragt und ausgeführt, wird in main in while loop aufgerufen
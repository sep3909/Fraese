#ifndef STATEMACHINE_H
#define STATEMACHINE_H
#include "stateMachine.h"
#include <stdint.h>

// states for state Machine
typedef enum {
    INITIAL,                //initial -> warten auf e11 (fräsen oder bohren)
    SET_X,                  // x Achse auf 0 fahren
    SET_Y,                  // y Achse auf 0 fahren
    SET_Z,                  // z Achse auf 0 fahren
    CONFIG,                 // konfiguration der z achse und spindle speed
    TRANSFER,               // Daten werden übertragen
    READY,                  // ready to work, befehle empfangen
    MILLING,                // während der fräsens
    DRILLING,               // während des bohrens
    FAIL_SAFE,              // Betätigung des Endanschlages während MILLING/DRILLING
    OVERHEATED,             // overheated
    FINISHED
} millingMachineStates_Enum;

typedef struct millingMachine_t{
    millingMachineStates_Enum volatile state;           //aktueller state der Fräse
    uint32_t speedForSteppers;                          // fräs oder bohr geschwindigkeit für Steppermotoren
    uint16_t currentShapeIdx;                           
} millingMachine_t;

void millingMachineInit(void);
void checkStateMachine(void);
void initialAction(void);
void set_x_Action(void);
void set_y_Action(void);
void set_z_Action(void);
void configAction(void);
void transferAction(void);
void readyAction(void);
void millingAction(void);
void drillingAction(void);
void pausedAction(void);
void FailSafeAction(void);
void overheatedAction();

long mm2steps(float mm);
float steps2mm(long steps);

void nextShape(uint16_t idx);
void nextHole(uint16_t idx);

void stateTransition(millingMachineStates_Enum oldState, millingMachineStates_Enum newState);
#endif // STATEMACHINE_H
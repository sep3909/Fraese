#include "data.h"
#include "stateMachine.h"

extern volatile millingMachine_t millingMachine;    // globale Variable für den Zustand der Fräse, damit in allen files zugreifbar
extern volatile float bewegung;                              // Bewegung der z Achse während dem Ankratzen
extern volatile float Drehzahl;                              // drehzhal des Spindelmotors
extern volatile float Vorschub;                              // vorschubgeschwindigkeit der fräse

extern volatile Shape_t milling_queue[MAX_SHAPES];           //Fräsbefehle
extern volatile uint16_t number_of_shapes;                   // Anzahl der Fräsbefehle

extern volatile millingMachineStates_Enum stateTransitionFlag[2];
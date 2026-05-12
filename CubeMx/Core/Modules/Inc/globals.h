#include "data.h"
#include "stateMachine.h"

extern volatile millingMachine_t millingMachine;    // globale Variable für den Zustand der Fräse, damit in allen files zugreifbar
extern float bewegung;                              // Bewegung der z Achse während dem Ankratzen
extern float Drehzahl;                              // drehzhal des Spindelmotors
extern float Vorschub;                              // vorschubgeschwindigkeit der fräse

extern Shape_t milling_queue[MAX_SHAPES];           //Fräsbefehle
extern uint16_t number_of_shapes;                   // Anzahl der Fräsbefehle
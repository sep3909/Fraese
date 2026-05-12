#include "data.h"
#include "stateMachine.h"

extern Shape_t milling_queue[MAX_SHAPES];           //Fräsbefehle
extern volatile millingMachine_struct millingMachine; // globale Variable für den Zustand der Fräse, damit in allen files zugreifbar
extern float bewegung;         //noch global, später lokal
extern char Modus;
extern float Drehzahl;      //* Umdrehungen pro Minute
extern float Vorschub;

extern uint16_t number_of_shapes;  
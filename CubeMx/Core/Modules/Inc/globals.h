#include "data.h"
#include "stateMachine.h"

extern volatile millingMachine_t millingMachine;    // globale Variable für den Zustand der Fräse, damit in allen files zugreifbar
extern float bewegung;                              // Bewegung der z Achse während dem Ankratzen
extern float Drehzahl;                              // drehzhal des Spindelmotors
extern float Vorschub;                              // vorschubgeschwindigkeit der fräse

extern Shape_t milling_queue[MAX_SHAPES];           //Fräsbefehle
<<<<<<< HEAD
extern volatile millingMachine_struct millingMachine; // globale Variable für den Zustand der Fräse, damit in allen files zugreifbar
extern float bewegung;         //noch global, später lokal
extern char Modus;
extern float Drehzahl;      //* Umdrehungen pro Minute
extern float Vorschub;

extern uint16_t number_of_shapes;  
=======
extern uint16_t number_of_shapes;                   // Anzahl der Fräsbefehle
>>>>>>> 09baa504af6e5888323966b33c0e5f136545b9cc

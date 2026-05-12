#include "data.h"
#include "stateMachine.h"

extern Shape_t milling_queue[MAX_SHAPES];           //Fräsbefehle
extern volatile DataState_t current_data_state;  
extern volatile millingMachine_struct millingMachine; // globale Variable für den Zustand der Fräse, damit in allen files zugreifbar

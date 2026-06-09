#include "stepper.h"
#include "stm32f407xx.h"
#include "stm32f4xx_hal_gpio.h"
#include "spindleMotor.h"
#include "stateMachine.h"
#include "globals.h"
#include "stdbool.h"
#include "stepper.h"

// -------------------------------------------------------------------------------------
// Interrupthandler für Endanschlagschalter
// wird bei fallender Flanke an PD6 ausgelöst
// MILLING/DRILLING: Stateübergang auf FAIL_SAFE
// SET_X/Y/Z: setzt Flag -> Endanschlag auf Achse erreicht
// -------------------------------------------------------------------------------------

bool end_reached = false;           // Flagvariable für SET_X/Y/Z

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    
    if (GPIO_Pin == endStopSwitch_Pin) { //* Überprüfen, ob die Unterbrechung vom Stoppschalter kommt
        if (millingMachine.state == MILLING || millingMachine.state == DRILLING){
            // //todo send message to GUI
            // //todo hier motor ausschalten sleep pins, dass kein haltemoment
            //* Reihenfolge wichtig:
            //* 1) Stepper-Flags löschen, damit ein laufender MoveTo sofort frei wird
            //* 1) Stepper-Flags löschen, damit ein laufender MoveTo sofort frei wird
            Stepper_StopAll();
            //* 2) Spindel stoppen
            spindleMotorStop();
            //* 3) State auf Not-Stopp -> MoveTo/Internal_Line brechen über den FAIL_SAFE-Check ab
            millingMachine.state = FAIL_SAFE;
            stateTransitionFlag[0] = millingMachine.state;
            stateTransitionFlag[1] = FAIL_SAFE;

        }

        //* Endanschlagsschalter wird während der Konfiguration ausgelöst -> z-Achse auf 0 setzen
        if(millingMachine.state == SET_X || millingMachine.state == SET_Y || millingMachine.state == SET_Z){
            end_reached = true;
        }
    }
}
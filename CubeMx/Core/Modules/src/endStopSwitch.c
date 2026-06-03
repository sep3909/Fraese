#include "main.h"
#include "stm32f407xx.h"
#include "stm32f4xx_hal_gpio.h"
#include "SpindleMotor.h"
#include "stateMachine.h"
#include "globals.h"
#include "stdbool.h"
// wir daufgerufe, wenn endanschlagschalter ausgelöst wird --> nullposition finden oder oder emergency stop

bool end_reached = false;           //! setzt Flag für den Aufruf

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    
    //* Aktion wenn Endanschlagschalter während des Fräsens ausgelöst wird -> Not-Stopp
    if (GPIO_Pin == endStopSwitch_Pin) { // Überprüfen, ob die Unterbrechung vom Stoppschalter kommt
        if (millingMachine.state == MILLING){
            HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13); //* orangene LED toggeln für Test
           //* 1) Stepper-Flags löschen, damit ein laufender MoveTo sofort frei wird
            Stepper_StopAll();
            //* 2) Spindel stoppen
            spindleMotorStop();
            //* 3) State auf Not-Stopp -> MoveTo/Internal_Line brechen über den FAIL_SAFE-Check ab
            millingMachine.state = FAIL_SAFE;
            //! send message to GUI
            //! switch state to emergency stop
        }

        //* Endanschlagsschalter wird während der Konfiguration ausgelöst -> z-Achse auf 0 setzen
        if(millingMachine.state == SET_X || millingMachine.state == SET_Y || millingMachine.state == SET_Z){
            end_reached = true;
        }
    }
}
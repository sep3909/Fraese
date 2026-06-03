#include "main.h"
#include "stm32f407xx.h"
#include "stm32f4xx_hal_gpio.h"
#include "SpindleMotor.h"
#include "stateMachine.h"

#include "stepper.h"
#include "globals.h"
#include "stdbool.h"

// wir daufgerufe, wenn endanschlagschalter ausgelöst wird --> nullposition finden oder oder emergency stop
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    
    if (GPIO_Pin == endStopSwitch_Pin) { // Überprüfen, ob die Unterbrechung vom Stoppschalter kommt
        if (millingMachine.currentState == milling){
            HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13); //* orangene LED toggeln für Test

            //* Reihenfolge wichtig:
            //* 1) Stepper-Flags löschen, damit ein laufender MoveTo sofort frei wird
            Stepper_StopAll();
            //* 2) Spindel stoppen
            spindleMotorStop();
            //* 3) State auf Not-Stopp -> MoveTo/Internal_Line brechen über den FAIL_SAFE-Check ab
            millingMachine.state = FAIL_SAFE;
            //! send message to GUI
        }
    }
}
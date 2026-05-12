#include "main.h"
#include "stm32f407xx.h"
#include "stm32f4xx_hal_gpio.h"
#include "SpindleMotor.h"
#include "stateMachine.h"

// wir daufgerufe, wenn endanschlagschalter ausgelöst wird --> nullposition finden oder oder emergency stop
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    
    if (GPIO_Pin == endStopSwitch_Pin) { // Überprüfen, ob die Unterbrechung vom Stoppschalter kommt
        if (millingMachine.currentState == milling){
            HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13); //* orangene LED toggeln für Test
            //! stop all motors -> x,y,z axis stoppen
            //! send message to GUI
            //! switch state to emergency stop
            spindleMotorStop();
        }
    }
}
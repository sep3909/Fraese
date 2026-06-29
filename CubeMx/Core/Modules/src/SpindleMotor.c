//Modul für Ansteuerung des Spindelmotors
#include "config.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_tim.h"
#include "tim.h"
#include <stdint.h>
#include "spindleMotor.h"

// initiale Zustände: Drehrichtung = Vorwärts, Duty Cycle = 0%, Motor aus
SpindleMotor_Struct spindleMotor = {0, 0, 0, IDLE};

void spindleMotorInit(void)
{
    // Default: Motor aus-> pwm mit duty cycle
    __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, 0);
    // Default Drehrichtung: Vorwärts
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, spindleMotor.direction); // Setze Richtungspin entsprechend der Drehrichtung
    // Timer für PWM starten
    HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_1);
    // MOtor ist initial idle
    spindleMotor.state = IDLE;
}

void hBridgeSetDutyCycle(uint8_t duty)
{
    // maximal 100 prozent möglich
    if (duty > 100) duty = 100;
    spindleMotor.dutyCycle = duty;
    // UMrechnen des Duty Cycles auf Compare Wert für Timer
    spindleMotor.dutyCycleForTim = (htim9.Init.Period + 1) * spindleMotor.dutyCycle / 100 - 1;
    if (spindleMotor.state ==  RUNNING) {
        __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, spindleMotor.dutyCycleForTim); // Setze den Duty Cycle im Timer (PWM)
    }
}

void spindleMotorSetDirection(uint8_t direction)
{
    if (direction == 0) {
        // Vorwärts
        spindleMotor.direction = 0;
    } else {
        // Rückwärts
        spindleMotor.direction = 1;
    }
}


void spindleMotorStart(void){
    if (spindleMotor.state == IDLE){
        // Pin für Richtung setzen
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, spindleMotor.direction); // Setze Richtungspin entsprechend der Drehrichtung
        __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, spindleMotor.dutyCycleForTim);
        TIM9->EGR |= TIM_EGR_UG;
        HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_1);
        spindleMotor.state = STARTING;
        while(spindleMotor.state == STARTING){}     // Solange in Funktion bleiben, bis Motor hochgefahren ist
   }
}   

void spindleMotorStop(void){
    // Motor sofort ausschalten -> Duty Cycle = 0%
    __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, 0);
    TIM9->EGR |= TIM_EGR_UG; // Update-Event
    spindleMotor.state = IDLE;
}

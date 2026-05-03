//Modul für H Brückenansteuererung
#include "main.h"
#include "stm32f4xx_hal_tim.h"
#include "tim.h"
#include <stdint.h>
#include "SpindleMotor.h"

// initiale Zustände: Drehrichtung = Vorwärts, Duty Cycle = 0%, Motor aus
SpindleMotorStruct spindleMotor = {0, 0, 0}; // Initialisierung des Motorzustands

void spindleMotorInit(void)
{
    // Default: Motor aus
    spindleMotorStop();
    // Default Drehrichtung: Vorwärts
    spindleMotorSetDirection(0);
}

void H_Bridge_set_DutyCycle(uint8_t duty)
{
    if (duty > 100) duty = 100; // maximal 100 prozent möglich    
    spindleMotor.dutyCycle = duty; // Speichern des Duty Cycles im Motorzustand
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

void spindleMotorStart(){
    if (spindleMotor.isRunning==0){
        //Berechnen des Compare Values für den Timer basierend auf dem Duty Cycle
        uint32_t compareValue = (htim9.Init.Period + 1) * spindleMotor.dutyCycle / 100 - 1;
        // Setzen des Compare Registers für die PWM-Ausgabe
        __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, compareValue);
        spindleMotor.isRunning = 1;
    }
}

void spindleMotorStop(void)
{
    // Motor sofort ausschalten -> Duty Cycle = 0%
    __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, 0);
    spindleMotor.isRunning = 0;
}

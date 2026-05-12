//Modul für H Brückenansteuererung
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_tim.h"
#include "tim.h"
#include <stdint.h>
#include "SpindleMotor.h"

// initiale Zustände: Drehrichtung = Vorwärts, Duty Cycle = 0%, Motor aus
SpindleMotor_Struct spindleMotor = {0, 0, 0, 0}; 

void spindleMotorInit(void)
{
    // Default: Motor aus-> pwm mit duty cycle
    __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, 0);
    // Default Drehrichtung: Vorwärts
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, spindleMotor.direction); // Setze Richtungspin entsprechend der Drehrichtung
    // Timer für PWM starten
    HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_1);
    spindleMotor.isRunning = 0; // Motor ist initial aus
}

void H_Bridge_set_DutyCycle(uint8_t duty)
{
    if (duty > 100) duty = 100; // maximal 100 prozent möglich    
    spindleMotor.dutyCycle = duty; // Speichern des Duty Cycles im Motorzustand
    spindleMotor.dutyCycleForTim = (htim9.Init.Period + 1) * spindleMotor.dutyCycle / 100 - 1;
    if (spindleMotor.isRunning == 1) {
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
    if (spindleMotor.isRunning==0){
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, spindleMotor.direction); // Setze Richtungspin entsprechend der Drehrichtung
        __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, spindleMotor.dutyCycleForTim);
        spindleMotor.isRunning = 1;
        HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_1);


   }
}

void spindleMotorStop(void){
    // Motor sofort ausschalten -> Duty Cycle = 0%
    __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, 0);
    TIM9->EGR |= TIM_EGR_UG; // Update-Event
    // alternativ über pwm stopp
    //HAL_TIM_PWM_Stop(&htim9, TIM_CHANNEL_1);
    spindleMotor.isRunning = 0;
}

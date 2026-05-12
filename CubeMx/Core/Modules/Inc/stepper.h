#ifndef STEPPER_H //if not defined
#define STEPPER_H

#include "main.h" // Enthält die HAL-Bibliothek und Pin-Defines aus CubeMX

// Struktur zur Verwaltung eines Motors
typedef struct {
    GPIO_TypeDef* step_port; //Beispielsweise Port A
    uint16_t step_pin;       //Pin 0
    GPIO_TypeDef* dir_port;
    uint16_t dir_pin;
    GPIO_TypeDef* limit_port;
    uint16_t limit_pin;

    volatile long current_pos;   // Aktuelle Position in Schritten
    volatile long target_pos;    // Zielposition in Schritten
    volatile int is_moving;      // Status-Flag
    
    // Einfache Geschwindigkeitssteuerung
    volatile uint32_t step_delay;         // Wie viele Timer-Ticks zwischen zwei Schritten warten
    volatile uint32_t timer_count;        // Hilfszähler für den Timer
} StepperMotor;

// Externe Deklaration der Achsen, damit main.c darauf zugreifen kann
extern StepperMotor motorX, motorY, motorZ;

// Funktionen
void Stepper_Init(void);
void Stepper_SetTarget(StepperMotor* motor, long new_target, uint32_t speed);
void Stepper_Update(void); // Diese Funktion wird im Timer-Interrupt aufgerufen
void Stepper_StopAll(void);
void Stepper_Enable(int enable);

#endif
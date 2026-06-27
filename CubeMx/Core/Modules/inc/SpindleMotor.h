#ifndef SPINDLEMOTOR_H
#define SPINDLEMOTOR_H

#include <stdio.h>
#include <stdint.h>



/*-----------------------------------------------------------------------------------------
enum für die Motorzustände
    IDLE: Motor im Stillstand
    RUNNING: Motor läuft
    STARTING: Motor ist im Anlauf
-----------------------------------------------------------------------------------------*/
typedef enum{
    RUNNING,
    IDLE,
    STARTING
}SpindleMotorState_Enum;

/*-----------------------------------------------------------------------------------------
strcut zum speichern der Motorzustände
    direction: Drehrichtung, initial:0 (vorwärts)
    dutyCycle: Dutycycle der PWM, initial: 0
    dutyCycleForTIm: umgerechneter compare Wert für den Timer->PWM
    state: status des Motors, initial:IDLE
-----------------------------------------------------------------------------------------*/
typedef struct SpindleMotor_Struct{
    uint8_t direction;                          // 0: Vorwärts, 1: Rückwärts
    uint8_t dutyCycle;                          // 0-100%
    uint16_t dutyCycleForTim;                   //umgerechnrter Wert für Timer (compare value)
    SpindleMotorState_Enum volatile state;      // 0: Motor aus, 1: Motor an
}SpindleMotor_Struct;

extern SpindleMotor_Struct spindleMotor;

/*-----------------------------------------------------------------------------------------
Init Function für H Brücke, setzt default values, setzt pins
direction, dutycyle, werden in der initialisierung gesetzt
    Vorwärtsrichtung
    Timer Starten
    initial state = IDLE
    dutyCycle =0
-----------------------------------------------------------------------------------------*/
void spindleMotorInit(void);

/*-----------------------------------------------------------------------------------------
Setzt den dutCylce und dutyCYlceForTim
//!Wird sofort angewendet
-----------------------------------------------------------------------------------------*/
void hBridgeSetDutyCycle(uint8_t duty);

/*-----------------------------------------------------------------------------------------
Setzt direction --> direction = 0: Vorwärts, direction ~= 0: Rückwärts
//! Wird erst mit SpindleMotorStart() angewendet
-----------------------------------------------------------------------------------------*/
void spindleMotorSetDirection(uint8_t direction);

/*-----------------------------------------------------------------------------------------
Stoppt Motor
-----------------------------------------------------------------------------------------*/
void spindleMotorStop(void);

/*-----------------------------------------------------------------------------------------
Startet Motor mit aktueller richtung, dutycycle
Wartet für "spindleMotorStartingTime" (config.h) Sekunden, bis weitere Aktion möglich
-----------------------------------------------------------------------------------------*/
void spindleMotorStart(void);

#endif // SPINDLEMOTOR_H
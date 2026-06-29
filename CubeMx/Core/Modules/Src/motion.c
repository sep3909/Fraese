#include "motion.h"
#include "config.h"
#include "stateMachine.h"
#include "stepper.h"
#include "globals.h"  // für millingMachine.state (Not-Stopp-Abbruch)
#include <stdlib.h>  // für abs()
#include "globals.h"
#include <math.h>
#include "spindleMotor.h"
#include "stm32f4xx_hal_tim.h"

// #define OFFSET_Z 250
// #define SPEED_Z 10

void MoveTo(StepperMotor* motor, long target, uint32_t speed) {
    Stepper_SetTarget(motor, target, speed);
    // blockiert bis Ziel erreicht ODER bis Not-Stopp (FAIL_SAFE) ausgelöst wird.
    // Ohne den FAIL_SAFE-Check würde der busy-wait ewig hängen, weil in FAIL_SAFE
    // der TIM2-Interrupt Stepper_Update() nicht mehr aufruft -> is_moving bleibt 1.
    // blockiert bis Ziel erreicht
    // bei OVERHEAT bleibt man in der Schleife, dass nach Abkühlen weitergefräst werden kann
    while ( (motor->is_moving) && 
            (millingMachine.state != FAIL_SAFE)   &&
            // (millingMachine.state != READY)       &&
            // (millingMachine.state != TRANSFER)    &&
            (millingMachine.state != INITIAL)){
                if(((stateTransitionFlag[0] == MILLING)||(stateTransitionFlag[0] == DRILLING)) && (stateTransitionFlag[1] == PAUSED)){
                    stateTransitionFlag[0] = stateTransitionFlag[1];
                    //aus werkstück herausfahren
                    MoveTo(&motorZ, motorZ.current_pos - OFFSET_Z, millingMachine.speedForSteppers);
                    spindleMotorStop();
                }
                else if((stateTransitionFlag[0] == PAUSED && ((stateTransitionFlag[1] == MILLING) || (stateTransitionFlag[1] == DRILLING)))){
                    stateTransitionFlag[0] = stateTransitionFlag[1];
                    spindleMotorStart();
                    // in Werkstück eintauchen
                    MoveTo(&motorZ, motorZ.current_pos + OFFSET_Z, millingMachine.speedForSteppers);
                }
                // nach overheat muss spindlemotor hier wieder gestartet werdens
                // if(startSpindleMotorAfterOverheatFlag){
                //     spindleMotorStart();
                //     startSpindleMotorAfterOverheatFlag = false;
                // }
            }
}


            /*Hilfsfunktion: Nur X/Y Linie ohne Z*/ 
void Internal_Line(long x_end, long y_end, uint32_t speed) {
    long dx = abs(x_end - motorX.current_pos);
    long dy = abs(y_end - motorY.current_pos);
    long sx = (motorX.current_pos < x_end) ? 1 : -1;    //wenn current < x_end dann 1, sonst -1 --> steuert die richtung
    long sy = (motorY.current_pos < y_end) ? 1 : -1; 
    long err = dx - dy;

    while (1) {
        if (millingMachine.state == FAIL_SAFE) break;  // Not-Stopp: Sequenz sofort abbrechen
        if (motorX.current_pos == x_end && motorY.current_pos == y_end) break;
        long e2 = 2 * err;
        if (e2 > -dy) { err -= dy; MoveTo(&motorX, motorX.current_pos + sx, speed); }
        if (e2 < dx)  { err += dx; MoveTo(&motorY, motorY.current_pos + sy, speed); }
    }
}


// Die öffentliche Linienfunktion --> steuert Z
void Motion_Line(long x_start, long y_start, long x_end, long y_end, uint32_t speed, long depth) {
    MoveTo(&motorX, x_start, stepperConfigSpeed);
    MoveTo(&motorY, y_start, stepperConfigSpeed);
    
    // MoveTo(&motorZ, -OFFSET_Z + (long)depth, speed); // Bohrer runterfahren
    MoveTo(&motorZ, depth, speed); // Bohrer runterfahren
    Internal_Line(x_end, y_end, speed);
    MoveTo(&motorZ, OFFSET_Z, speed); //Bohrer hochfahren
}

void Motion_Triangle(long x1, long y1, long x2, long y2, long x3, long y3, uint32_t speed, long depth) {
    // 1. Startposition (erster Punkt) anfahren
    MoveTo(&motorX, x1, stepperConfigSpeed);
    MoveTo(&motorY, y1, stepperConfigSpeed);

    // 2. Bohrer absenken
    MoveTo(&motorZ, depth, speed);

    // 3. Die drei Kanten abfahren
    Internal_Line(x2, y2, speed); // P1 -> P2
    Internal_Line(x3, y3, speed); // P2 -> P3
    Internal_Line(x1, y1, speed); // P3 -> P1 (schließen)

    // 4. Bohrer hochfahren
    MoveTo(&motorZ, OFFSET_Z, speed);
}

void Motion_Rectangle(long x1, long y1, long x2, long y2, uint32_t speed, long depth) {
                  /* Koordinatenangabe in UntenLinks, ObenRechts*/

    // 1. Startposition anfahren
    MoveTo(&motorX, x1, stepperConfigSpeed);
    MoveTo(&motorY, y1, stepperConfigSpeed);
    
    // 2. Eintauchen
    MoveTo(&motorZ, depth, speed);

    // 3. Die vier Seiten abfahren
    Internal_Line(x2, y1, speed); // Rechts
    Internal_Line(x2, y2, speed); // Oben
    Internal_Line(x1, y2, speed); // Links
    Internal_Line(x1, y1, speed); // Zurück zum Start

    // 4. Auftauchen
    MoveTo(&motorZ, OFFSET_Z, speed);
}

void Motion_Circle(long x, long y, uint32_t r, uint32_t speed, long depth) {
    int segments = 3600;

    double step = (2.0 * M_PI) / segments; // 360° in segments Teile
    //0.1° Schritte in Bogenmaß umrechnen: 2*PI/360 = PI/180

    // 1. Startpunkt bei 0° berechnen und anfahren (rechts vom Mittelpunkt)
    long start_x = x + r;
    long start_y = y;
    
    // Hinfahren zur Position ist schneller
    MoveTo(&motorX, start_x, stepperConfigSpeed);
    MoveTo(&motorY, start_y, stepperConfigSpeed);

    // 2. Eintauchen
    MoveTo(&motorZ, depth, speed);

    // 3. Voller Kreis abfahren (von 0° bis 360°)
    for (int i = 1; i <= segments; i++) {
        double angle = i * step;
        long next_x = x + (long)(r * cos(angle));
        long next_y = y + (long)(r * sin(angle));
        
        Internal_Line(next_x, next_y, speed);
    }

    // 4. Bohrer hochfahren
    MoveTo(&motorZ, OFFSET_Z, speed);
}


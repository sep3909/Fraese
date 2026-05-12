#include "motion.h"
#include "stepper.h"
#include <stdlib.h>  // für abs()
#include <math.h>

#define OFFSET_Z -250
#define SPEED_Z 10

void MoveTo(StepperMotor* motor, long target, uint32_t speed) {
    Stepper_SetTarget(motor, target, speed);
    while (motor->is_moving);  // blockiert bis Ziel erreicht
}


            /*Hilfsfunktion: Nur X/Y Linie ohne Z*/ 
void Internal_Line(long x_end, long y_end, uint32_t speed) {
    long dx = abs(x_end - motorX.current_pos);
    long dy = abs(y_end - motorY.current_pos);
    long sx = (motorX.current_pos < x_end) ? 1 : -1;    //wenn current < x_end dann 1, sonst -1 --> steuert die richtung
    long sy = (motorY.current_pos < y_end) ? 1 : -1; 
    long err = dx - dy;

    while (1) {
        if (motorX.current_pos == x_end && motorY.current_pos == y_end) break;
        long e2 = 2 * err;
        if (e2 > -dy) { err -= dy; MoveTo(&motorX, motorX.current_pos + sx, speed); }
        if (e2 < dx)  { err += dx; MoveTo(&motorY, motorY.current_pos + sy, speed); }
    }
}


// Die öffentliche Linienfunktion --> steuert Z
void Motion_Line(long x_start, long y_start, long x_end, long y_end, uint32_t speed, double depth) {
    MoveTo(&motorX, x_start, speed);
    MoveTo(&motorY, y_start, speed);
    
    MoveTo(&motorZ, -OFFSET_Z + (long)depth, speed); // Bohrer runterfahren
    Internal_Line(x_end, y_end, speed);
    MoveTo(&motorZ, OFFSET_Z, speed); //Bohrer hochfahren
}

void Motion_Triangle(long x1, long y1, long x2, long y2, long x3, long y3, uint32_t speed, double depth) {
    // 1. Startposition (erster Punkt) anfahren
    MoveTo(&motorX, x1, speed);
    MoveTo(&motorY, y1, speed);

    // 2. Bohrer absenken
    MoveTo(&motorZ, -OFFSET_Z + (long)depth, speed);

    // 3. Die drei Kanten abfahren
    Internal_Line(x2, y2, speed); // P1 -> P2
    Internal_Line(x3, y3, speed); // P2 -> P3
    Internal_Line(x1, y1, speed); // P3 -> P1 (schließen)

    // 4. Bohrer hochfahren
    MoveTo(&motorZ, OFFSET_Z, speed);
}

void Motion_Rectangle(long x1, long y1, long x2, long y2, uint32_t speed, double depth) {
                  /* Koordinatenangabe in UntenLinks, ObenRechts*/

    // 1. Startposition anfahren
    MoveTo(&motorX, x1, speed);
    MoveTo(&motorY, y1, speed);
    
    // 2. Eintauchen
    MoveTo(&motorZ, -OFFSET_Z + (long)depth, speed);

    // 3. Die vier Seiten abfahren
    Internal_Line(x2, y1, speed); // Rechts
    Internal_Line(x2, y2, speed); // Oben
    Internal_Line(x1, y2, speed); // Links
    Internal_Line(x1, y1, speed); // Zurück zum Start

    // 4. Auftauchen
    MoveTo(&motorZ, OFFSET_Z, speed);
}

void Motion_Circle(long x, long y, uint32_t r, double angle_start_deg, double angle_end_deg, uint32_t speed, double depth) {
    // Umrechnung von Grad in Bogenmaß
    double start_rad = angle_start_deg * (M_PI / 180.0);
    double end_rad   = angle_end_deg   * (M_PI / 180.0);

    // Für Uhrzeigersinn: Wenn end > start, muss die Differenz negativ sein
    if (end_rad > start_rad) {
        end_rad -= 2.0 * M_PI; 
    }

    int segments = r / 2; 
    if (segments < 36) segments = 36; // Mindestanzahl für Glätte (Später anpassen falls nötig)

    double step = (end_rad - start_rad) / segments; //zurückgelegter Winkel in rad pro Segment

    // 1. Startpunkt bei 0° (bzw. angle_start) berechnen und anfahren
    long start_x = x + (long)(r * cos(start_rad));
    long start_y = y + (long)(r * sin(start_rad));
    
    MoveTo(&motorX, start_x, speed);
    MoveTo(&motorY, start_y, speed);

    // 2. Eintauchen
    MoveTo(&motorZ, -OFFSET_Z + (long)depth, speed);

    // 3. Den Bogen abfahren (Winkel wird pro Schritt kleiner -> Uhrzeigersinn)
    for (int i = 1; i <= segments; i++) {
        double current_angle = start_rad + (i * step);
        long next_x = x + (long)(r * cos(current_angle));
        long next_y = y + (long)(r * sin(current_angle));
        
        Internal_Line(next_x, next_y, speed);
    }

    // 4. Bohrer hochfahren
    MoveTo(&motorZ, OFFSET_Z, speed);
}


#include "stepper.h"


/* * 1. Instanzen der Motoren definieren
 * Diese Variablen speichern den aktuellen Zustand (Position, Ziel, Pins)
 * für jede Achse separat ab.
 */
StepperMotor motorX;
StepperMotor motorY;
StepperMotor motorZ;

/**
  * Initialisiert die Motor-Strukturen mit den Pins aus CubeMX
  * Wird einmalig in der main() aufgerufen.
  */
void Stepper_Init(void) {
    // --- X-ACHSE ---
    motorX.step_port    = X_STEP_GPIO_Port; //X_STEP_GPIO_Port --> benennung aus CubeMX
    motorX.step_pin     = X_STEP_Pin;       //defines sind in main.h zu finden
    motorX.dir_port     = X_DIR_GPIO_Port;
    motorX.dir_pin      = X_DIR_Pin;
    //motorX.limit_port   = X_LIMIT_GPIO_Port; //Endanschläge
    //motorX.limit_pin    = X_LIMIT_Pin;
    motorX.current_pos  = 0;
    motorX.target_pos   = 0;
    motorX.is_moving    = 0;
    motorX.timer_count  = 0;

    // --- Y-ACHSE ---
    motorY.step_port    = Y_STEP_GPIO_Port;
    motorY.step_pin     = Y_STEP_Pin;
    motorY.dir_port     = Y_DIR_GPIO_Port;
    motorY.dir_pin      = Y_DIR_Pin;
    //motorY.limit_port   = Y_LIMIT_GPIO_Port;
    //motorY.limit_pin    = Y_LIMIT_Pin;
    motorY.current_pos  = 0;
    motorY.target_pos   = 0;
    motorY.is_moving    = 0;
    motorY.timer_count  = 0;

    // --- Z-ACHSE ---
    motorZ.step_port    = Z_STEP_GPIO_Port;
    motorZ.step_pin     = Z_STEP_Pin;
    motorZ.dir_port     = Z_DIR_GPIO_Port;
    motorZ.dir_pin      = Z_DIR_Pin;
    //motorZ.limit_port   = Z_LIMIT_GPIO_Port;
    //otorZ.limit_pin    = Z_LIMIT_Pin;
    motorZ.current_pos  = 0;
    motorZ.target_pos   = 0;
    motorZ.is_moving    = 0;
    motorZ.timer_count  = 0;
}

/**
  * Schaltet den Strom für alle Motoren ein oder aus
  * Laut Datenblatt ist ENABLE active low
  */
void Stepper_Enable(int enable) {
    if (enable) {
        // RESET setzt den Pin auf 0V -> Treiber ist AKTIVIERT
        HAL_GPIO_WritePin(MOTORS_ENABLE_GPIO_Port, MOTORS_ENABLE_Pin, GPIO_PIN_RESET); //Rücksetzen (Auf 0 setzen)
    } else {
        // SET setzt den Pin auf 3.3V -> Treiber ist DEAKTIVIERT (Stromlos)
        HAL_GPIO_WritePin(MOTORS_ENABLE_GPIO_Port, MOTORS_ENABLE_Pin, GPIO_PIN_SET); //Setzen (Auf 1 setzen)
    }
}

/**
  * Gibt einen neuen Fahrbefehl für eine Achse vor.
  * motor: Zeiger auf die Motor-Struktur (z.B. &motorX)
  * new_target: Zielposition in Schritten
  * speed: Verzögerung in Timer-Ticks (höherer Wert = langsamer)
  */
void Stepper_SetTarget(StepperMotor* motor, long new_target, uint32_t speed) {
    motor->target_pos = new_target;
    motor->step_delay = speed;
    motor->timer_count = 0; // Zähler zurücksetzen für neuen Rhythmus (Falls noch Restwert übrig bleibt)

    // Richtung bestimmen und DIR-Pin setzen
    if (motor->target_pos >= motor->current_pos) {
        HAL_GPIO_WritePin(motor->dir_port, motor->dir_pin, GPIO_PIN_SET); //"positive" Richtung -> DIR-Pin auf "high"
    } else {
        HAL_GPIO_WritePin(motor->dir_port, motor->dir_pin, GPIO_PIN_RESET);
    }

    // Bewegung nur starten, wenn wir nicht schon am Ziel sind
    if (motor->target_pos != motor->current_pos) {
        motor->is_moving = 1;
    }
}


void Stepper_Update(void) {
    // Array mit allen Motoren, um sie in einer Schleife abzuarbeiten
    StepperMotor* motors[] = {&motorX, &motorY, &motorZ};

    for (int i = 0; i < 3; i++) {
        StepperMotor* m = motors[i];

        // Nur wenn der Motor ein Ziel hat, machen wir was
        if (m->is_moving) {
            m->timer_count++;

            // Ist es Zeit für den nächsten Schritt?
            if (m->timer_count >= m->step_delay) {
                m->timer_count = 0;

                /* * Schritt-Impuls (Pulse) erzeugen:
                 * Der Treiber braucht eine steigende Flanke
                 * SET -> RESET erzeugt einen sehr kurzen Peak.
                 */
                HAL_GPIO_WritePin(m->step_port, m->step_pin, GPIO_PIN_SET);
                
                // kleiner delay (No Operation) 
                // 1 __NOP = 1 Taktzyklus
                for (volatile int i = 0; i < 100; i++) __NOP(); 
                
                HAL_GPIO_WritePin(m->step_port, m->step_pin, GPIO_PIN_RESET);

                // Interne Position mitzählen
                if (m->target_pos > m->current_pos) {
                    m->current_pos++;
                } else {
                    m->current_pos--;
                }

                // Prüfen, ob wir das Ziel erreicht haben
                if (m->current_pos == m->target_pos) {
                    m->is_moving = 0;
                }
            }
        }
    }
}

/**
  * Stoppt alle Motoren sofort (setzt moving-Flags zurück)
  */
void Stepper_StopAll(void) {
    motorX.is_moving = 0;
    motorY.is_moving = 0;
    motorZ.is_moving = 0;
}

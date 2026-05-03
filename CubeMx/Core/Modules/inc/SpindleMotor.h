// struct zum abspeichern des motorzustandes
struct SpindleMotorStruct {
    uint8_t direction; // 0: Vorwärts, 1: Rückwärts
    uint8_t dutyCycle; // 0-100%
    uint8_t isRunning; // 0: Motor aus, 1: Motor an
};

// Init Function für H Brücke
// setzt die Drehrichtung auf Vorwärts und den Duty Cycle auf 0%
void spindleMotorInit(void);

// Funktion zum Setzen des Duty Cycles (0-100%)
// bekommt Wert zwischen 0 und 100
//! neuer Duty Cycle wird im Motorzustand gespeichert, aber erst mit spindleMotorStart() aktiv
//! motor muss erst gestoppt werden, damit der neue Duty Cycle aktiv wird
void H_Bridge_set_DutyCycle(uint8_t duty);

// Funktion zum Setzen der Drehrichtung
// direction = 0: Vorwärts, direction ~= 0: Rückwärts
//! neue Drehrichtung wird im Motorzustand gespeichert, aber erst mit spindleMotorStart() aktiv
//! Motor muss erst gestoppt werden, damit die neue Drehrichtung aktiv wird
void spindleMotorSetDirection(uint8_t direction);

// Funktion zum Stoppen des Motors (Duty Cycle = 0%)
void spindleMotorStop(void);

// Funktion zum Starten des Motors mit aktuellem Duty Cycle und Richtung
void spindleMotorStart(void);
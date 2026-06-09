// Hier werden allgemeine defines/configs gemacht

#ifndef CONFIG_H
#define CONFIG_H
//* Stepper motoren/////////////////////////////////////////////////////////////////////////////////////
#define PITCH_MM 8                                              //mm per rotation
#define FULLSTEPS 1                                             //todo ?
#define MICROSTEPS 4                                            // Viertelschritt-Auflösung
#define STEPS_PER_REV 200                                       // steps per Umdrehung
#define MM_PER_STEP (PITCH_MM / (STEPS_PER_REV * MICROSTEPS))   

// #define TIM_UPDATE_HZ ((uint32_t)10000)
#define TIM_UPDATE_HZ ((uint32_t)10000)                         // tim frequenz für update funktion
#define SPEED_CODE_MM_PER_S_TO_CODE(mm_per_s) ((uint32_t)((TIM_UPDATE_HZ * PITCH_MM) / (STEPS_PER_REV * MICROSTEPS * (mm_per_s)))) // speed (mm/s) -> speed for steppers

#define stepperConfigSpeed SPEED_CODE_MM_PER_S_TO_CODE(8)       // speed für die stepper während config
#define offsetToZeroX 40   // offset von X-Endanschlag
#define offsetToZeroY 40   // offset von Y-Endanschlag
#define offsetToZeroZ 40   // offset von Z-Endanschlag

#define OFFSET_Z -250       // offset, den das Werkzeug beim Positionieren über der Werkstückoberfläche hat
#define SPEED_Z 10          // Geschwindigkeit, mit der das Werkzeug in Z-Richtung arbeitet
//*   ///////////////////////////////////////////////////////////////////////////////////////////////////

//! muss ein Vielfaches von 200ms sein
#define spindleMotor_startingTime 2    // Zeit [s], die der Spindelmotor zum Hochlaufen benötigt

#endif
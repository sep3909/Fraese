// Hier werden allgemeine defines/configs gemacht

#ifndef CONFIG_H
#define CONFIG_H
//* Stepper motoren/////////////////////////////////////////////////////////////////////////////////////
#define PITCH_MM 8
#define FULLSTEPS 1
#define MICROSTEPS 4
#define STEPS_PER_REV 200
#define MM_PER_STEP (PITCH_MM / (STEPS_PER_REV * MICROSTEPS))

#define TIM_UPDATE_HZ ((uint32_t)10000)
#define SPEED_CODE_MM_PER_S_TO_CODE(mm_per_s) ((uint32_t)((TIM_UPDATE_HZ * PITCH_MM) / (STEPS_PER_REV * MICROSTEPS * (mm_per_s))))

//*   ///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
#ifndef MOTION_H
#define MOTION_H

#include <stdint.h>
#include "main.h"
#include "stepper.h"

void MoveTo(StepperMotor* motor, long target, uint32_t speed);

void Internal_Line(long x_end, long y_end, uint32_t speed);

void Motion_Line(long x_start, long y_start, long x_end, long y_end, uint32_t speed, double depth);

void Motion_Triangle(long x1, long y1, long x2, long y2, long x3, long y3, uint32_t speed, double depth);

void Motion_Rectangle(long x1, long y1, long x2, long y2, uint32_t speed, double depth);

void Motion_Circle(long x, long y, uint32_t r, double angle_start_deg, double angle_end_deg, uint32_t speed, double depth);

#endif
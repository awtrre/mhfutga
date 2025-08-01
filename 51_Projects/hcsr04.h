// hcsr04.h

#ifndef HCSR04_H
#define HCSR04_H

#include <reg52.h>

/* Define pins for HC-SR04 */
sbit HCSR04_TRIG = P2^0;  /* TRIG pin */
sbit HCSR04_ECHO = P2^1;  /* ECHO pin */

/**
 * @brief  Initialize TRIG output (low) and ECHO input (pull-up).
 */
void HCSR04_Init(void);

/**
 * @brief  Trigger ultrasonic burst and measure echo pulse width.
 * @return Distance to object in centimeters (0..max). 
 *         Returns 0xFFFF if timeout/no echo.
 */
unsigned int HCSR04_GetDistanceCm(void);

/**
 * @brief  Periodic task:
 *         If motor is IDLE, measure distance. If obstacle is within threshold
 *         and not previously triggered, call OneKey_Close() once.
 *         Otherwise, reset trigger flag if obstacle no longer present.
 *         If motor is MOVING, do nothing and reset flag immediately.
 *
 *         Call this every cycle (e.g., ~10ms in main loop).
 */
void HCSR04_Task(void);

#endif /* HCSR04_H */

#ifndef MOTOR_H
#define MOTOR_H

#include <reg52.h>

#define MOTOR_PORT P1  // Define the motor control port (connect your stepper driver here)

/**
 * @brief Delay for approximately ms milliseconds.
 *        Note: Actual delay depends on crystal frequency and compiler optimization.
 * 
 * @param ms Number of milliseconds to delay
 */
void delay_ms(unsigned int ms);

/**
 * @brief Run the stepper motor for a specified number of seconds, then stop.
 * 
 * @param time_s Duration in seconds (e.g. passing 10 makes the motor run for ~10s)
 */
void StepMotor_RunForSeconds(unsigned int time_s);

/**
 * @brief Run the stepper motor in reverse direction for a specified number of seconds, then stop.
 * 
 * @param time_s Duration in seconds (each step ~2ms, total_steps = time_s * 1000 / 2)
 */
void StepMotor_RunReverseForSeconds(unsigned int time_s);

#endif  // MOTOR_H
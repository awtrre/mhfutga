// motor.h

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
 * @brief Run the stepper motor forward for a specified duration in milliseconds.
 *        After the elapsed time, the motor remains energized at the last step.
 *
 * @param time_ms Duration in milliseconds (each step ~2ms, total_steps = time_ms / 2)
 */
void StepMotor_RunMs(unsigned int time_ms);

/**
 * @brief Run the stepper motor in reverse direction for a specified duration in milliseconds.
 *        After the elapsed time, the motor remains energized at the last step.
 *
 * @param time_ms Duration in milliseconds (each step ~2ms, total_steps = time_ms / 2)
 */
void StepMotor_RunReverseMs(unsigned int time_ms);

/**
 * @brief Immediately stop the stepper motor and de-energize all coils.
 */
void StepMotor_Stop(void);

#endif  // MOTOR_H
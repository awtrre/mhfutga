#include "motor.h"

/**
 * @brief Delay for approximately ms milliseconds.
 *        This is a blocking, software-based delay.
 *
 * @param ms Number of milliseconds to delay
 */
void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = ms; i > 0; i--) {
        for (j = 110; j > 0; j--);
    }
}

/**
 * @brief Run the stepper motor for a specified number of seconds.
 *        After the elapsed time, the motor outputs are turned off.
 *
 * @param time_s Duration in seconds (each step ˜2ms, total_steps = time_s * 1000 / 2)
 */
void StepMotor_RunForSeconds(unsigned int time_s) {
    // 8-step half-stepping sequence for a unipolar stepper motor
    unsigned char StepCode[8] = {
        0x01, 0x03, 0x02, 0x06,
        0x04, 0x0C, 0x08, 0x09
    };

    unsigned char step_index = 0;
    // Calculate total number of steps: one step ˜ 2ms
    // total_steps = (time_s * 1000ms) / 2ms_per_step
    unsigned long total_steps = ((unsigned long)time_s * 1000UL) / 2UL;
    unsigned long i;

    for (i = 0; i < total_steps; i++) {
        MOTOR_PORT = StepCode[step_index];  // Output current step pattern

        step_index++;
        if (step_index >= 8) {
            step_index = 0;  // Wrap around after 8 patterns
        }

        delay_ms(2);  // Wait ~2ms between steps
    }

    MOTOR_PORT = 0x00;  // Turn off all coils (stop motor) after finishing
}
/**
 * @brief Run the stepper motor in reverse direction for a specified number of seconds.
 *        After the elapsed time, the motor outputs are turned off.
 *
 * @param time_s Duration in seconds (each step ~2ms, total_steps = time_s * 1000 / 2)
 */
void StepMotor_RunReverseForSeconds(unsigned int time_s) {
    // 8-step half-stepping sequence for a unipolar stepper motor (same array, but iterate backwards)
    unsigned char StepCode[8] = {
        0x01, 0x03, 0x02, 0x06,
        0x04, 0x0C, 0x08, 0x09
    };

    // Start from the last index to move in reverse
    unsigned char step_index = 7;
    // Calculate total number of steps: one step ~2ms
    unsigned long total_steps = ((unsigned long)time_s * 1000UL) / 2UL;
    unsigned long i;

    for (i = 0; i < total_steps; i++) {
        MOTOR_PORT = StepCode[step_index];  // Output current step pattern

        // Decrement index to go in reverse order
        if (step_index == 0) {
            step_index = 7;
        } else {
            step_index--;
        }

        delay_ms(2);  // Wait ~2ms between steps
    }

    MOTOR_PORT = 0x00;  // Turn off all coils (stop motor) after finishing
}

// motor.c

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
 * @brief 8-step half-stepping sequence for a unipolar stepper motor.
 *        This array is shared by both forward and reverse functions.
 */
static const unsigned char StepCode[8] = {
    0x01, 0x03, 0x02, 0x06,
    0x04, 0x0C, 0x08, 0x09
};

/**
 * @brief Keeps track of the current step index across multiple calls,
 *        so that consecutive calls continue smoothly from where they left off.
 */
static unsigned char g_step_index = 0;

/**
 * @brief Run the stepper motor forward for a specified duration in milliseconds.
 *        After the elapsed time, the motor remains energized at the last step.
 *
 * @param time_ms Duration in milliseconds (each step ~2ms, total_steps = time_ms / 2)
 */
void StepMotor_RunMs(unsigned int time_ms) {
    unsigned long total_steps = ((unsigned long)time_ms) / 2UL;
    unsigned long i;

    for (i = 0; i < total_steps; i++) {
        MOTOR_PORT = StepCode[g_step_index];  // Output current step pattern

        g_step_index++;
        if (g_step_index >= 8) {
            g_step_index = 0;  // Wrap around after 8 patterns
        }

        delay_ms(2);  // Wait ~2ms between steps
    }
    // Do NOT turn off MOTOR_PORT here; leave coils energized at last step.
}

/**
 * @brief Run the stepper motor in reverse direction for a specified duration in milliseconds.
 *        After the elapsed time, the motor remains energized at the last step.
 *
 * @param time_ms Duration in milliseconds (each step ~2ms, total_steps = time_ms / 2)
 */
void StepMotor_RunReverseMs(unsigned int time_ms) {
    unsigned long total_steps = ((unsigned long)time_ms) / 2UL;
    unsigned long i;

    for (i = 0; i < total_steps; i++) {
        MOTOR_PORT = StepCode[g_step_index];  // Output current step pattern

        if (g_step_index == 0) {
            g_step_index = 7;
        } else {
            g_step_index--;
        }

        delay_ms(2);  // Wait ~2ms between steps
    }
    // Do NOT turn off MOTOR_PORT here; leave coils energized at last step.
}

/**
 * @brief Immediately stop the stepper motor and de-energize all coils.
 */
void StepMotor_Stop(void) {
    MOTOR_PORT = 0x00;  // Turn off all coils (stop motor)
    // Optionally reset g_step_index to 0 or leave as-is to resume smoothly next time.
}

#include "buttons.h"
#include "motor.h"

// Debounce delay in milliseconds
#define DEBOUNCE_MS        50

// After button release, run extra tail in milliseconds
#define TAIL_DURATION_MS  100

// Approximate time per step in milliseconds (must match motor.c step timing)
#define STEP_TIME_MS        2

// Number of tail steps (= TAIL_DURATION_MS / STEP_TIME_MS)
#define TAIL_STEPS       (TAIL_DURATION_MS / STEP_TIME_MS)

// 8-step half-stepping sequence for a unipolar stepper motor
static const unsigned char StepCode[8] = {
    0x01, 0x03, 0x02, 0x06,
    0x04, 0x0C, 0x08, 0x09
};

// Current forward/reverse step indices, preserved between calls
static unsigned char fwd_index = 0;
static unsigned char rev_index = 7;

/**
 * @brief  Software-based delay for approximately ms milliseconds.
 * @param  ms Number of milliseconds to delay.
 * @note   Blocking; actual timing depends on crystal frequency and compiler optimizations.
 */
static void delay_ms_local(unsigned int ms) {
    unsigned int i, j;
    for (i = ms; i > 0; i--) {
        for (j = 110; j > 0; j--);
    }
}

/**
 * @brief  Check if Button 1 (K1 on P3.0) is pressed (active-low).
 * @return 1 if pressed, 0 otherwise.
 */
static bit is_button1_pressed(void) {
    return (P3 & 0x01) == 0x00;
}

/**
 * @brief  Check if Button 2 (K2 on P3.1) is pressed (active-low).
 * @return 1 if pressed, 0 otherwise.
 */
static bit is_button2_pressed(void) {
    return (P3 & 0x02) == 0x00;
}

void Buttons_Init(void) {
    // Ensure motor port is off at startup
    MOTOR_PORT = 0x00;
}

void Button1_Handle(void) {
    unsigned int tail_count;

    // If K1 is detected as pressed, debounce and confirm
    if (is_button1_pressed()) {
        delay_ms_local(DEBOUNCE_MS);
        if (is_button1_pressed()) {
            // While K1 remains pressed, spin forward continuously
            while (is_button1_pressed()) {
                MOTOR_PORT = StepCode[fwd_index];
                fwd_index++;
                if (fwd_index >= 8) {
                    fwd_index = 0;
                }
                delay_ms_local(STEP_TIME_MS);
            }
            // After release, run tail steps before stopping
            for (tail_count = 0; tail_count < TAIL_STEPS; tail_count++) {
                MOTOR_PORT = StepCode[fwd_index];
                fwd_index++;
                if (fwd_index >= 8) {
                    fwd_index = 0;
                }
                delay_ms_local(STEP_TIME_MS);
            }
            // Turn off coils once tail run completes
            MOTOR_PORT = 0x00;
            delay_ms_local(DEBOUNCE_MS);

            // Optional: synchronize rev_index for seamless reversal
            // if (fwd_index == 0) rev_index = 7;
            // else rev_index = fwd_index - 1;
        }
    }
}

void Button2_Handle(void) {
    unsigned int tail_count;

    // If K2 is detected as pressed, debounce and confirm
    if (is_button2_pressed()) {
        delay_ms_local(DEBOUNCE_MS);
        if (is_button2_pressed()) {
            // While K2 remains pressed, spin in reverse continuously
            while (is_button2_pressed()) {
                MOTOR_PORT = StepCode[rev_index];
                if (rev_index == 0) {
                    rev_index = 7;
                } else {
                    rev_index--;
                }
                delay_ms_local(STEP_TIME_MS);
            }
            // After release, run tail steps before stopping
            for (tail_count = 0; tail_count < TAIL_STEPS; tail_count++) {
                MOTOR_PORT = StepCode[rev_index];
                if (rev_index == 0) {
                    rev_index = 7;
                } else {
                    rev_index--;
                }
                delay_ms_local(STEP_TIME_MS);
            }
            // Turn off coils once tail run completes
            MOTOR_PORT = 0x00;
            delay_ms_local(DEBOUNCE_MS);

            // Optional: synchronize fwd_index for seamless reversal
            // fwd_index = (rev_index >= 7) ? 0 : rev_index + 1;
        }
    }
}
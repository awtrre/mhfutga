#include "onekey.h"
#include "motor.h"      /* Provides StepMotor_RunMs, StepMotor_RunReverseMs, StepMotor_Stop */
#include "eeprom.h"     /* Provides SavePositionToEEPROM */
#include "buzzer.h"     /* NEW: For Buzzer_IsEnabled(), Buzzer_Beep() */
#include <reg52.h>      /* SFR definitions */

/* External global variable holding current position in milliseconds */
extern long g_position_ms;

/* Internal flag indicating whether the motor is running (0 = idle, 1 = running) */
static bit g_motor_running = 0;

/* Time slice for segmented motion in milliseconds */
#define SLICE_MS 100U  /* Each segment runs for 100 ms; adjust as needed */

/* Macros to check if K1 (P3.0) or K2 (P3.1) are pressed (active-low) */
#define IS_K1_PRESSED() ((P3 & 0x01) == 0)
#define IS_K2_PRESSED() ((P3 & 0x02) == 0)

/**
 * @brief  Immediately stop the motor and save current position to EEPROM.
 *         If the motor is running, it calls StepMotor_Stop(), writes
 *         g_position_ms into EEPROM, and clears the running flag.
 */
void OneKey_StopAndSave(void)
{
    if (g_motor_running)
    {
        StepMotor_Stop();            /* Stop the motor immediately */
        SavePositionToEEPROM();      /* Save current position to EEPROM */
        g_motor_running = 0;         /* Mark motor as idle */
    }
}

/**
 * @brief  Perform a one-key "open" action:
 *         - If motor is already running, stop and save, then return.
 *         - If position <= 0 (already fully closed), do nothing.
 *         - Otherwise, run the motor in reverse (toward closed) in segments:
 *             * Each segment runs for SLICE_MS (or the remaining time).
 *             * After each segment, update g_position_ms and check K1/K2.
 *             * If K1 or K2 is pressed, stop motor, save position, and exit.
 *         - If segments finish normally, set position to 0, stop motor,
 *           save to EEPROM, and clear running flag.
 */
void OneKey_Open(void)
{
    unsigned long remaining_time;
    unsigned int slice;

    /* If motor is running, interrupt and save */
    if (g_motor_running)
    {
        OneKey_StopAndSave();
        return;
    }

    /* If already fully closed, no action needed */
    if (g_position_ms <= 0)
    {
        return;
    }

    /* Calculate time needed to reach position 0 */
    remaining_time = (unsigned long)g_position_ms;

    g_motor_running = 1;  /* Mark motor as running */

    /* Loop in segments until target or interruption */
    while (remaining_time > 0)
    {
        /* Determine segment duration */
        slice = (remaining_time >= SLICE_MS) ? SLICE_MS : (unsigned int)remaining_time;

        /* Run motor in reverse for this segment */
        StepMotor_RunReverseMs(slice);

        /* Update current position */
        g_position_ms -= slice;
        remaining_time -= slice;

        /* Check if K1 or K2 is pressed; if so, interrupt */
        if (IS_K1_PRESSED() || IS_K2_PRESSED())
        {
            StepMotor_Stop();           /* Stop motor immediately */
            SavePositionToEEPROM();     /* Save current position */
            g_motor_running = 0;        /* Mark motor as idle */
            return;                     /* Exit the function */
        }
    }

    /* If loop completes normally, we've reached position 0 */
    g_position_ms = 0;
    StepMotor_Stop();             /* Ensure motor is stopped */
    SavePositionToEEPROM();       /* Save final position */
    g_motor_running = 0;          /* Mark motor as idle */
}

/**
 * @brief  Perform a one-key "close" action:
 *         - If motor is already running, stop and save, then return.
 *         - If position >= TOTAL_TIME_MS (fully open), do nothing.
 *         - Otherwise, run the motor forward (toward open) in segments:
 *             * Each segment runs for SLICE_MS (or the remaining time).
 *             * After each segment, update g_position_ms and check K1/K2.
 *             * If buzzer is enabled, call Buzzer_Beep() here (every 100 ms).
 *             * If K1 or K2 is pressed, stop motor, save position, and exit.
 *         - If segments finish normally, set position to TOTAL_TIME_MS, stop motor,
 *           save to EEPROM, and clear running flag.
 */
void OneKey_Close(void)
{
    unsigned long remaining_time;
    unsigned int slice;

    /* If motor is running, interrupt and save */
    if (g_motor_running)
    {
        OneKey_StopAndSave();
        return;
    }

    /* If already fully open, no action needed */
    if (g_position_ms >= TOTAL_TIME_MS)
    {
        return;
    }

    /* Calculate time needed to reach fully open */
    remaining_time = TOTAL_TIME_MS - g_position_ms;

    g_motor_running = 1;  /* Mark motor as running */

    /* Loop in segments until target or interruption */
    while (remaining_time > 0)
    {
        /* Determine segment duration */
        slice = (remaining_time >= SLICE_MS) ? SLICE_MS : (unsigned int)remaining_time;

        /* Run motor forward for this segment */
        StepMotor_RunMs(slice);

        /* Update current position */
        g_position_ms += slice;
        remaining_time -= slice;

        /* If buzzer is enabled (auto-close), produce one beep per 100 ms */
        if (Buzzer_IsEnabled())
        {
            Buzzer_Beep();
        }

        /* Check if K1 or K2 is pressed; if so, interrupt */
        if (IS_K1_PRESSED() || IS_K2_PRESSED())
        {
            StepMotor_Stop();           /* Stop motor immediately */
            SavePositionToEEPROM();     /* Save current position */
            g_motor_running = 0;        /* Mark motor as idle */
            return;                     /* Exit the function */
        }
    }

    /* If loop completes normally, we've reached fully open */
    g_position_ms = TOTAL_TIME_MS;
    StepMotor_Stop();             /* Ensure motor is stopped */
    SavePositionToEEPROM();       /* Save final position */
    g_motor_running = 0;          /* Mark motor as idle */
}

/**
 * @brief  Periodic task placeholder.
 *         In this blocking implementation, OneKey_Open/Close handle their own loops,
 *         so no periodic action is needed here.
 */
void OneKey_Task(void)
{
    /* No periodic slices needed in blocking design */
}

/**
 * @brief  Return 1 if motor is currently moving, 0 otherwise.
 */
unsigned char OneKey_IsMoving(void)
{
    return g_motor_running ? 1 : 0;
}

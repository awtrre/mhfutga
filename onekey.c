// onekey.c

#include "onekey.h"
#include "motor.h"      /* Provides StepMotor_RunMs, StepMotor_RunReverseMs, StepMotor_Stop */
#include "eeprom.h"     /* Provides SavePositionToEEPROM */
#include <reg52.h>      /* SFR definitions */

/* External global variable holding current position in ms */
extern long g_position_ms;

/* Internal flag to indicate if motor is running (0 = idle, 1 = running) */
static bit g_motor_running = 0;

/**
 * @brief  Immediately stop motor and save current position to EEPROM.
 *         Note: In this simplified implementation, we cannot calculate
 *         the new g_position_ms if it's stopped mid-run, so we leave it unchanged.
 */
void OneKey_StopAndSave(void)
{
    /* If motor is currently running, stop it */
    if (g_motor_running)
    {
        StepMotor_Stop();
        /* Do NOT change g_position_ms here, because we don't know elapsed time */
        SavePositionToEEPROM();
        g_motor_running = 0;
    }
}

/**
 * @brief  Initiate a one-key open action:
 *         - If motor currently running, stop and save.
 *         - Else if not fully open, run forward continuously until fully open.
 */
void OneKey_Open(void)
{
	  unsigned long remaining_time;

    /* If motor already running, stop + save, then return */
    if (g_motor_running)
    {
        OneKey_StopAndSave();
        return;
    }

    /* If already fully closed or below, do nothing */
    if (g_position_ms <= 0)
    {
        return;
    }

    /* Calculate how much time is needed to reach 0 */
    remaining_time = (unsigned long)g_position_ms;  /* time to run reverse to zero */

    /* Run reverse for remaining_time (blocking call) */
    g_motor_running = 1;
    StepMotor_RunReverseMs((unsigned int)remaining_time);

    /* After running to fully closed, update position and save */
    g_position_ms = 0;
    StepMotor_Stop();
    SavePositionToEEPROM();
    g_motor_running = 0;

}

/**
 * @brief  Initiate a one-key close action:
 *         - If motor currently running, stop and save.
 *         - Else if not fully closed, run reverse continuously until fully closed.
 */
void OneKey_Close(void)
{
   unsigned long remaining_time;

    /* If motor already running, stop + save, then return */
    if (g_motor_running)
    {
        OneKey_StopAndSave();
        return;
    }

    /* If already fully open or beyond, do nothing */
    if (g_position_ms >= TOTAL_TIME_MS)
    {
        return;
    }

    /* Calculate how much time is needed to reach TOTAL_TIME_MS */
    remaining_time = TOTAL_TIME_MS - g_position_ms;

    /* Run forward for remaining_time (blocking call) */
    g_motor_running = 1;
    StepMotor_RunMs((unsigned int)remaining_time);

    /* After running to fully open, update position and save */
    g_position_ms = TOTAL_TIME_MS;
    StepMotor_Stop();
    SavePositionToEEPROM();
    g_motor_running = 0;
}

/**
 * @brief  Empty periodic task placeholder. In this blocking design,
 *         OneKey_Open/Close do all the work, so OneKey_Task() can be empty.
 */
void OneKey_Task(void)
{
    /* No periodic slices needed in blocking implementation */
}

/**
 * @brief  Return non-zero if motor is currently in motion.
 */
unsigned char OneKey_IsMoving(void)
{
    return g_motor_running ? 1 : 0;
}

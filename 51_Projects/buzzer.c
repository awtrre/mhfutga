#include "buzzer.h"
#include <reg52.h>

/* 
 * Global flag indicating whether automatic beeping is allowed.
 *  - 0: disable automatic beeping (manual mode or idle).
 *  - 1: enable automatic beeping during auto-close.
 */
bit g_buzzer_enabled = 0;

/**
 * @brief  Timer0 interrupt service routine.
 *         When Timer0 overflows (~50ms), this ISR turns off the buzzer
 *         and stops Timer0 to prevent further interrupts.
 */
void Timer0_ISR(void) interrupt 1
{
    /* Turn off the buzzer pin */
    BUZZER = 0;

    /* Stop Timer0 and disable its interrupt */
    TR0 = 0;   /* Stop Timer0 */
    ET0 = 0;   /* Disable Timer0 interrupt */

    /* TF0 (overflow flag) is automatically cleared on ISR entry */
}

/**
 * @brief  Initialize buzzer pin and Timer0 settings.
 *         - Configure P2.5 as GPIO output, initial low.
 *         - Configure Timer0 in mode1 (16-bit), interrupt disabled initially.
 */
void Buzzer_Init(void)
{
    /* Ensure buzzer is off */
    BUZZER = 0;

    /* Configure Timer0 to Mode1 (16-bit timer). Clear previous mode bits first */
    TMOD &= 0xF0;  /* Clear lower 4 bits (Timer0 mode bits) */
    TMOD |= 0x01;  /* Set Timer0 to mode1 (16-bit timer) */

    /* Ensure Timer0 is stopped and interrupt is disabled */
    TR0 = 0;
    ET0 = 0;
    TF0 = 0;
}

/**
 * @brief  Enable automatic beeping during auto-close operation.
 */
void Buzzer_Enable(void)
{
    g_buzzer_enabled = 1;
}

/**
 * @brief  Disable automatic beeping and ensure buzzer pin is off.
 */
void Buzzer_Disable(void)
{
    g_buzzer_enabled = 0;

    /* Turn off buzzer immediately if it was on */
    BUZZER = 0;

    /* Stop Timer0 and disable its interrupt in case it was still running */
    TR0 = 0;
    ET0 = 0;
    TF0 = 0;
}

/**
 * @brief  Produce one short beep (~50ms) via Timer0 interrupt (non-blocking).
 *         If buzzer is enabled, this function will:
 *           1. Set P2.5 = 1 (turn buzzer on).
 *           2. Load Timer0 initial value for ~50ms countdown.
 *           3. Enable Timer0 and its interrupt.
 *           4. Return immediately (no CPU busy-wait).
 */
void Buzzer_Beep(void)
{
    unsigned int t0_init;

    /* Only beep when automatic mode is enabled */
    if (!g_buzzer_enabled)
    {
        return;
    }

    /* Turn buzzer pin on */
    BUZZER = 1;

    /*
     * Calculate Timer0 initial value for ~50ms delay:
     * Assuming 12MHz crystal, 1 machine cycle = 1µs.
     * To achieve 50ms delay, need 50000 timer ticks.
     * Timer0 is 16-bit, total count = 65536. So initial value = 65536 - 50000 = 15536 (0x3CB0).
     */
    t0_init = 0x3CB0;
    TH0 = (unsigned char)(t0_init >> 8);
    TL0 = (unsigned char)(t0_init & 0xFF);

    /* Clear overflow flag just in case */
    TF0 = 0;

    /* Enable Timer0 interrupt and start Timer0 */
    ET0 = 1;
    TR0 = 1;
}

/**
 * @brief  Return non-zero if buzzer beeping is currently enabled.
 */
bit Buzzer_IsEnabled(void)
{
    return g_buzzer_enabled;
}

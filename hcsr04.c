// hcsr04.c

#include "hcsr04.h"
#include "onekey.h"      /* For OneKey_Close() and OneKey_IsMoving() */
#include <intrins.h>     /* for _nop_() */

/*
 * We will use Timer0 in mode 1 (16-bit) to measure echo pulse width.
 * Assume crystal frequency is 12MHz, so 1 timer tick ˜ 1µs.
 */

#define OBSTACLE_THRESHOLD_CM   20    /* Obstacle threshold in cm */

/* Internal flag: whether we have already triggered a close action */
static bit obstacle_triggered = 0;

/**
 * @brief  Microsecond-level approximate delay.
 *         Calibrated for 12MHz crystal and Keil C51 optimization level 2.
 *
 * @param us  Number of microseconds to delay.
 */
static void delay_us(unsigned int us)
{
    unsigned int i;
    /* Approximately 1µs per loop iteration at 12MHz, OPTIMIZE=2 */
    for (i = 0; i < us; i++)
    {
        _nop_();
        _nop_();
    }
}

/**
 * @brief  Initialize TRIG as output low; ECHO as input with pull-up.
 */
void HCSR04_Init(void)
{
    HCSR04_TRIG = 0;      /* Ensure TRIG is low */
    P2 |= 0x02;           /* Pull-up P2.1 (ECHO) if needed */
}

/**
 * @brief  Trigger an ultrasonic burst and measure echo pulse width.
 * @return Distance in cm, or 0xFFFF if timeout/no echo.
 */
unsigned int HCSR04_GetDistanceCm(void)
{
    unsigned int count;
    unsigned int dist_cm;
    unsigned int timeout;

    /* 1) Send 10µs TRIG pulse */
    HCSR04_TRIG = 0;
    delay_us(2);
    HCSR04_TRIG = 1;
    delay_us(10);
    HCSR04_TRIG = 0;

    /* 2) Wait for ECHO to go high (start). Timeout ~30ms */
    timeout = 30000;
    while (!HCSR04_ECHO)
    {
        if (--timeout == 0) 
        {
            return 0xFFFF;  /* Timeout: no echo detected */
        }
    }

    /* 3) Start Timer0 (mode1) to count µs */
    TMOD &= 0xF0;
    TMOD |= 0x01;   /* Timer0 in mode 1 (16-bit) */
    TH0 = 0;
    TL0 = 0;
    TR0 = 1;

    /* 4) Wait for ECHO to go low or timeout ~30ms */
    timeout = 30000;
    while (HCSR04_ECHO)
    {
        if (--timeout == 0) 
        {
            TR0 = 0;  /* Stop timer */
            return 0xFFFF;
        }
    }

    /* 5) Stop Timer0, read count */
    TR0 = 0;
    count = (TH0 << 8) | TL0; /* Timer value in ticks (~1µs/tick) */

    /* 6) Convert to cm: sound speed round-trip ~58µs per cm */
    dist_cm = count / 58;
    return dist_cm;
}

/**
 * @brief  Periodic task: if motor is moving, skip ultrasonic entirely.
 *         Only when motor is idle, perform distance measurement:
 *         - If distance <= threshold and not yet triggered, call OneKey_Close() once.
 *         - If distance > threshold or timeout, reset trigger flag.
 *
 *         Call this every cycle (e.g., ~10ms in main loop).
 */
void HCSR04_Task(void)
{
    unsigned int distance;

    /* 1) If motor is in motion, skip HC-SR04 ops and reset flag */
    if (OneKey_IsMoving())
    {
        /* Clear trigger flag so next time motor is idle,
         * a new obstacle-trigger can occur if needed */
        obstacle_triggered = 0;
        return;
    }

    /* 2) Motor is idle: perform distance measurement */
    distance = HCSR04_GetDistanceCm();

    /* 3) If valid distance and <= threshold AND not yet triggered */
    if (distance != 0xFFFF && distance <= OBSTACLE_THRESHOLD_CM)
    {
        if (!obstacle_triggered)
        {
            /* Trigger one-key close exactly once */
            OneKey_Close();
            obstacle_triggered = 1;
        }
    }
    else
    {
        /* If no obstacle or distance > threshold, reset flag */
        obstacle_triggered = 0;
    }
}

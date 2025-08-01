// hcsr04.c

#include "hcsr04.h"
#include "onekey.h"      /* For OneKey_Close() and OneKey_IsMoving() */
#include <intrins.h>     /* for _nop_() */
#include <reg52.h>       /* SFR definitions */
#include "buzzer.h" 		/* NEW: For Buzzer_Enable()/Buzzer_Disable() */

extern long g_position_ms;  /* Declare external global from main.c */

/*
 * We will use Timer0 in mode 1 (16-bit) to measure echo pulse width.
 * Assume crystal frequency is 12MHz, so 1 timer tick ~1µs.
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
 * @brief  Periodic task with debouncing & down-sample:
 *         – Measure distance only once every 100 ms (skip 9 calls).
 *         – If motor is MOVING, skip and reset flags.
 *         – If IDLE and distance = threshold for 3 consecutive readings, trigger OneKey_Close() once.
 *         – If distance > threshold for 3 consecutive readings, reset trigger flag.
 *
 *         Call this every cycle (e.g., ~10ms in main loop).
 */
void HCSR04_Task(void)
{
    static unsigned char skip_cnt  = 0;  /* Counter to implement 100ms down-sampling (0..9) */
    static unsigned char hit_cnt   = 0;  /* Consecutive readings = threshold */
    static unsigned char miss_cnt  = 0;  /* Consecutive readings > threshold or timeout */
    unsigned int distance;
	  /* ==== 0. If motor already at max position, stop sensor task immediately ==== */
    if (g_position_ms >= TOTAL_TIME_MS)
    {
        HCSR04_TRIG = 0;      /* Ensure TRIG stays low so sensor is disabled */
        obstacle_triggered = 0;/* Reset trigger flag so that after?????????? */
        return;
    }

    /* ---- 1. Down-sample: only measure every 10th call (~100ms) ---- */
    if (++skip_cnt < 10)
    {
        return;  /* Skip this sampling round */
    }
    skip_cnt = 0;  /* Reset counter every 100ms */

    /* ---- 2. If motor is moving, skip measurement and reset flags ---- */
    if (OneKey_IsMoving())
    {
        hit_cnt  = 0;
        miss_cnt = 0;
        obstacle_triggered = 0;
        return;
    }

    /* ---- 3. Perform one distance measurement ---- */
    distance = HCSR04_GetDistanceCm();

    /* ---- 4. Update hit/miss counters ---- */
    if (distance != 0xFFFF && distance <= OBSTACLE_THRESHOLD_CM)
    {
        /* Detected valid reading within threshold */
        if (hit_cnt < 3) hit_cnt++;
        miss_cnt = 0;
    }
    else
    {
        /* No obstacle or timeout */
        if (miss_cnt < 3) miss_cnt++;
        hit_cnt = 0;
    }

    /* ---- 5. Decision: trigger on 3 consecutive hits; reset on 3 consecutive misses ---- */
    if (hit_cnt >= 3 && !obstacle_triggered)
    {
			/* Enable buzzer before calling auto-close */
        Buzzer_Enable();
        OneKey_Close();          /* Initiate one-key close */
				Buzzer_Disable();
        obstacle_triggered = 1;  /* Ensure only triggered once */
    }
    if (miss_cnt >= 3)
    {
        obstacle_triggered = 0;  /* Ready for next trigger */
    }
}
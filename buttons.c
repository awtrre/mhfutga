// buttons.c

#include "buttons.h"
#include "motor.h"      /* Provides delay_ms(), StepMotor_RunMs, StepMotor_RunReverseMs, StepMotor_Stop */
#include "eeprom.h"     /* Provides SavePositionToEEPROM */
#include "onekey.h"     /* Provides OneKey_Open, OneKey_Close, OneKey_StopAndSave */
#include <reg52.h>      /* SFR definitions */

/* Debounce time in ms */
#define DEBOUNCE_MS       50U

/* Manual rotation slice in ms for Button1 and Button2 */
#define MANUAL_STEP_MS   700U

/* Internal helpers: active-low checks for buttons on P3 */
static bit is_btn1(void)    { return (P3 & 0x01) == 0; }  /* P3.0 */
static bit is_btn2(void)    { return (P3 & 0x02) == 0; }  /* P3.1 */
static bit is_btn3(void)    { return (P3 & 0x04) == 0; }  /* P3.2 */
static bit is_btn4(void)    { return (P3 & 0x08) == 0; }  /* P3.3 */

/**
 * @brief Configure P3.0..P3.3 as inputs with internal pull-ups.
 */
void Buttons_Init(void)
{
    P3 = 0xFF;  /* Set P3 pins to high (inputs with pull-ups) */
}

/**
 * @brief Manual forward button handler (P3.0).
 *        While held and not at max, run forward 700ms slices.
 */
void Button1_Handle(void)
{
    unsigned long slice;

    if (is_btn1()) {
        delay_ms(DEBOUNCE_MS);
        if (is_btn1()) {
            /* If at or beyond fully open, do nothing */
            if (g_position_ms >= TOTAL_TIME_MS) {
                StepMotor_Stop();
                delay_ms(DEBOUNCE_MS);
                while (is_btn1());
                return;
            }
            /* While pressed and not at max, run forward in slices */
            while (is_btn1() && g_position_ms < TOTAL_TIME_MS) {
                slice = (TOTAL_TIME_MS - g_position_ms >= MANUAL_STEP_MS)
                            ? MANUAL_STEP_MS
                            : (unsigned long)(TOTAL_TIME_MS - g_position_ms);
                StepMotor_RunMs((unsigned int)slice);
                g_position_ms += slice;
            }
            StepMotor_Stop();
            SavePositionToEEPROM();
            delay_ms(DEBOUNCE_MS);
        }
    }
}

/**
 * @brief Manual reverse button handler (P3.1).
 *        While held and not at zero, run reverse 700ms slices.
 */
void Button2_Handle(void)
{
    unsigned long slice;

    if (is_btn2()) {
        delay_ms(DEBOUNCE_MS);
        if (is_btn2()) {
            /* If already at zero, do nothing */
            if (g_position_ms == 0) {
                StepMotor_Stop();
                delay_ms(DEBOUNCE_MS);
                while (is_btn2());
                return;
            }
            /* While pressed and not at zero, run reverse in slices */
            while (is_btn2() && g_position_ms > 0) {
                slice = (g_position_ms >= MANUAL_STEP_MS)
                            ? MANUAL_STEP_MS
                            : (unsigned long)g_position_ms;
                StepMotor_RunReverseMs((unsigned int)slice);
                g_position_ms -= slice;
            }
            StepMotor_Stop();
            SavePositionToEEPROM();
            delay_ms(DEBOUNCE_MS);
        }
    }
}

/**
 * @brief One-key OPEN button handler (P3.2).
 *        If idle, call OneKey_Open(); if running, call OneKey_StopAndSave().
 */
void Button3_Handle(void)
{
    if (is_btn3()) {
        delay_ms(DEBOUNCE_MS);
        if (is_btn3()) {
            OneKey_Open();
            while (is_btn3());  /* Wait for release */
        }
    }
}

/**
 * @brief One-key CLOSE button handler (P3.3).
 *        If idle, call OneKey_Close(); if running, call OneKey_StopAndSave().
 */
void Button4_Handle(void)
{
    if (is_btn4()) {
        delay_ms(DEBOUNCE_MS);
        if (is_btn4()) {
            OneKey_Close();
            while (is_btn4());  /* Wait for release */
        }
    }
}

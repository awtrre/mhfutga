// buttons.h

#ifndef BUTTONS_H
#define BUTTONS_H

#include <reg52.h>

/* External global for motor position in ms */
extern long g_position_ms;

/**
 * @brief Configure P3.0..P3.3 as inputs with pull-ups for 4 buttons.
 */
void Buttons_Init(void);

/**
 * @brief Manual forward (Button1 on P3.0): 
 *        While held, run motor forward in 700ms slices until fully open.
 */
void Button1_Handle(void);

/**
 * @brief Manual reverse (Button2 on P3.1):
 *        While held, run motor reverse in 700ms slices until fully closed.
 */
void Button2_Handle(void);

/**
 * @brief One-key OPEN (Button3 on P3.2): 
 *        If idle, call OneKey_Open(); if running, call OneKey_StopAndSave().
 */
void Button3_Handle(void);

/**
 * @brief One-key CLOSE (Button4 on P3.3): 
 *        If idle, call OneKey_Close(); if running, call OneKey_StopAndSave().
 */
void Button4_Handle(void);

#endif /* BUTTONS_H */

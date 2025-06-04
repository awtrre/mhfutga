#ifndef BUTTONS_H
#define BUTTONS_H

#include <reg52.h>

/**
 * @brief  Initialize buttons module.
 *         Currently ensures the MOTOR_PORT is off.
 */
void Buttons_Init(void);

/**
 * @brief  Handle Button 1 (K1 on P3.0): 
 *         when pressed, spin forward; on release, run a short tail and stop.
 *         Call repeatedly inside main loop.
 */
void Button1_Handle(void);

/**
 * @brief  Handle Button 2 (K2 on P3.1): 
 *         when pressed, spin reverse; on release, run a short tail and stop.
 *         Call repeatedly inside main loop.
 */
void Button2_Handle(void);

#endif  // BUTTONS_H
#ifndef BUZZER_H
#define BUZZER_H

#include <reg52.h>

// Buzzer pin definition (P2.5)
sbit BUZZER = P2^5;

/**
 * @brief  Initialize buzzer pin and Timer0 settings.
 *         - Configure P2.5 as GPIO output, initial low.
 *         - Configure Timer0 in mode1 (16-bit), interrupt disabled initially.
 */
void Buzzer_Init(void);

/**
 * @brief  Enable automatic beeping during auto-close.
 */
void Buzzer_Enable(void);

/**
 * @brief  Disable automatic beeping and ensure buzzer is off.
 */
void Buzzer_Disable(void);

/**
 * @brief  Produce one short beep (50ms) via Timer0 interrupt (non-blocking).
 *         If buzzer is enabled, this function will:
 *         - Set P2.5 = 1,
 *         - Load Timer0 initial value for ~50ms countdown,
 *         - Enable Timer0 and its interrupt,
 *         - Return immediately (no CPU busy-wait).
 */
void Buzzer_Beep(void);

/**
 * @brief  Return non-zero if buzzer beeping is currently enabled.
 */
bit Buzzer_IsEnabled(void);

#endif /* BUZZER_H */

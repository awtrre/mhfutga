// onekey.h

#ifndef ONEKEY_H
#define ONEKEY_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Total motion time (in ms) representing full open (10 seconds).
 */
#define TOTAL_TIME_MS   10000L   /* 10 000 ms = 10 s */

/**
 * @brief  Immediately stop any ongoing motion and save position to EEPROM.
 */
void OneKey_StopAndSave(void);

/**
 * @brief  Initiate a one-key “open” action:
 *         - If idle and not fully open, run continuously until fully open.
 *         - If already running, call OneKey_StopAndSave().
 */
void OneKey_Open(void);

/**
 * @brief  Initiate a one-key “close” action:
 *         - If idle and not fully closed, run continuously until fully closed.
 *         - If already running, call OneKey_StopAndSave().
 */
void OneKey_Close(void);

/**
 * @brief  Periodic call to handle any internal state transitions.
 *         In this simplified version, OneKey_Open/Close are blocking,
 *         so OneKey_Task() can be left empty or used for expansions.
 */
void OneKey_Task(void);

/**
 * @brief  Return non-zero if motor is currently in motion (opening or closing).
 *         Return 0 if motor is idle.
 */
unsigned char OneKey_IsMoving(void);

#ifdef __cplusplus
}
#endif

#endif /* ONEKEY_H */

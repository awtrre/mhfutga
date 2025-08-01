// uart.h

#ifndef UART_H
#define UART_H

/**
 * @brief Initialize UART for 9600 baud rate at 11.0592MHz.
 * Enables serial port and global interrupts.
 */
// English Comment: Initialize UART for 9600 baud rate at 11.0592MHz.
// English Comment: Enables serial port and global interrupts.
void UART_Init(void);

/**
 * @brief UART processing task to be called in the main loop.
 * Checks if a valid command frame has been received and executes the corresponding action.
 */
// English Comment: UART processing task to be called in the main loop.
// English Comment: Checks if a valid command frame has been received and executes the corresponding action.
void UART_Task(void);

#endif /* UART_H */
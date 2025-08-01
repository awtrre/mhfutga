// uart.c

#include <reg52.h>
#include "uart.h"
#include "onekey.h" // Needed to call OneKey_Open() and OneKey_Close()

// --- UART Communication Variables ---
// English Comment: UART Communication Variables
#define RX_BUFFER_SIZE 5
static char rx_buffer[RX_BUFFER_SIZE];
static unsigned char rx_index = 0;
static bit frame_received = 0; // Flag to indicate a complete frame has been received
                               // English Comment: Flag to indicate a complete frame has been received

/**
 * @brief Initialize UART for 9600 baud rate at 11.0592MHz.
 */
// English Comment: Initialize UART for 9600 baud rate at 11.0592MHz.
void UART_Init(void)
{
    TMOD = 0x20;    // Timer 1, mode 2 (8-bit auto-reload)
    SCON = 0x50;    // Mode 1 (8-bit UART), enable receiver
    TH1 = 0xFD;     // Set baud rate to 9600 (for 11.0592MHz crystal)
    TL1 = 0xFD;     // Set baud rate to 9600
    TR1 = 1;        // Start Timer 1
    ES = 1;         // Enable serial interrupt
    EA = 1;         // Enable global interrupt
}

/**
 * @brief UART processing task to be called in the main loop.
 */
// English Comment: UART processing task to be called in the main loop.
void UART_Task(void)
{
    if (frame_received)
    {
        frame_received = 0; // Clear the flag immediately
                            // English Comment: Clear the flag immediately

        // Check if the received frame is "@K#" for OneKey_Open
        // English Comment: Check if the received frame is "@K#" for OneKey_Open
        if (rx_buffer[0] == '@' && rx_buffer[1] == 'K' && rx_buffer[2] == '#')
        {
            if (!OneKey_IsMoving())
            {
                OneKey_Open(); // Perform one-key open action
                               // English Comment: Perform one-key open action
            }
        }
        // NEW: Check if the received frame is "@G#" for OneKey_Close
        // English Comment: NEW: Check if the received frame is "@G#" for OneKey_Close
        else if (rx_buffer[0] == '@' && rx_buffer[1] == 'G' && rx_buffer[2] == '#')
        {
            if (!OneKey_IsMoving())
            {
                OneKey_Close(); // Perform one-key close action
                                // English Comment: Perform one-key close action
            }
        }
    }
}

/**
 * @brief  UART Interrupt Service Routine. Handles byte reception.
 */
// English Comment: UART Interrupt Service Routine. Handles byte reception.
void UART_ISR(void) interrupt 4
{
    // --- Variable Declaration ---
    // In C51, all local variables must be declared at the beginning of the function.
    // English Comment: In C51, all local variables must be declared at the beginning of the function.
    unsigned char received_char; 
    
    if (RI) // If receive interrupt flag is set
    {
        RI = 0; // Clear the flag
        // English Comment: Clear the flag

        received_char = SBUF; // Read received character from buffer
        // English Comment: Read received character from buffer

        if (received_char == '@') // If start of frame
        {
            rx_index = 0; // Reset buffer index
            // English Comment: Reset buffer index
        }
        
        if (rx_index < RX_BUFFER_SIZE)
        {
            rx_buffer[rx_index] = received_char; // Store character in buffer
            // English Comment: Store character in buffer

            if (received_char == '#' && rx_index > 0) // If end of frame
            {
                frame_received = 1; // Set frame received flag for main loop to handle
                // English Comment: Set frame received flag for main loop to handle
            }
            
            rx_index++;
        }
    }
}
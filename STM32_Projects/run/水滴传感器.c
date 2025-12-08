#include "main.h"

// ==========================================
// 1. Hardware Configuration
// ==========================================
// Rain Sensor Pin Definitions (Digital Output from Sensor)
#define RAIN_SENSOR_PORT      GPIOB
#define RAIN_SENSOR_PIN       GPIO_PIN_2

// Logic Level Definition
// Most sensors pull LOW (0) when rain is detected
#define RAIN_DETECTED_LEVEL   GPIO_PIN_RESET 

// External Global Variables (Defined in main.c or interrupt file)
// 'command_flag' is updated by UART/Voice Interrupt
// 0: No Command, 1: Open, 2: Close, 3: Stop
extern volatile uint8_t command_flag; 

// Definition of Commands (Must match your protocol)
#define CMD_STOP    0x00
#define CMD_OPEN    0x01
#define CMD_CLOSE   0x02

// Function Prototypes (Assuming SmartWindow_Handler is defined elsewhere)
extern void SmartWindow_Handler(uint8_t cmd);

/**
  * @brief  Reads the digital state of the rain sensor.
  * @retval 1 if rain is detected, 0 if dry.
  */
static uint8_t RainSensor_GetState(void)
{
    if (HAL_GPIO_ReadPin(RAIN_SENSOR_PORT, RAIN_SENSOR_PIN) == RAIN_DETECTED_LEVEL)
    {
        return 1; // Raining
    }
    return 0; // Not Raining
}

/**
  * @brief  Main processing task for the Smart Window System.
  * Integrates Sensor Logic and Voice Logic with Priority Control.
  * @note   Call this function continuously in the while(1) loop.
  */
void SmartWindow_ProcessTask(void)
{
    // Static variable to track the previous rain state
    // This prevents sending the CLOSE command repeatedly every millisecond while raining
    static uint8_t is_raining_previous = 0;

    // Step 1: Check Rain Sensor (High Priority)
    uint8_t is_raining_now = RainSensor_GetState();

    if (is_raining_now == 1)
    {
        // === RAIN DETECTED ===
        
        // Only execute logic if the state has just changed (Edge Detection)
        // or if you want to enforce closing periodically.
        if (is_raining_previous == 0)
        {
            // Action 1: Force Close the Window
            SmartWindow_Handler(CMD_CLOSE);
            
            // Action 2: Clear any pending "Open" voice commands for safety
            command_flag = CMD_STOP; 
        }

        // Update history
        is_raining_previous = 1;
    }
    else
    {
        // === NO RAIN (Normal Operation) ===
        
        // Reset rain history when rain stops
        is_raining_previous = 0;

        // Step 2: Check Voice Commands (Lower Priority)
        if (command_flag != CMD_STOP)
        {
            // Execute the user's voice command
            SmartWindow_Handler(command_flag);

            // Optional: Consume the command (Reset flag) 
            // to avoid re-executing the same command repeatedly
            // command_flag = CMD_STOP; 
        }
    }
}

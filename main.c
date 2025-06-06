// main.c

#include <reg52.h>
#include "buttons.h"     /* For Buttons_Init(), ButtonX_Handle() */
#include "onekey.h"      /* For OneKey_Open(), OneKey_Close(), OneKey_Task(), OneKey_IsMoving() */
#include "motor.h"       /* For delay_ms() and stepper functions */
#include "hcsr04.h"      /* For HCSR04_Init(), HCSR04_Task() */
#include "eeprom.h"      /* For EEPROM_ReadByte() */
#include "buzzer.h" 		/* NEW: For Buzzer_Init(), etc. */


long g_position_ms = 0;    /* Global motor position in ms */

/**
 * @brief  Read the 32-bit saved position from EEPROM (0x2000..0x2003),
 *         combine into 'g_position_ms' (little-endian).
 */
static void LoadPositionFromEEPROM(void)
{
    unsigned char b0, b1, b2, b3;
    unsigned int base_addr = 0x2000;

    /* Read four bytes from EEPROM */
    b0 = EEPROM_ReadByte(base_addr);
    b1 = EEPROM_ReadByte(base_addr + 1);
    b2 = EEPROM_ReadByte(base_addr + 2);
    b3 = EEPROM_ReadByte(base_addr + 3);

    /* Combine into a signed 32-bit long (little-endian) */
    g_position_ms =  (long)b0
                   | ((long)b1 << 8)
                   | ((long)b2 << 16)
                   | ((long)b3 << 24);
}

void main(void)
{
    /* 1) Initialize buttons (configure P3.0~P3.3 as inputs with pull-ups) */
    Buttons_Init();

    /* 2) Restore last saved motor position from EEPROM */
    LoadPositionFromEEPROM();

    /* 3) Initialize HC-SR04 ultrasonic sensor pins */
    HCSR04_Init();
	
	  /* 4) Initialize buzzer pin */
		Buzzer_Init();   
	
    while (1)
    {
        /* --- Handle manual buttons (blocking in 700ms slices) --- */
        Button1_Handle();     /* Manual forward (P3.0) */
        Button2_Handle();     /* Manual reverse (P3.1) */

        /* --- Handle one-key OPEN / CLOSE (blocking until done or interrupted) --- */
        Button3_Handle();     /* One-key OPEN (P3.2) */
        Button4_Handle();     /* One-key CLOSE (P3.3) */

        /* 
         * --- HC-SR04 automatic obstacle check ---
         * Only call HCSR04_Task when motor is IDLE.
         */
        if (!OneKey_IsMoving())
        {
            HCSR04_Task();
        }

        /* 
         * --- No need to call OneKey_Task() here, 
         *     because blocking OneKey_Open/Close do not require slices. --- 
         */

        /* --- Main loop tick: approx. 10 ms delay --- */
        delay_ms(10);
    }
}

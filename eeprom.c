// eeprom.c

#include "eeprom.h"
#include <intrins.h>     /* _nop_() */

/* IAP special function registers */
sfr ISP_DATA  = 0xE2;
sfr ISP_ADDRH = 0xE3;
sfr ISP_ADDRL = 0xE4;
sfr ISP_CMD   = 0xE5;
sfr ISP_TRIG  = 0xE6;
sfr ISP_CONTR = 0xE7;

/* IAP timing: 12 MHz crystal -> WAIT_TIME = 0x02 */
#define WAIT_TIME       0x02

/* ISP command codes */
#define ISP_CMD_READ    0x01
#define ISP_CMD_PROGRAM 0x02
#define ISP_CMD_ERASE   0x03

/**
 * @brief  Enable IAP (In-Application Programming) interface.
 */
static void IAP_Enable(void)
{
    EA = 0;                              /* Disable global interrupts */
    ISP_CONTR = (ISP_CONTR & 0x18) | WAIT_TIME;
    ISP_CONTR |= 0x80;                   /* Set ISPEN = 1 */
}

/**
 * @brief  Disable IAP interface and re-enable interrupts.
 */
static void IAP_Disable(void)
{
    ISP_CONTR &= 0x7F;                   /* Clear ISPEN = 0 */
    ISP_TRIG   = 0x00;
    EA = 1;                              /* Re-enable interrupts */
}

/**
 * @brief  Trigger the IAP operation sequence.
 */
static void IAP_Trigger(void)
{
    ISP_TRIG = 0x46;
    ISP_TRIG = 0xB9;
    _nop_();                             /* Timing NOP */
}

/**
 * @brief  Read one byte from Data-Flash at byte_addr.
 */
unsigned char EEPROM_ReadByte(unsigned int byte_addr)
{
    unsigned char val;
    ISP_ADDRH = (unsigned char)(byte_addr >> 8);
    ISP_ADDRL = (unsigned char) byte_addr;
    ISP_CMD   = (ISP_CMD & 0xF8) | ISP_CMD_READ;

    IAP_Enable();
    IAP_Trigger();
    val = ISP_DATA;
    IAP_Disable();

    return val;
}

/**
 * @brief  Erase the 512-byte sector aligned to sector_addr.
 */
void EEPROM_EraseSector(unsigned int sector_addr)
{
    unsigned int base = sector_addr & 0xFE00;   /* Align to 512-byte boundary */

    ISP_ADDRH = (unsigned char)(base >> 8);
    ISP_ADDRL = 0x00;
    ISP_CMD   = (ISP_CMD & 0xF8) | ISP_CMD_ERASE;

    IAP_Enable();
    IAP_Trigger();
    IAP_Disable();
}

/**
 * @brief  Write one byte to Data-Flash at byte_addr (sector must be erased).
 */
void EEPROM_WriteByte(unsigned int byte_addr, unsigned char value)
{
    ISP_ADDRH = (unsigned char)(byte_addr >> 8);
    ISP_ADDRL = (unsigned char) byte_addr;
    ISP_DATA  = value;                       /* Load data */
    ISP_CMD   = (ISP_CMD & 0xF8) | ISP_CMD_PROGRAM;

    IAP_Enable();
    IAP_Trigger();
    IAP_Disable();
}

/**
 * @brief  Initialize EEPROM emulation (empty for STC89C52RC).
 */
void EEPROM_Init(void)
{
    /* No action needed for Data-Flash on STC89C52RC */
}

extern long g_position_ms;  /* Defined in main.c */

/**
 * @brief  Store the global g_position_ms into Data-Flash at 0x2000..0x2003.
 */
void SavePositionToEEPROM(void)
{
    unsigned char b0, b1, b2, b3;

    /* Split 32-bit value into four bytes (little-endian) */
    b0 = (unsigned char)( g_position_ms        & 0xFF );
    b1 = (unsigned char)((g_position_ms >>  8) & 0xFF);
    b2 = (unsigned char)((g_position_ms >> 16) & 0xFF);
    b3 = (unsigned char)((g_position_ms >> 24) & 0xFF);

    /* Erase 512-byte sector at 0x2000 */
    EEPROM_EraseSector(0x2000);

    /* Program the four bytes */
    EEPROM_WriteByte(0x2000, b0);
    EEPROM_WriteByte(0x2001, b1);
    EEPROM_WriteByte(0x2002, b2);
    EEPROM_WriteByte(0x2003, b3);
}

// eeprom.h

#ifndef EEPROM_H
#define EEPROM_H

#include <reg52.h>

/**
 * @brief Initialize EEPROM emulation (Data-Flash).
 *        (No special init required for STC89C52RC Data-Flash.)
 */
void EEPROM_Init(void);

/**
 * @brief Read a byte from internal Data-Flash.
 *
 * @param byte_addr 16-bit address (e.g., 0x2000 ~ 0x20FF).
 * @return Byte read from Flash.
 */
unsigned char EEPROM_ReadByte(unsigned int byte_addr);

/**
 * @brief Erase one 512-byte sector (address must be sector-aligned).
 *
 * @param sector_addr Sector start address (e.g., 0x2000).
 */
void EEPROM_EraseSector(unsigned int sector_addr);

/**
 * @brief Write one byte into Data-Flash (sector must be erased first).
 *
 * @param byte_addr 16-bit address within Data-Flash.
 * @param value     Byte to be programmed.
 */
void EEPROM_WriteByte(unsigned int byte_addr, unsigned char value);

/**
 * @brief Store the global variable g_position_ms (32-bit) into EEPROM
 *        at addresses 0x2000..0x2003 (little-endian).
 */
void SavePositionToEEPROM(void);

#endif /* EEPROM_H */

#ifndef EEPROMUTILS_H
#define EEPROMUTILS_H

#define EEPROM_START_C1 EEPROM_STORAGE_SPACE_START
#define EEPROM_START_C2 EEPROM_START_C1 + 1
#define EEPROM_SCORE    EEPROM_START_C1 + 2

//-----------------------------------------------------------------------------------------
// This will initalize the beginning of the memory to have the highscore
// It also checks for initials to verify the data is there before it is written to
//-----------------------------------------------------------------------------------------
void initEEPROM() {

  uint8_t c1 = EEPROM.read(EEPROM_START_C1);
  uint8_t c2 = EEPROM.read(EEPROM_START_C2);

  if(c1 != 'D' || c2 != 'I') {
    EEPROM.update(EEPROM_START_C1, 'D');
    EEPROM.update(EEPROM_START_C2, 'I');
    EEPROM.put(EEPROM_SCORE, (uint16_t)0);
  }

}

#endif
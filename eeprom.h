#ifndef EEPROM_H_INCLUDED
#define EEPROM_H_INCLUDED



extern void    EepromWrite(uint16_t addr, uint8_t val);
extern uint8_t EepromRead (uint16_t addr);


#endif // EEPROM_H_INCLUDED

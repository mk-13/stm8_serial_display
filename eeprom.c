#include "stm8s.h"
#include "eeprom.h"

void    EepromWrite(uint16_t addr, uint8_t val)
{
    char *address;
    if ((FLASH->IAPSR & FLASH_IAPSR_DUL) == 0)
    {
       FLASH->DUKR = 0xae;
       FLASH->DUKR = 0x56;
    }

    address = (char *) (0x4000 + addr);
    *address = val;

    //FLASH->IAPSR &= ~FLASH_IAPSR_DUL;
}


uint8_t EepromRead(uint16_t addr)
{
    uint8_t *address = (uint8_t *) (0x4000 + addr);
    return (*address);
}

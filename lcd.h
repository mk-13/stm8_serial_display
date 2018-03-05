#ifndef LCD_H_INCLUDED
#define LCD_H_INCLUDED

#define SSD1306

#define LCD_C     0
#define LCD_D     1

extern void LcdWrite(unsigned char dc, unsigned char data);

extern void LcdCharacter(char character);
extern void LcdClear(void);
extern void LcdString(char *characters);
extern void LcdInitialise(void);
extern void LcdInvertLine(uint8_t row);


extern void LcdSetScroll(unsigned char s);
extern void LcdSetInvert(unsigned char i);
extern void LcdSetCharPos(unsigned char row, unsigned char column);
extern void LcdSetMemoryPtr(unsigned char row, unsigned char ofs);
extern void LcdSetLight(unsigned char l);
extern void LcdSetDiagram(unsigned char d);


//Diagramm/Grafik Funktionen
extern void LcdSetDiaMaxVal(int v);
extern void LcdDiaClear(void);
extern void LcdAddDiaValue(uint8_t v);
extern void LcdAddGraphicData(uint8_t b1, uint8_t b2, uint8_t b3);
extern void LcdDiaUpdate(void);


extern uint8_t diagramActive;


// SPI Flash Functions
extern void get_jedec_id(uint8_t *b1,uint8_t *b2, uint8_t *b3);
extern void display_flash_page(uint16_t page_number);
extern void showFlashPictPart(uint16_t addr, uint8_t StartRow, uint8_t StartCol);


extern void flash_fill_page(uint16_t page, uint8_t val);
extern void flash_erase_sector(uint16_t sectorAdr);

extern void flash_writePageStart(uint16_t page);
extern void flash_writePageEnd(void);


extern void shiftOut(uint8_t data);


#endif // LCD_H_INCLUDED

#ifndef LCD_CMD_H
#define LCD_CMD_H

#define CMD_CLEAR_DISPLAY    0xFF
#define CMD_SET_CURSOR       0xFE
#define CMD_SET_INVERT       0xFC    // LSB indicates on/off
#define CMD_SET_FUNCTION     0xFA    // LSB indicates on/off, second Byte contains Function Bits Scroll/Light/Diagram/Monitor,
                                     //               optional 3. Byte
                                     //                   for MONITOR:
                                     //                     BIT7..BIT4  Monitor refreshrate in ms/100
                                     //                     BIT3..BIT0  channels for monitoring, Bitposition corresponds to channel
                                     //                   for MONITOR_DIAGRAM:
                                     //                     BIT7..BIT4  Monitor refreshrate in ms/100
                                     //                     BIT3..BIT0  channel to draw as BCD
#define CMD_GRAPHIC          0xF8    // 1 Byte Line 0..5/7
                                     // 1 Byte Offset
                                     // 1 Byte Anz Bytes (max 84)
                                     // 1 Byte Header-CS
                                     // Grafik Bytes

#define CMD_RESET            0xF7    // set all values to default and clear display
#define CMD_INVERT_LINE      0xF6    // 1 Byte Line 0..5/7
#define CMD_SET_DIAGRAM_MAX  0xF5    // 1 Byte Max Wert
#define CMD_SET_DIAGRAM_VAL  0xF4    // 1 Byte zur Visualisierung im Diagramm
#define CMD_CLEAR_DIAGRAM    0xF3
#define CMD_SET_GRAFIK_BYTES 0xF2    // 3 Byte fuer eine Spalte, Oberste Reihe zuerst, LSB ist oben
#define CMD_SHOW_CONFIG      0xF1
#define CMD_CONFIG           0xF0    // Config Comm and, 3 Byte : Item, value, summe der zuvor empfangenen Bytes als Checksumme

#define CMD_VARIABLE         '$'     // wenn Zeichen '1'..'4' folgt, wird die korrespondierende Spannung/ADC-Wert ausgegeben


#define SCROLL  1
#define LIGHT   2
#define DIAGRAM 4
#define MONITOR 8            // Wenn On, dann wird im nächsten Byte angegeben, welche Signale ausgegeben werden sollen
#define MONITOR_DIAGRAM 16   // Wenn On, dann wird im nächsten Byte angegeben, welches Signal im Diagramm dargestellt wird

#define CFG_ITEM_BAUD_RATE   0x01
#define CFG_ITEM_I2C_ADDR    0x02
#define CFG_ITEM_VMAP_HIGH   0x03
#define CFG_ITEM_VMAP_LOW    0x04



/*
    For CMD_GRAPHIC a Number of sub cmds is defined
	 --> defined in range>128 too in order to avaiod collisions with printable chars
*/

#define SUB_CMD_DISPLAY_GRAPHIC_FULL  0x80  // followed by complete image
#define SUB_CMD_DISPLAY_GRAPHIC_PART  0x81  // height in rows (1..7), width in pixel,  first row, first column, data
#define SUB_CMD_FLASH_GRAPHIC_FULL    0x90  // picture id high, picture id low
#define SUB_CMD_FLASH_GRAPHIC_PART    0x91  // picture id high, picture id low, first row, first column  -> height/width stored in flash
#define SUB_CMD_FLASH_ERASE_SECTOR    0xA0  // sector addr high, sector addr low, cs
#define SUB_CMD_FLASH_WRITE_PAGE      0xA1  // page addr high,  page addr low, len-1, cs, data

/* Checksum cs: 
     - used only for flash write cmds in order to block unintended modification of flash by random data 
	    (e.g. received if Arduino gets programmed)
     - 8 LSBits of Bytes SUB_CMD to Byte before cs
 */


#endif

// core header file from our library project:
#include "stm8s.h"
#include "lcd.h"
#include "timebase.h"

/*
                         STM8S103F3
                     .-----\/-----.
  LIGHT         D4  -| 1        20|- D3            ANA4
  ANA5      TX  D5  -| 2        19|- D2            ANA3
  RX      ANA6  D6  -| 3        18|- D1 SWIM
                RST -| 4        17|- C7 SPI_MISO         DO_FLASH
  DC            A1  -| 5        16|- C6 SPI_MOSI   SDIN  DI_FLASH
  CS_FLASH      A2  -| 6        15|- C5 SPI_CLK    SCLK  CLK_FLASH
                GND -| 7        14|- C4            AIN2
                VCA -| 8        13|- C3            RESET
                VDD -| 9        12|- B4            SCL
  SCE    SPI_SS A3  -|10        11|- B5            SDA
                     '------------'


Fuer OLED Display:


 ----------------------------------------------------
                         Pinbelegung
    Portpin STM8S                     OLED Display
    oled_uhr Projekt    lcd
    ----------------------------------------------------
          |                                  |
         PD4            C6            // D1 / Mosi
         PD1            A3            // CS
         PC4            C3            // RES
         PC3            A1            // DC (Data or command)
         PD2            C5            // D0 / SCK


                     G   V           R
                     N   c   D   D   E   D   C
                     D   c   0   1   S   C   s
                 +-------------------------------+
                 |   o   o   o   o   o   o   o   |
                 |                               |
                 |   -------------------------   |
                 |  |                         |  |
                 |  |                         |  |
                 |  |                         |  |
                 |  |                         |  |
                 |  |                         |  |
                 |  |                         |  |
                 |   -----+-------------+-----   |
                 |        |             |        |
                 |        +-------------+        |
                 +-------------------------------+




*/



//#define CHAR_X_SIZE_6



#define PIN_SCE   GPIO_PIN_3
#define PORT_SCE  GPIOA

#define PIN_RESET  GPIO_PIN_3
#define PORT_RESET GPIOC

#define PIN_DC    GPIO_PIN_1
#define PORT_DC   GPIOA

#define PIN_SDIN  GPIO_PIN_6
#define PORT_SDIN GPIOC

#define PIN_SCLK  GPIO_PIN_5
#define PORT_SCLK GPIOC

#define PIN_LED   GPIO_PIN_4
#define PORT_LED  GPIOD



#define PIN_FLASH_DO GPIO_PIN_7
#define PORT_FLASH_DO GPIOC


#define PIN_FLASH_CS GPIO_PIN_2
#define PORT_FLASH_CS GPIOA


#ifdef CHAR_X_SIZE_6
#define DISP_CHAR_SIZE_X 6
#else
#define DISP_CHAR_SIZE_X 8
#endif

#define DISP_CHAR_SIZE_Y 8

#ifdef SSD1306

#define LCD_X     128
#define LCD_Y     64

#else

#define LCD_X     84
#define LCD_Y     48

#endif // SSD1306


#define CHARS_PER_ROW (LCD_X/DISP_CHAR_SIZE_X)
#define NO_OF_ROW     (LCD_Y/DISP_CHAR_SIZE_Y)


#define DIA_UPDATE_TIME 200

#define INVERT_BIT  0x80


#ifdef SSD1306

#define oled_enable()   ( PORT_SCE->ODR &= ~PIN_SCE )
#define oled_cmdmode()  ( PORT_DC->ODR &= ~PIN_DC )
#define oled_datamode() ( PORT_DC->ODR |= PIN_DC )
#define rst_clr()       ( PORT_RESET->ODR &= ~PIN_RESET )
#define rst_set()       ( PORT_RESET->ODR |= PIN_RESET )

#define cs_clr()       ( PORT_SCE->ODR &= ~PIN_SCE )
#define cs_set()       ( PORT_SCE->ODR |= PIN_SCE )


#define spi_out shiftOut

#endif


typedef unsigned char byte;

#ifdef CHAR_X_SIZE_6

const byte ASCII[]  =
{
 0x00, 0x00, 0x00, 0x00, 0x00, // 20
 0x00, 0x00, 0x5f, 0x00, 0x00, // 21 !
 0x00, 0x07, 0x00, 0x07, 0x00, // 22 "
 0x14, 0x7f, 0x14, 0x7f, 0x14, // 23 #
 0x24, 0x2a, 0x7f, 0x2a, 0x12, // 24 $
 0x23, 0x13, 0x08, 0x64, 0x62, // 25 %
 0x36, 0x49, 0x55, 0x22, 0x50, // 26 &
 0x00, 0x05, 0x03, 0x00, 0x00, // 27 '
 0x00, 0x1c, 0x22, 0x41, 0x00, // 28 (
 0x00, 0x41, 0x22, 0x1c, 0x00, // 29 )
 0x14, 0x08, 0x3e, 0x08, 0x14, // 2a *
 0x08, 0x08, 0x3e, 0x08, 0x08, // 2b +
 0x00, 0x50, 0x30, 0x00, 0x00, // 2c ,
 0x08, 0x08, 0x08, 0x08, 0x08, // 2d -
 0x00, 0x60, 0x60, 0x00, 0x00, // 2e .
 0x20, 0x10, 0x08, 0x04, 0x02, // 2f /
 0x3e, 0x51, 0x49, 0x45, 0x3e, // 30 0
 0x00, 0x42, 0x7f, 0x40, 0x00, // 31 1
 0x42, 0x61, 0x51, 0x49, 0x46, // 32 2
 0x21, 0x41, 0x45, 0x4b, 0x31, // 33 3
 0x18, 0x14, 0x12, 0x7f, 0x10, // 34 4
 0x27, 0x45, 0x45, 0x45, 0x39, // 35 5
 0x3c, 0x4a, 0x49, 0x49, 0x30, // 36 6
 0x01, 0x71, 0x09, 0x05, 0x03, // 37 7
 0x36, 0x49, 0x49, 0x49, 0x36, // 38 8
 0x06, 0x49, 0x49, 0x29, 0x1e, // 39 9
 0x00, 0x36, 0x36, 0x00, 0x00, // 3a :
 0x00, 0x56, 0x36, 0x00, 0x00, // 3b ;
 0x08, 0x14, 0x22, 0x41, 0x00, // 3c <
 0x14, 0x14, 0x14, 0x14, 0x14, // 3d =
 0x00, 0x41, 0x22, 0x14, 0x08, // 3e >
 0x02, 0x01, 0x51, 0x09, 0x06, // 3f ?
 0x32, 0x49, 0x79, 0x41, 0x3e, // 40 @
 0x7e, 0x11, 0x11, 0x11, 0x7e, // 41 A
 0x7f, 0x49, 0x49, 0x49, 0x36, // 42 B
 0x3e, 0x41, 0x41, 0x41, 0x22, // 43 C
 0x7f, 0x41, 0x41, 0x22, 0x1c, // 44 D
 0x7f, 0x49, 0x49, 0x49, 0x41, // 45 E
 0x7f, 0x09, 0x09, 0x09, 0x01, // 46 F
 0x3e, 0x41, 0x49, 0x49, 0x7a, // 47 G
 0x7f, 0x08, 0x08, 0x08, 0x7f, // 48 H
 0x00, 0x41, 0x7f, 0x41, 0x00, // 49 I
 0x20, 0x40, 0x41, 0x3f, 0x01, // 4a J
 0x7f, 0x08, 0x14, 0x22, 0x41, // 4b K
 0x7f, 0x40, 0x40, 0x40, 0x40, // 4c L
 0x7f, 0x02, 0x0c, 0x02, 0x7f, // 4d M
 0x7f, 0x04, 0x08, 0x10, 0x7f, // 4e N
 0x3e, 0x41, 0x41, 0x41, 0x3e, // 4f O
 0x7f, 0x09, 0x09, 0x09, 0x06, // 50 P
 0x3e, 0x41, 0x51, 0x21, 0x5e, // 51 Q
 0x7f, 0x09, 0x19, 0x29, 0x46, // 52 R
 0x46, 0x49, 0x49, 0x49, 0x31, // 53 S
 0x01, 0x01, 0x7f, 0x01, 0x01, // 54 T
 0x3f, 0x40, 0x40, 0x40, 0x3f, // 55 U
 0x1f, 0x20, 0x40, 0x20, 0x1f, // 56 V
 0x3f, 0x40, 0x38, 0x40, 0x3f, // 57 W
 0x63, 0x14, 0x08, 0x14, 0x63, // 58 X
 0x07, 0x08, 0x70, 0x08, 0x07, // 59 Y
 0x61, 0x51, 0x49, 0x45, 0x43, // 5a Z
 0x00, 0x7f, 0x41, 0x41, 0x00, // 5b [
 0x02, 0x04, 0x08, 0x10, 0x20, // 5c ¥
 0x00, 0x41, 0x41, 0x7f, 0x00, // 5d ]
 0x04, 0x02, 0x01, 0x02, 0x04, // 5e ^
 0x40, 0x40, 0x40, 0x40, 0x40, // 5f _
 0x00, 0x01, 0x02, 0x04, 0x00, // 60 `
 0x20, 0x54, 0x54, 0x54, 0x78, // 61 a
 0x7f, 0x48, 0x44, 0x44, 0x38, // 62 b
 0x38, 0x44, 0x44, 0x44, 0x20, // 63 c
 0x38, 0x44, 0x44, 0x48, 0x7f, // 64 d
 0x38, 0x54, 0x54, 0x54, 0x18, // 65 e
 0x08, 0x7e, 0x09, 0x01, 0x02, // 66 f
 0x0c, 0x52, 0x52, 0x52, 0x3e, // 67 g
 0x7f, 0x08, 0x04, 0x04, 0x78, // 68 h
 0x00, 0x44, 0x7d, 0x40, 0x00, // 69 i
 0x20, 0x40, 0x44, 0x3d, 0x00, // 6a j
 0x7f, 0x10, 0x28, 0x44, 0x00, // 6b k
 0x00, 0x41, 0x7f, 0x40, 0x00, // 6c l
 0x7c, 0x04, 0x18, 0x04, 0x78, // 6d m
 0x7c, 0x08, 0x04, 0x04, 0x78, // 6e n
 0x38, 0x44, 0x44, 0x44, 0x38, // 6f o
 0x7c, 0x14, 0x14, 0x14, 0x08, // 70 p
 0x08, 0x14, 0x14, 0x18, 0x7c, // 71 q
 0x7c, 0x08, 0x04, 0x04, 0x08, // 72 r
 0x48, 0x54, 0x54, 0x54, 0x20, // 73 s
 0x04, 0x3f, 0x44, 0x40, 0x20, // 74 t
 0x3c, 0x40, 0x40, 0x20, 0x7c, // 75 u
 0x1c, 0x20, 0x40, 0x20, 0x1c, // 76 v
 0x3c, 0x40, 0x30, 0x40, 0x3c, // 77 w
 0x44, 0x28, 0x10, 0x28, 0x44, // 78 x
 0x0c, 0x50, 0x50, 0x50, 0x3c, // 79 y
 0x44, 0x64, 0x54, 0x4c, 0x44, // 7a z
 0x00, 0x08, 0x36, 0x41, 0x00, // 7b {
 0x00, 0x00, 0x7f, 0x00, 0x00, // 7c |
 0x00, 0x41, 0x36, 0x08, 0x00, // 7d }
 0x10, 0x08, 0x08, 0x10, 0x08, // 7e <-
 0x78, 0x46, 0x41, 0x46, 0x78  // 7f ->
};

#else

const byte ASCII[] = {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,       // Ascii 32 = ' '
  0x00,0x00,0x00,0xfa,0xfa,0x00,0x00,       // Ascii 33 = '!'
  0x00,0xe0,0xe0,0x00,0xe0,0xe0,0x00,       // Ascii 34 = '"'
  0x28,0xfe,0xfe,0x28,0xfe,0xfe,0x28,       // Ascii 35 = '#'
  0x00,0x24,0x54,0xfe,0xfe,0x54,0x48,       // Ascii 36 = '$'
  0x00,0x62,0x66,0x0c,0x18,0x30,0x66,       // Ascii 37 = '%'
  0x00,0x0c,0x5e,0xf2,0xba,0xec,0x5e,       // Ascii 38 = '&'
  0x00,0x00,0x20,0xe0,0xc0,0x00,0x00,       // Ascii 39 = '''
  0x00,0x00,0x38,0x7c,0xc6,0x82,0x00,       // Ascii 40 = '('
  0x00,0x00,0x82,0xc6,0x7c,0x38,0x00,       // Ascii 41 = ')'
  0x10,0x54,0x7c,0x38,0x38,0x7c,0x54,       // Ascii 42 = '*'
  0x00,0x10,0x10,0x7c,0x7c,0x10,0x10,       // Ascii 43 = '+'
  0x00,0x00,0x00,0x06,0x06,0x00,0x00,       // Ascii 44 = ','
  0x00,0x10,0x10,0x10,0x10,0x10,0x10,       // Ascii 45 = '-'
  0x00,0x00,0x00,0x06,0x06,0x00,0x00,       // Ascii 46 = '.'
  0x06,0x0c,0x18,0x30,0x60,0xc0,0x80,       // Ascii 47 = '/'
  0x7c,0xfe,0x8a,0x92,0xa2,0xfe,0x7c,       // Ascii 48 = '0'
  0x00,0x00,0x40,0xfe,0xfe,0x00,0x00,       // Ascii 49 = '1'
  0x00,0x42,0xc6,0x8e,0x9a,0xf2,0x62,       // Ascii 50 = '2'
  0x00,0x44,0xc6,0x92,0x92,0xfe,0x6c,       // Ascii 51 = '3'
  0x18,0x38,0x68,0xc8,0xfe,0xfe,0x08,       // Ascii 52 = '4'
  0x00,0xe4,0xe6,0xa2,0xa2,0xbe,0x9c,       // Ascii 53 = '5'
  0x00,0x7c,0xfe,0x92,0x92,0xde,0x4c,       // Ascii 54 = '6'
  0x00,0x80,0x80,0x8e,0x9e,0xf0,0xe0,       // Ascii 55 = '7'
  0x00,0x6c,0xfe,0x92,0x92,0xfe,0x6c,       // Ascii 56 = '8'
  0x00,0x60,0xf2,0x96,0x9c,0xf8,0x70,       // Ascii 57 = '9'
  0x00,0x00,0x00,0x36,0x36,0x00,0x00,       // Ascii 58 = ':'
  0x00,0x00,0x80,0xb6,0x36,0x00,0x00,       // Ascii 59 = ';'
  0x00,0x10,0x38,0x6c,0xc6,0x82,0x00,       // Ascii 60 = '<'
  0x00,0x24,0x24,0x24,0x24,0x24,0x24,       // Ascii 61 = '='
  0x00,0x82,0xc6,0x6c,0x38,0x10,0x00,       // Ascii 62 = '>'
  0x00,0x40,0xc0,0x8a,0x9a,0xf0,0x60,       // Ascii 63 = '?'
  0x7c,0xfe,0x82,0xba,0xba,0xfa,0x78,       // Ascii 64 = '@'
  0x3e,0x7e,0xc8,0x88,0xc8,0x7e,0x3e,       // Ascii 65 = 'A'
  0xfe,0xfe,0x92,0x92,0x92,0xfe,0x6c,       // Ascii 66 = 'B'
  0x38,0x7c,0xc6,0x82,0x82,0xc6,0x44,       // Ascii 67 = 'C'
  0xfe,0xfe,0x82,0x82,0xc6,0x7c,0x38,       // Ascii 68 = 'D'
  0xfe,0xfe,0x92,0x92,0x92,0x82,0x82,       // Ascii 69 = 'E'
  0xfe,0xfe,0x90,0x90,0x90,0x80,0x80,       // Ascii 70 = 'F'
  0x38,0x7c,0xc6,0x82,0x8a,0xce,0x4e,       // Ascii 71 = 'G'
  0xfe,0xfe,0x10,0x10,0x10,0xfe,0xfe,       // Ascii 72 = 'H'
  0x00,0x82,0xfe,0xfe,0x82,0x00,0x00,       // Ascii 73 = 'I'
  0x0c,0x0e,0x02,0x02,0x02,0xfe,0xfc,       // Ascii 74 = 'J'
  0xfe,0xfe,0x90,0xb8,0x6c,0xc6,0x82,       // Ascii 75 = 'K'
  0xfe,0xfe,0x02,0x02,0x02,0x02,0x02,       // Ascii 76 = 'L'
  0xfe,0xfe,0x60,0x30,0x60,0xfe,0xfe,       // Ascii 77 = 'M'
  0xfe,0xfe,0x60,0x30,0x18,0xfe,0xfe,       // Ascii 78 = 'N'
  0x38,0x7c,0xc6,0x82,0xc6,0x7c,0x38,       // Ascii 79 = 'O'
  0xfe,0xfe,0x90,0x90,0x90,0xf0,0x60,       // Ascii 80 = 'P'
  0x38,0x7c,0xc6,0x8a,0xcc,0x76,0x3a,       // Ascii 81 = 'Q'
  0xfe,0xfe,0x90,0x90,0x98,0xfe,0x66,       // Ascii 82 = 'R'
  0x64,0xf6,0x92,0x92,0x92,0xde,0x4c,       // Ascii 83 = 'S'
  0x80,0x80,0xfe,0xfe,0x80,0x80,0x00,       // Ascii 84 = 'T'
  0xfc,0xfe,0x02,0x02,0x02,0xfe,0xfc,       // Ascii 85 = 'U'
  0xf8,0xfc,0x06,0x02,0x06,0xfc,0xf8,       // Ascii 86 = 'V'
  0xfe,0xfe,0x0c,0x18,0x0c,0xfe,0xfe,       // Ascii 87 = 'W'
  0xc6,0xee,0x38,0x10,0x38,0xee,0xc6,       // Ascii 88 = 'X'
  0x00,0xe0,0xf0,0x1e,0x1e,0xf0,0xe0,       // Ascii 89 = 'Y'
  0x82,0x86,0x8e,0x9a,0xb2,0xe2,0xc2,       // Ascii 90 = 'Z'
  0x00,0x00,0xfe,0xfe,0x82,0x82,0x00,       // Ascii 91 = '['
  0x80,0xc0,0x60,0x30,0x18,0x0c,0x06,       // Ascii 92 = '\'
  0x00,0x00,0x82,0x82,0xfe,0xfe,0x00,       // Ascii 93 = ']'
  0x00,0x10,0x30,0x60,0xc0,0x60,0x30,       // Ascii 94 = '^'
  0x01,0x01,0x01,0x01,0x01,0x01,0x01,       // Ascii 95 = '_'
  0x00,0x00,0x80,0xc0,0x60,0x20,0x00,       // Ascii 96 = '`'
  0x04,0x2e,0x2a,0x2a,0x2a,0x3e,0x1e,       // Ascii 97 = 'a'
  0xfe,0xfe,0x22,0x22,0x22,0x3e,0x1c,       // Ascii 98 = 'b'
  0x1c,0x3e,0x22,0x22,0x22,0x36,0x14,       // Ascii 99 = 'c'
  0x1c,0x3e,0x22,0x22,0x22,0xfe,0xfe,       // Ascii 100 = 'd'
  0x1c,0x3e,0x2a,0x2a,0x2a,0x3a,0x18,       // Ascii 101 = 'e'
  0x10,0x7e,0xfe,0x90,0x90,0xc0,0x40,       // Ascii 102 = 'f'
  0x19,0x3d,0x25,0x25,0x25,0x3f,0x3e,       // Ascii 103 = 'g'
  0xfe,0xfe,0x20,0x20,0x20,0x3e,0x1e,       // Ascii 104 = 'h'
  0x00,0x00,0x00,0xbe,0xbe,0x00,0x00,       // Ascii 105 = 'i'
  0x02,0x03,0x01,0x01,0xbf,0xbe,0x00,       // Ascii 106 = 'j'
  0xfe,0xfe,0x08,0x08,0x1c,0x36,0x22,       // Ascii 107 = 'k'
  0x00,0x00,0x00,0xfe,0xfe,0x00,0x00,       // Ascii 108 = 'l'
  0x1e,0x3e,0x30,0x18,0x30,0x3e,0x1e,       // Ascii 109 = 'm'
  0x3e,0x3e,0x20,0x20,0x20,0x3e,0x1e,       // Ascii 110 = 'n'
  0x1c,0x3e,0x22,0x22,0x22,0x3e,0x1c,       // Ascii 111 = 'o'
  0x3f,0x3f,0x24,0x24,0x24,0x3c,0x18,       // Ascii 112 = 'p'
  0x18,0x3c,0x24,0x24,0x24,0x3f,0x3f,       // Ascii 113 = 'q'
  0x3e,0x3e,0x20,0x20,0x20,0x30,0x10,       // Ascii 114 = 'r'
  0x12,0x3a,0x2a,0x2a,0x2a,0x2e,0x04,       // Ascii 115 = 's'
  0x20,0xfc,0xfe,0x22,0x22,0x26,0x04,       // Ascii 116 = 't'
  0x3c,0x3e,0x02,0x02,0x02,0x3e,0x3e,       // Ascii 117 = 'u'
  0x38,0x3c,0x06,0x02,0x06,0x3c,0x38,       // Ascii 118 = 'v'
  0x3c,0x3e,0x06,0x0c,0x06,0x3e,0x3c,       // Ascii 119 = 'w'
  0x22,0x36,0x1c,0x08,0x1c,0x36,0x22,       // Ascii 120 = 'x'
  0x39,0x3d,0x05,0x05,0x05,0x3f,0x3e,       // Ascii 121 = 'y'
  0x00,0x22,0x26,0x2e,0x3a,0x32,0x22,       // Ascii 122 = 'z'
  0x00,0x10,0x10,0x7c,0xee,0x82,0x82,       // Ascii 123 = '{'
  0x00,0x00,0x00,0xee,0xee,0x00,0x00,       // Ascii 124 = '|'
  0x00,0x82,0x82,0xee,0x7c,0x10,0x10,       // Ascii 125 = '}'
  0x00,0x40,0x80,0x80,0x40,0x40,0x80,       // Ascii 126 = '~'
  0x0e,0x1e,0x32,0x62,0x62,0x32,0x1e,       // Ascii 127 = ' '

  0x3c,0x42,0x81,0x81,0x81,0x42,0x3c,       // Ascii 128 = rundes O
  0x3c,0x42,0xbd,0xbd,0xbd,0x42,0x3c,       // Ascii 129 = ausgefuelltes =
  0x00,0x00,0x70,0x88,0x88,0x88,0x70        // Ascii 130 = hochgestelltes o
};

#endif


// Displaybuffer for scrolling
uint8_t DispBuf[NO_OF_ROW][CHARS_PER_ROW];  // 6 Zeilen a 14 Zeichen Nokia, 8 Zeilen a 16 Zeichen OLED

static byte xPos,yPos; // actual Character Position on Display

byte invert=0;
byte scroll=1;
uint8_t diagramActive=0;

void shiftOut(byte data)
{
   byte i;
   for (i=0; i<8; i++)
   {
       if (data&0x80)
          PORT_SDIN->ODR |= PIN_SDIN;
       else
          PORT_SDIN->ODR &= ~PIN_SDIN;

      PORT_SCLK->ODR |= PIN_SCLK;
      PORT_SCLK->ODR &= ~PIN_SCLK;

      data = data << 1;
   }
}


uint8_t shiftIn(void)
{
   byte i, res=0, bit = 0x80;
   for (i=0; i<8; i++)
   {
      PORT_SCLK->ODR |= PIN_SCLK;
      if (PORT_FLASH_DO->IDR & PIN_FLASH_DO)
         res |= bit;
      PORT_SCLK->ODR &= ~PIN_SCLK;
      bit = bit >> 1;
   }
   return res;
}


uint8_t transfer(byte data)
{
   byte i,res=0, bit = 0x80;
   for (i=0; i<8; i++)
   {
       if (data&bit)
          PORT_SDIN->ODR |= PIN_SDIN;
       else
          PORT_SDIN->ODR &= ~PIN_SDIN;

      PORT_SCLK->ODR |= PIN_SCLK;
      if (PORT_FLASH_DO->IDR & PIN_FLASH_DO)
         res |= bit;
      PORT_SCLK->ODR &= ~PIN_SCLK;
      bit = bit >> 1;
   }
   return res;
}


#define WB_WRITE_ENABLE       0x06
#define WB_WRITE_DISABLE      0x04
#define WB_CHIP_ERASE         0xc7
#define WB_SECTOR_ERASE       0x20
#define WB_READ_STATUS_REG_1  0x05
#define WB_READ_DATA          0x03
#define WB_PAGE_PROGRAM       0x02
#define WB_JEDEC_ID           0x9F

void get_jedec_id(byte *b1, byte *b2, byte *b3) {
  PORT_FLASH_CS->ODR |= PIN_FLASH_CS;
  PORT_FLASH_CS->ODR &= ~PIN_FLASH_CS;
  shiftOut(WB_JEDEC_ID);
  *b1 = shiftIn(); // manufacturer id
  *b2 = shiftIn(); // memory type
  *b3 = shiftIn(); // capacity
  PORT_FLASH_CS->ODR |= PIN_FLASH_CS;
}



/*
 * See the timing diagram in section 9.2.10 of the
 * data sheet, "Read Data (03h)".

 Idee:

 Pro 128 Byte:

 CS Display LOW
 Set Display Address to n*128
 CS Display HIGH

 CS Flash LOW
 Write Read Command to Flash  (4 Byte)
 read first Byte from Flash
 CS Display LOW, CD=DATA
 transfer last read Byte to Display while reading next Byte from Flash
 after 128 Bytes read from Flash
 CS Flash HIGH
 Write last Byte to Display
 CS Display HIGH



 */
void display_flash_page(uint16_t page_number)
{
  int i,j, b;

  for (i=0; i<8; i++)
  {
      // Set Display write address
      LcdSetMemoryPtr(i, 0);
      // Display CS should be high now

      // enable Flash
      PORT_FLASH_CS->ODR &= ~PIN_FLASH_CS;
      shiftOut(WB_READ_DATA);
      // Construct the 24-bit address from the 16-bit page
      // number and 0x00, since we will read 256 bytes (one
      // page).
      shiftOut((page_number >> 8) & 0xFF);
      shiftOut((page_number) & 0xFF);
      if (i&1) {
         shiftOut(128);
         page_number++;
      }
      else
         shiftOut(0);

      // read first Byte
      b = shiftIn();

      // set Display to Data Mode
      PORT_DC->ODR |= PIN_DC;
      PORT_SCE->ODR &= ~PIN_SCE; // CS Display LOW

      // read/write 127 Byte
      for (j = 0; j < 127; ++j) {
        b = transfer(b);
      }

      // disable Flash
      PORT_FLASH_CS->ODR |= PIN_FLASH_CS;

      // write last read Byte to Display
      shiftOut(b);
      PORT_SCE->ODR |= PIN_SCE; // CS Display HIGH
  }
}



void showFlashPictPart(uint16_t addr, uint8_t StartRow, uint8_t StartCol)
{


}



void flash_wait_ready()
{
  PORT_FLASH_CS->ODR &= ~PIN_FLASH_CS;
  shiftOut(WB_READ_STATUS_REG_1);
  while (shiftIn() & 1) {};
  PORT_FLASH_CS->ODR |= PIN_FLASH_CS;
}

/*
 * erases an 4K Byte Sector
 */
void flash_erase_sector(uint16_t sectorAddr)
{
  PORT_FLASH_CS->ODR &= ~PIN_FLASH_CS;
  shiftOut(WB_WRITE_ENABLE);
  PORT_FLASH_CS->ODR |= PIN_FLASH_CS;
  PORT_FLASH_CS->ODR &= ~PIN_FLASH_CS;
  shiftOut(WB_SECTOR_ERASE);
  shiftOut((sectorAddr>>8)&0xFF);
  shiftOut((sectorAddr)&0xFF);
  shiftOut(0);
  PORT_FLASH_CS->ODR |= PIN_FLASH_CS;
  flash_wait_ready();
}


void flash_writePageStart(uint16_t page)
{
  PORT_FLASH_CS->ODR &= ~PIN_FLASH_CS;
  shiftOut(WB_WRITE_ENABLE);
  PORT_FLASH_CS->ODR |= PIN_FLASH_CS;
  PORT_FLASH_CS->ODR &= ~PIN_FLASH_CS;
  shiftOut(WB_PAGE_PROGRAM);
  shiftOut((page>>8)&0xFF);
  shiftOut((page)&0xFF);
  shiftOut(0);
}


void flash_writePageEnd(void)
{
  PORT_FLASH_CS->ODR |= PIN_FLASH_CS;
  flash_wait_ready();
}


void flash_fill_page(uint16_t page, uint8_t val)
{
  uint16_t i;
  flash_writePageStart(page);
  for (i=0; i<256; i++)
    shiftOut(val);
 flash_writePageEnd();
}


void LcdWrite(byte dc, byte data)
{
  if (dc)
     PORT_DC->ODR |= PIN_DC;
  else
     PORT_DC->ODR &= ~PIN_DC;
  //digitalWrite(PIN_DC, dc);
  PORT_SCE->ODR &= ~PIN_SCE;
  //digitalWrite(PIN_SCE, LOW);
  shiftOut( data);

  PORT_SCE->ODR |= PIN_SCE;
  //digitalWrite(PIN_SCE, HIGH);
}



void LcdSetMemoryPtr(byte row, byte ofs)
{
#ifdef SSD1306

 // Set page Address
    oled_cmdmode();
    cs_clr();

    spi_out(0xb0 | (NO_OF_ROW-1-row));  // Pageadresse schreiben
    spi_out(0x10 | (ofs >> 4 & 0x0f));    // MSB X-Adresse
    spi_out(ofs & 0x0f    );              // LSB X-Adresse (+Offset)

    cs_set();
    oled_datamode();


 #else
  // column - range: 0 to 84
  // row - range: 0 to 5
  LcdWrite( LCD_C, 0x80 | ofs);
  LcdWrite( LCD_C, 0x40 | row);
 #endif // SSD1306

}



void LcdSetCharPos(byte row, byte column)
{
  LcdSetMemoryPtr(row, column*DISP_CHAR_SIZE_X);
  xPos=column;
  yPos=row;
}


void LcdSetScroll(byte s)
{
    scroll = s;
}


void LcdSetInvert(byte i)
{
    invert = i;
}



void LcdSetDiagram(byte d)
{
    diagramActive = d;
    if (d)
      startTimer(DIA_UPDATE_TIME);
}


void LcdSetLight(byte l)
{
 #ifndef SSD1306
    if (l)
       PORT_LED->ODR |= PIN_LED;
    else
       PORT_LED->ODR &= ~PIN_LED;
 #endif
}


void LcdClear(void)
{
  int index, row;
  if (diagramActive==0)
  {
     for (row=0; row < LCD_Y/8; row++) {
         LcdSetCharPos(row, 0);
         for (index = 0; index < LCD_X; index++)
         {
             LcdWrite(LCD_D, 0x00);
         }
     }
  }
  else
  {
     for (row=3; row < LCD_Y/8; row++) {
         LcdSetCharPos(row, 0);
         for (index = 0; index < LCD_X; index++)
         {
             LcdWrite(LCD_D, 0x00);
         }
     }
  }

   for (yPos=0; yPos<NO_OF_ROW; yPos++)
   {
       for (xPos=0; xPos<CHARS_PER_ROW; xPos++)
         DispBuf[yPos][xPos]=' ';
   }

   if (diagramActive==0)
      LcdSetCharPos(0,0);
   else
      LcdSetCharPos(3,0);
}



void LcdCharacter(char character)
{
  byte index;

  if ((character == '\n') /*|| (character == '\r')*/)
  {
     for (xPos; xPos<CHARS_PER_ROW; xPos++)
     {
        DispBuf[yPos][xPos] = ' ';
        if (invert)
           DispBuf[yPos][xPos] |= INVERT_BIT;
        for (index = 0; index < DISP_CHAR_SIZE_X; index++)
        {
           if (invert)
              LcdWrite(LCD_D,  0xFF);
           else
              LcdWrite(LCD_D,  0x00);
        }
     }
  }
  else
  {
      if ((character < 0x20) || (character > (sizeof(ASCII)/(DISP_CHAR_SIZE_X-1) + 0x20) ))
      {
         return;
      }
      else
      {
         if (invert)
           LcdWrite(LCD_D, 0xFF);
         else
           LcdWrite(LCD_D, 0x00);
         for (index = 0; index < DISP_CHAR_SIZE_X-1; index++)
         {
           //LcdWrite(LCD_D, ASCII[(character - 0x20)*5 + index]);
           if (invert)
             LcdWrite(LCD_D, ~ASCII[(character - 0x20)*(DISP_CHAR_SIZE_X-1)  + index]);
           else
             LcdWrite(LCD_D,  ASCII[(character - 0x20)*(DISP_CHAR_SIZE_X-1)  + index]);
         }

         DispBuf[yPos][xPos]=character;
         if (invert)
           DispBuf[yPos][xPos] |= INVERT_BIT;
         xPos++;
      }
  }

  // DisplayBuffer handling
  if (xPos == CHARS_PER_ROW)
  {
     xPos = 0;
     yPos++;
     #ifdef SSD1306
     if (yPos<NO_OF_ROW)
        LcdSetCharPos(yPos,xPos);
     #endif // SSD1306
     if (yPos==NO_OF_ROW)
     {
        if (scroll==0) // no scrolling -> set writepointer to first address
        {
           if (diagramActive==0)
              LcdSetCharPos(0,0);
           else
              LcdSetCharPos(3,0);
        }
        else
        {
           // Scroll Display Content
           byte x,y;
           if (diagramActive)
           {
#ifndef SSD1306
              LcdSetCharPos(3,0);
#endif
              y = 3;
           }
           else
           {
#ifndef SSD1306
              LcdSetCharPos(0,0);
#endif
              y = 0;
           }
           for (y; y<NO_OF_ROW-1; y++)
           {
             #ifdef SSD1306
             LcdSetCharPos(y,0);
             #endif // SSD1306

              for (x=0; x<CHARS_PER_ROW; x++)
              {
                 uint16_t c =  DispBuf[y+1][x];
                 DispBuf[y][x] = c;

                 if (c&INVERT_BIT)
                   LcdWrite(LCD_D, 0xFF);
                 else
                   LcdWrite(LCD_D, 0x00);
                 for (index = 0; index < (DISP_CHAR_SIZE_X-1); index++)
                 {
                   if (c&INVERT_BIT)
                     LcdWrite(LCD_D, ~ASCII[((c&0x7F) - 0x20)*(DISP_CHAR_SIZE_X-1)  + index]);
                   else
                     LcdWrite(LCD_D,  ASCII[((c&0x7F) - 0x20)*(DISP_CHAR_SIZE_X-1)  + index]);
                 }
              }
           }
           #ifdef SSD1306
           LcdSetCharPos(y,0);
           #endif // SSD1306
           for (x=0; x<CHARS_PER_ROW; x++)
           {
              DispBuf[y][x] = ' ';
              if (invert)
                 DispBuf[y][x] |= INVERT_BIT;
              for (index = 0; index < DISP_CHAR_SIZE_X; index++)
              {
                 if (invert)
                    LcdWrite(LCD_D,  0xFF);
                 else
                    LcdWrite(LCD_D,  0);

              }
           }
           yPos=NO_OF_ROW-1;
           LcdSetCharPos(NO_OF_ROW-1,0);
        }
      }
   }
}



void LcdString(char *characters)
{
  while (*characters)
  {
     LcdCharacter(*characters++);
  }
}


void LcdInvertLine(uint8_t row)
{
   uint8_t i;
   LcdSetCharPos(row,0);
   for (i=0; i<CHARS_PER_ROW; i++)
   {
      uint8_t index;
      uint8_t c;
      DispBuf[row][i] ^= INVERT_BIT;
      c = DispBuf[row][i];
      if (c&INVERT_BIT)
        LcdWrite(LCD_D, 0xFF);
      else
        LcdWrite(LCD_D, 0x00);
      for (index = 0; index < (DISP_CHAR_SIZE_X-1); index++)
      {
        if (c&INVERT_BIT)
          LcdWrite(LCD_D, ~ASCII[((c&0x7F) - 0x20)*(DISP_CHAR_SIZE_X-1)  + index]);
        else
          LcdWrite(LCD_D,  ASCII[((c&0x7F) - 0x20)*(DISP_CHAR_SIZE_X-1)  + index]);
      }
   }
   LcdSetCharPos(row,0);
}


#define enable_output(port, pin) { port->DDR |= pin; port->CR1 |= pin; port->CR2 &= ~pin; }



void LcdInitialise(void)
{

  #ifdef SSD1306

  enable_output(PORT_SDIN,  PIN_SDIN);
  enable_output(PORT_SCE,   PIN_SCE);
  enable_output(PORT_RESET, PIN_RESET);
  enable_output(PORT_DC,    PIN_DC);
  enable_output(PORT_SCLK,  PIN_SCLK);
  enable_output(PORT_FLASH_CS,  PIN_FLASH_CS);

  rst_set();
  delay(1);

  oled_enable();   // Enable Dipslay
  delay(10);
  rst_clr();
  delay(10);
  rst_set();

  oled_cmdmode();

  shiftOut( 0x8d);           // Ladungspumpe an
  shiftOut( 0x14);
  shiftOut( 0xaf);           // Display on
  delay(150);
  shiftOut( 0xa1);           // Segment Map
  shiftOut( 0xc0);           // Direction Map

  oled_datamode();

  LcdSetCharPos(0,0);

  #else  // PCD8544

  enable_output(PORT_SCE,   PIN_SCE);
  enable_output(PORT_RESET, PIN_RESET);
  enable_output(PORT_DC,    PIN_DC);
  enable_output(PORT_SDIN,  PIN_SDIN);
  enable_output(PORT_SCLK,  PIN_SCLK);
  enable_output(PORT_LED,   PIN_LED);

  PORT_RESET->ODR &= ~PIN_RESET;
  PORT_RESET->ODR |=  PIN_RESET;

  LcdWrite(LCD_C, 0x21 );  // LCD Extended Commands.
  LcdWrite(LCD_C, 0xB2 );  // Set LCD Vop (Contrast).
  LcdWrite(LCD_C, 0x04 );  // Set Temp coefficent. //0x04
  LcdWrite(LCD_C, 0x15 );  // LCD bias mode 1:48. //0x13
  LcdWrite(LCD_C, 0x20 );  // LCD Basic Commands
  LcdWrite(LCD_C, 0x0C );  // LCD in normal mode.

  #endif // SSD1306

}


//---------------------------------------------------------------------
// Empfangene Werte als Diagramm darstellen
//---------------------------------------------------------------------


uint8_t diaData[3][LCD_X];
uint8_t diaPos;
int     maxVal=100; // min is 0

uint16_t valuesAddedSinceLastUpdate;


static void LcdEnterDiaValue(uint8_t pos, uint8_t val)
{
   // - Es stehen 24 Pixel in Y-Richtung zur Verfügung
   // - es muss ermittelt werden, welches Pixel in der Spalte aktiviert werden muss
   int pix;

   if (val>maxVal)
      val = maxVal;

   pix = (24*(int)val)/maxVal;

   diaData[0][pos]= 0;
   diaData[1][pos]= 0;
   diaData[2][pos]= 0;

   if (pix < 8)
   {
       diaData[2][pos] = 1<<(7-pix);
   }
   else if (pix<16)
   {
       pix -= 8;
       diaData[1][pos] = 1<<(7-pix);
   }
   else
   {
       pix-= 16;
       diaData[0][pos] = 1<<(7-pix);
   }

   valuesAddedSinceLastUpdate++;
}

static void LcdDiaWriteData(void)
{
    uint8_t i,j;
    #ifndef SSD1306
    LcdWrite( 0, 0x80 );
    LcdWrite( 0, 0x40 );
    #endif
    for (i=0; i<3;i++) {
      #ifdef SSD1306
      LcdSetCharPos(i,0);
      #endif // SSD1306

      for (j=0; j<LCD_X;j++)
          LcdWrite(LCD_D, diaData[i][j]);
    }
}

void LcdAddDiaValue(uint8_t v)
{
    uint8_t i,j;

    if (diagramActive==0)
        return;

    if (diaPos >= LCD_X-1)
    {
        // erstmal alle Daten scrollen
        for (i=0; i<3;i++)
          for (j=0; j<LCD_X-1;j++)
             diaData[i][j] = diaData[i][j+1];
        diaPos--;
    }
    LcdEnterDiaValue(diaPos++, v);
    LcdDiaUpdate();
}


void LcdSetDiaMaxVal(int v)
{
   maxVal = v;
}

void LcdDiaClear(void)
{
   uint8_t i,j;


   for (i=0; i<3;i++)
     for (j=0; j<LCD_X;j++)
         diaData[i][j] = 0;
   diaPos = 0;

   valuesAddedSinceLastUpdate=0;

   if (diagramActive==0)
      return;
   LcdDiaWriteData();
}


void LcdAddGraphicData(uint8_t b1, uint8_t b2, uint8_t b3)
{
    uint8_t i,j;

    if (diagramActive==0)
      return;

    if (diaPos >= LCD_X-1)
    {
        // erstmal alle Daten scrollen
        for (i=0; i<3;i++)
          for (j=0; j<LCD_X-1;j++)
             diaData[i][j] = diaData[i][j+1];
        diaPos--;
    }

    diaData[0][diaPos] = b1;
    diaData[1][diaPos] = b2;
    diaData[2][diaPos++] = b3;
    valuesAddedSinceLastUpdate++;

    LcdDiaUpdate();
}


void LcdDiaUpdate(void)
{
   if (checkTimer() == 0)
   {
       startTimer(DIA_UPDATE_TIME);
       if (valuesAddedSinceLastUpdate)
       {
           LcdDiaWriteData();
           valuesAddedSinceLastUpdate=0;
       }
   }
}

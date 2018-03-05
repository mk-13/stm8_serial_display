// core header file from our library project:
#include "stm8s.h"
#include "lcd.h"
#include "uart.h"
#include "i2cSlave.h"
#include "timebase.h"
#include "lcd_cmd.h"
#include "adc.h"
#include "eeprom.h"


//#define LED_PIN (GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7)
#define LED_PIN GPIO_PIN_5

#define STATE_IDLE            0
#define STATE_SET_CURSOR      1
#define STATE_DIAGRAM_MAX     2
#define STATE_DIAGRAM_VAL     3
#define STATE_GRAPHIC_DATA    4
#define STATE_CONFIG_DATA     5
#define STATE_SET_FUNCTION    6
#define STATE_VARIABLE        7
#define STATE_MONITOR         8
#define STATE_MONITOR_DIAGRAM  9
#define STATE_INVERT_LINE      10
//#define STATE_GRAPHIC_CMD      11
#define STATE_GRAPHIC_SUB_CMD    12
#define STATE_GRAPHIC_CMD_HEADER 13
#define STATE_GRAPHIC_CMD_DATA   14
#define STATE_GRAPHIC_CMD_DISPLAY_PART 15
#define STATE_FLASH_WRITE_PAGE   16


#define U1  4   // Mit Spannungsteiler
#define U2  3   //   "
#define U3  2   // Ohne Spannungteiler
#define U4  5   //   "

uint16_t byteCount;
uint16_t recDataCount;
uint8_t gdBytes[5];
uint8_t StartRow, StartCol, height, width;
uint16_t PictId;


uint8_t baudRate = BAUD_115200;
uint8_t i2cAdr   = 0x37;
uint8_t onOff;
uint8_t monitorTimeout;
uint8_t monitorChannels;
uint8_t monitorDiagram;
uint8_t monitorAktiv;

uint8_t state = STATE_IDLE;

uint8_t lastCommand;

uint8_t GraphicSubCmd;



#define SW_VERSION "1.0.1"


void ShowNibble(uint8_t n)
{
  if (n < 10)
    LcdCharacter(0x30 + n);
  else
    LcdCharacter('A' + n - 10);
}


void ShowHex(uint8_t b)
{
   ShowNibble(b>>4);
   ShowNibble(b&0x0F);
}

void ShowDez(unsigned int n)
{
  char buf[5 + 1]; // max. 5 Stellen
  char *str = &buf[sizeof(buf) - 1];
  int base = 10;


  *str = '\0';

  do {
    unsigned int m = n;
    char c;
    n /= base;
    c = m - base * n;
    *--str = c < 10 ? c + '0' : c + 'A' - 10;
  } while(n);

  //UartPuts(str);

  LcdString(str);
}


//#define MAP_TO_MAX  1427 // Volt*100  6,8k/2,2K
#define MAP_TO_MAX  1286 // Volt*100  6,2k/2,1K

int MapMax = MAP_TO_MAX;


#define PSTR(x) x
#define LcdString_P LcdString


long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


uint16_t ShowAnalog(uint8_t id, uint8_t mapVal)
{
  int v = analogRead(id);
  if (mapVal)
  {
    v = (int) map(v,0,1023, 0, MapMax);
    ShowDez(v/100);
    LcdCharacter('.');
    ShowDez(v%100);
  }
  else
  {
    ShowDez(v);
  }
  return v;
}


const uint8_t analogIds[] =
{
    U1,
    U2,
    U3,
    U4
};

/*
 *
 */
void monitorUpdate()
{
   if (checkMonitorTimer() == 0)
   {
      LcdSetCharPos(0,0);
      if (monitorChannels & 1)
      {
          LcdString("U1 ");
          ShowAnalog(U1, 1);
          LcdString(" V\n");
      }
      if (monitorChannels & 2)
      {
          LcdString("U2 ");
          ShowAnalog(U2, 1);
          LcdString(" V\n");
      }
      if (monitorChannels & 4)
      {
          LcdString("U3 ");
          ShowAnalog(U3, 0);
          LcdString("\n");
      }
      if (monitorChannels & 8)
      {
          LcdString("U4 ");
          ShowAnalog(U4, 0);
          LcdString("\n");
      }
      startMonitorTimer(monitorTimeout*100);
   }
}


/*
 * den in monitorChannels gepseicherten Kanal auslesen und den
 * Wert dem Diagramm hinzufügen.
 * Min/Max Werte für das Diagramm müssen von außen gesetzt werden!
 */
void monitorDiagramUpdate()
{
    if (checkMonitorTimer() == 0)
    {
        uint16_t v;
        LcdSetCharPos(4,0);
        switch (monitorChannels)
        {
        case 1:
        case 2:
            v  = ShowAnalog(analogIds[monitorChannels-1], 1);
            LcdString(" V\n");
            LcdAddDiaValue(v >> 2);
            break;
        case 3:
        case 4:
            v  = ShowAnalog(analogIds[monitorChannels-1], 0);
            LcdString("\n");
            LcdAddDiaValue(v >> 2);
            break;
        }
        startMonitorTimer(monitorTimeout*100);
    }
}



void ShowInitValues()
{
  uint8_t b1,b2,b3;

  LcdClear();

#if 0
  LcdString_P(PSTR("12345678901234567890\n"));
  LcdString_P(PSTR("row2 \n"));
  LcdString_P(PSTR("row3 \n"));
  LcdString_P(PSTR("row4 \n"));
  LcdString_P(PSTR("row5 \n"));
  LcdString_P(PSTR("row6 \n"));
  //LcdString_P(PSTR("row7 \n"));
  //LcdString_P(PSTR("row8 \n"));
  //LcdString_P(PSTR("row9 "));

  return;
#endif

  get_jedec_id(&b1, &b2, &b3);


  LcdString_P(PSTR("SW-Ver "));
  LcdString_P(PSTR(SW_VERSION));
  LcdString_P(PSTR("\nUART   "));

  switch (baudRate) {
  case 1:
    LcdString_P(PSTR("9600\n"));
      break;
    case 2:
      LcdString_P(PSTR("38400\n"));
      break;
    case 3:
      LcdString_P(PSTR("57600\n"));
      break;
    default:
      LcdString_P(PSTR("115200\n"));
      break;
  }

#ifdef USE_I2C
  LcdString_P(PSTR("I2C    0x"));
  ShowHex(i2cAdr);
#endif

  LcdString_P(PSTR("\nFlashId "));
  ShowHex(b1);
  ShowHex(b2);
  ShowHex(b3);


  LcdString_P(PSTR("\nU1 "));
  ShowAnalog(U1,1);
  LcdString_P(PSTR(" V\n"));

  LcdString_P(PSTR("U2 "));
  ShowAnalog(U2,1);
  LcdString_P(PSTR(" V"));

}


void processConfigData()
{
   int cs = CMD_CONFIG;
   cs += gdBytes[0];
   cs += gdBytes[1];

   if ((uint8_t)cs == gdBytes[2])
   {
      if (gdBytes[1] != EepromRead(gdBytes[0]))
      {
          LcdString_P(PSTR("cfg "));
          ShowDez(gdBytes[0]);
          LcdString(" 0x");
          ShowHex(gdBytes[1]);
          LcdCharacter('\n');

          if (gdBytes[0] == CFG_ITEM_VMAP_LOW) {
             MapMax = (EepromRead(CFG_ITEM_VMAP_HIGH)<<8)| gdBytes[1];
             LcdString_P(PSTR("\nU1 "));
             ShowAnalog(U1,1);
             LcdString_P(PSTR(" V\n"));
          }

          EepromWrite(gdBytes[0], gdBytes[1]);
      }
   }
}

int main(void)
{
    //uint8_t i,j;
    uint8_t value, value1;
    // Reset GPIO ports to default state
  //  GPIO_DeInit(GPIOA);
  //  GPIO_DeInit(GPIOB);
  //  GPIO_DeInit(GPIOC);
  //  GPIO_DeInit(GPIOD);
    // Set operation mode for port D / pin 0
    // (connected to the onboard LED)
//    GPIO_Init(GPIOB, LED_PIN, GPIO_MODE_OUT_PP_LOW_FAST);

    value = EepromRead(CFG_ITEM_BAUD_RATE);
    if ((value > 0) && (value <5))
       baudRate = value;

    value = EepromRead(CFG_ITEM_I2C_ADDR);
    if ((value > 0) && (value < 127))
       i2cAdr = value;

    value = EepromRead(CFG_ITEM_VMAP_HIGH);
    value1 = EepromRead(CFG_ITEM_VMAP_LOW);

    if (value<128)
       MapMax = (value<<8)| value1;

    UartInit(baudRate);

    TimebaseInit();

    LcdInitialise();

#ifdef USE_I2C
    i2cSlaveInit(i2cAdr);
#endif
    ADCInit();

    //UartPuts("Hello World\r\n");

    ShowInitValues();

#if 0
    delay(3000);
    //flash_erase_sector(0);
    //flash_fill_page(0, 0x11);
    display_flash_page(0);

    delay(3000);

    ShowInitValues();
#endif
    while (1)
    {
        if (RxBufRp != RxBufWp)
        {
            uint8_t b = Buffer_Rx[RxBufRp++];
            RxBufRp &= (RX_BUF_SIZE-1);

            switch (state)
            {
            case STATE_IDLE:
                if ((b & 0xF0)  == 0xF0) // command
                {
                    if (b == CMD_CLEAR_DISPLAY)
                    {
                       LcdClear();
                    }
                    else
                    {
                        switch (b&0xFE)
                        {
                        case CMD_SET_CURSOR:
                            state = STATE_SET_CURSOR; // next Byte contains position
                            break;
                        case CMD_SET_INVERT:
                            LcdSetInvert(b&0x01);
                            break;
                        case CMD_SET_FUNCTION:
                            state = STATE_SET_FUNCTION;
                            onOff = b&0x01;
                            break;
                        case CMD_CONFIG:
                            if (b & 1) // CMD_SHOW_CONFIG
                            {
                               ShowInitValues();
                            }
                            else {
                               state = STATE_CONFIG_DATA;
                               byteCount = 0;
                            }
                            break;
                        case CMD_SET_DIAGRAM_VAL:
                            if (b & 0x01) // CMD_SET_DIAGRAM_MAX:
                               state = STATE_DIAGRAM_MAX; // next Byte contains max y Value for diagram
                            else
                               state = STATE_DIAGRAM_VAL; // next Byte contains diagram value
                            break;
                        case CMD_SET_GRAFIK_BYTES:
                            if (b & 0x01) {
                                // FIXME: CMD_CLEAR_DIAGRAM implementation missing
                            }
                            else {
                                state = STATE_GRAPHIC_DATA;
                                byteCount = 0;
                            }
                            break;
                        case CMD_GRAPHIC:
                            if ((b & 0x01)==0) {
                               byteCount=0;
                               state = STATE_GRAPHIC_SUB_CMD;
                            }
                            break;
                        case CMD_INVERT_LINE:
                            if ((b & 1) == 0)
                                state = STATE_INVERT_LINE;
                            else // CMD_RESET
                            {
                                if (lastCommand != b)
                                {
                                    monitorDiagram=0;
                                    monitorAktiv=0;
                                    LcdSetScroll(1);
                                    LcdSetInvert(0);
                                    LcdSetLight(0);
                                    LcdSetDiagram(0);
                                    LcdDiaClear();
                                    ShowInitValues();
                                }
                            }
                            break;
                        }
                    }
                    lastCommand = b;
                }
                else
                {
                  if (b == CMD_VARIABLE) // '$'
                  {
                      state = STATE_VARIABLE;
                  }
                  else
                  {
                     //if (b=='o')
                     //   GPIOD->ODR ^= GPIO_PIN_4;

                     LcdCharacter(b);
                  }
                }
                break;
            case STATE_SET_FUNCTION:
                if (b & MONITOR)
                {
                    if (onOff)
                    {
                       state = STATE_MONITOR;
                       break;
                    }
                    else
                       monitorAktiv  = 0;
                }
                if (b & MONITOR_DIAGRAM)
                {
                    LcdSetDiagram(onOff);
                    if (onOff)
                    {
                       state = STATE_MONITOR_DIAGRAM;
                       break;
                    }
                    else
                    {
                       monitorDiagram = 0;
                    }
                }
                if (b & SCROLL)
                   LcdSetScroll(onOff);
                if (b & LIGHT)
                   LcdSetLight(onOff);
                if (b & DIAGRAM)
                   LcdSetDiagram(onOff);
                state = STATE_IDLE;
                break;
            case STATE_SET_CURSOR:
                LcdSetCharPos(b>>4, b&0x0F);
                state = STATE_IDLE;
                break;
            case STATE_DIAGRAM_MAX:
                state = STATE_IDLE;
                LcdSetDiaMaxVal(b);
                break;
            case STATE_DIAGRAM_VAL:
                state = STATE_IDLE;
                LcdAddDiaValue(b);
                break;
            case STATE_GRAPHIC_DATA:
                gdBytes[byteCount++] = b;
                if (byteCount == 3)
                {
                   LcdAddGraphicData(gdBytes[0], gdBytes[1], gdBytes[2]);
                   state = STATE_IDLE;
                }
                break;
            case STATE_CONFIG_DATA:
                gdBytes[byteCount++] = b;
                if (byteCount == 3)
                {
                  processConfigData();
                  state = STATE_IDLE;
                }
                break;
            case STATE_VARIABLE:
                switch (b)
                {
                case '1':
                    ShowAnalog(U1,1);
                    break;
                case '2':
                    ShowAnalog(U2,1);
                    break;
                case '3':
                    ShowAnalog(U3,0);
                    break;
                case '4':
                    ShowAnalog(U4,0);
                    break;
                default:
                    LcdCharacter('$');
                    LcdCharacter(b);
                }
                state = STATE_IDLE;
                break;
            case STATE_MONITOR:
                state = STATE_IDLE;
                monitorTimeout = b>>4;
                monitorChannels = b&0x0F;
                monitorAktiv = 1;
                LcdClear();
                startMonitorTimer(monitorTimeout*100);
                break;
            case STATE_MONITOR_DIAGRAM:
                state = STATE_IDLE;
                monitorTimeout = b>>4;
                monitorChannels = b&0x0F;
                monitorDiagram = 1;
                LcdClear();
                LcdDiaClear();
                startMonitorTimer(monitorTimeout*100);
                break;
            case STATE_INVERT_LINE:
                state  = STATE_IDLE;
                LcdInvertLine(b);
                break;
            //case STATE_GRAPHIC_CMD:
            //    state = STATE_GRAPHIC_SUB_CMD;
            //    break;
            case STATE_GRAPHIC_SUB_CMD:
                GraphicSubCmd = b;
                recDataCount = 0;
                switch (GraphicSubCmd) {
                   case SUB_CMD_DISPLAY_GRAPHIC_FULL:
#ifdef SSD1306
                      byteCount = 1024;
#else
                      byteCount = 504; //84*6
#endif
                      LcdSetMemoryPtr(0, 0);
                      state = STATE_GRAPHIC_CMD_DATA;
                      break;
                   case SUB_CMD_DISPLAY_GRAPHIC_PART:
                      byteCount = 4;
                      state = STATE_GRAPHIC_CMD_HEADER;
                      break;
                   case SUB_CMD_FLASH_GRAPHIC_FULL:
                      byteCount = 2;
                      state = STATE_GRAPHIC_CMD_HEADER;
                      break;
                   case SUB_CMD_FLASH_GRAPHIC_PART:
                      byteCount = 4;
                      state = STATE_GRAPHIC_CMD_HEADER;
                      break;
                   case SUB_CMD_FLASH_ERASE_SECTOR:
                      byteCount = 3;
                      state = STATE_GRAPHIC_CMD_HEADER;
                      break;
                   case SUB_CMD_FLASH_WRITE_PAGE:
                      byteCount = 4;
                      state = STATE_GRAPHIC_CMD_HEADER;
                      break;
                   default:
                     state  = STATE_IDLE;
                }
                break;
            case STATE_GRAPHIC_CMD_HEADER:
                gdBytes[recDataCount++] = b;
                if (recDataCount == byteCount) {
                   switch (GraphicSubCmd) {
                      case SUB_CMD_DISPLAY_GRAPHIC_PART:
                         height = gdBytes[0];
                         width  = gdBytes[1];
                         StartRow = gdBytes[2];
                         StartCol = gdBytes[3];
                         LcdSetMemoryPtr(StartRow, StartCol);
                         byteCount = height*width;
                         state = STATE_GRAPHIC_CMD_DISPLAY_PART;
                         break;
                      case SUB_CMD_FLASH_GRAPHIC_FULL:
                         PictId = (gdBytes[0]<<8|gdBytes[1]);
                         if (PictId < 1024)
                            display_flash_page(PictId<<2); // one Pict 1024 Byte
                         state = STATE_IDLE;
                         break;
                      case SUB_CMD_FLASH_GRAPHIC_PART:
                         PictId = (gdBytes[0]<<8|gdBytes[1]);
                         if (PictId < 1024) {
                            showFlashPictPart(PictId<<2, gdBytes[2], gdBytes[3]); // Pict Size first 2 Bytes in Flash
                         }
                         state = STATE_IDLE;
                         break;
                      case SUB_CMD_FLASH_ERASE_SECTOR:
                         {
                            int cs = GraphicSubCmd;
                            cs+=gdBytes[0];
                            cs+=gdBytes[1];
                            if ((uint8_t)cs == gdBytes[2]) {
                               LcdClear();
                               LcdString("erase ");
                               ShowHex(gdBytes[0]);
                               ShowHex(gdBytes[1]);
                               LcdString("..");
                               flash_erase_sector((gdBytes[0]<<8)|gdBytes[1]);
                               LcdString("done\n");
                            }
                            state = STATE_IDLE;
                         }
                         break;
                      case SUB_CMD_FLASH_WRITE_PAGE:
                         {
                            int cs = GraphicSubCmd;
                            cs+=gdBytes[0]; // addr high
                            cs+=gdBytes[1]; // addr low
                            cs+=gdBytes[2]; // len -1
                            if ((cs&0xFF) == gdBytes[3])
                            {
                               LcdString("wr page ");
                               ShowHex(gdBytes[0]);
                               ShowHex(gdBytes[1]);
                               LcdString("\n");
                               byteCount = gdBytes[2]+1;
                               flash_writePageStart(gdBytes[0]<<8|gdBytes[1]);
                               state = STATE_FLASH_WRITE_PAGE;
                            }
                            else
                            {
                               state = STATE_IDLE;
                               LcdString("csewp");
                               ShowHex(gdBytes[2]);
                               ShowHex(gdBytes[3]);
                               ShowHex(cs);
                               LcdString("\n");
                            }
                         }
                         break;
                      default:
                        state  = STATE_IDLE;
                   }
                }
                break;
            case STATE_GRAPHIC_CMD_DATA:
                // write data byte to Display
                LcdWrite(LCD_D, b);
                byteCount--;
#ifdef SSD1306
                recDataCount++;
                if ((recDataCount&0x7F) == 0)
                   LcdSetMemoryPtr(recDataCount>>7, 0);
#endif
                if (byteCount==0)
                    state = STATE_IDLE;
                break;
            case STATE_GRAPHIC_CMD_DISPLAY_PART:
               recDataCount++;
               byteCount--;
               // write data byte to Display
               LcdWrite(LCD_D, b);
               if (byteCount == 0)
                    state = STATE_IDLE;
               if ((recDataCount%width) == 0)
                  LcdSetMemoryPtr(StartRow+(recDataCount/width), StartCol);
               break;
            case STATE_FLASH_WRITE_PAGE:
               byteCount--;
               shiftOut(b);
               if (byteCount == 0) {
                  flash_writePageEnd();
                  state = STATE_IDLE;
               }
               break;
           }
        } // if character received

        if (diagramActive)
           LcdDiaUpdate();

        if (monitorAktiv)
           monitorUpdate();

        if (monitorDiagram)
            monitorDiagramUpdate();

    } // while 1
}


 INTERRUPT_HANDLER(UART1_RX_IRQHandler, 18)
{
    GPIOB->ODR ^= LED_PIN;
    /* Read one byte from the receive data register */
    Buffer_Rx[RxBufWp++] = UART1->DR;
    RxBufWp &= (RX_BUF_SIZE-1);

}


extern uint16_t  waitTime;
extern uint16_t  timerTime;
extern uint16_t  monitorTime;


INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23)
{
   if (waitTime)
      waitTime--;
   if (timerTime)
      timerTime--;
   if (monitorTime)
      monitorTime--;

    /* Clear Interrupt Pending bit */
  //TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
  TIM4->SR1 = (uint8_t)(~(TIM4_IT_UPDATE));

}


#ifdef USE_I2C
__IO uint16_t Event = 0x00;


/**
  * @brief  I2C Interrupt routine
  * @param None
  * @retval
  * None
  */
INTERRUPT_HANDLER(I2C_IRQHandler, 19)
{
  /* Read SR2 register to get I2C error */
  if ((I2C->SR2) != 0)
  {
    /* Clears SR2 register */
    I2C->SR2 = 0;
  }

  Event = I2C_GetLastEvent();
  switch (Event)
  {
      /******* Slave transmitter ******/
      /* check on EV1 */
    case I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED:
      //Tx_Idx = 0;
      break;

      /* check on EV3 */
    case I2C_EVENT_SLAVE_BYTE_TRANSMITTING:
      /* Transmit data */
      //I2C_SendData(Slave_Buffer_Rx[Tx_Idx++]);
      break;
      /******* Slave receiver **********/
      /* check on EV1*/
    case I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED:
      break;

      /* Check on EV2*/
    case I2C_EVENT_SLAVE_BYTE_RECEIVED:
      Buffer_Rx[RxBufWp++] = I2C_ReceiveData();
      RxBufWp &= (RX_BUF_SIZE-1);
      break;

      /* Check on EV4 */
    case (I2C_EVENT_SLAVE_STOP_DETECTED):
            /* write to CR2 to clear STOPF flag */
            I2C->CR2 |= I2C_CR2_ACK;
      break;

    default:
      break;
  }

}
#endif

// Testcode
#if 0
    while (1)
    {
        int v = analogRead(3);
        ShowDez(v);
          UartPuts(" \t ");
          v= analogRead(4);
        ShowDez(v);
            UartPuts("\r\n");
        delay(300);
    }
#endif

#if 0
    LcdClear();


    LcdSetDiagram(1);

    LcdSetCharPos(4,1);
    LcdString("Diagramm Demo");

    #define ANZ 20
    #define DLAY 63
    #define UPD_INTERVALL 10
    for (j=0; j<4; j++)
    {
      for (i=0; i<ANZ; i++)
      {
        LcdAddDiaValue(i*100/ANZ);
        if (RxBufRp != RxBufWp)
            goto MAINLOOP;

        delay(DLAY);
      }
      for (i=ANZ; i>0; i--)
      {
        LcdAddDiaValue(i*100/ANZ);
        if (RxBufRp != RxBufWp)
            goto MAINLOOP;
        delay(DLAY);
      }
    }

    delay(100);
    LcdDiaUpdate();
    delay(100);
    LcdDiaUpdate();
    delay(100);
    LcdDiaUpdate();
    delay(500);

    if (RxBufRp != RxBufWp)
      goto MAINLOOP;



    LcdClear();
    LcdSetLight(1);
    LcdString("Hello World\n Zeile2\n  Zeile3");
    delay(500);
    LcdString("\n   Zeile4");
    delay(500);
    LcdString("\n    Zeile5");
    delay(500);
    LcdString("\n   Zeile6");
    delay(500);
    LcdString("\n  Zeile7");
    delay(1000);
    LcdSetLight(0);

    if (RxBufRp != RxBufWp)
      goto MAINLOOP;

    LcdSetDiagram(0);
    LcdClear();

    LcdString("Hello World\n Zeile2\n  Zeile3");
    delay(500);
    LcdString("\n   Zeile4");
    delay(500);
    LcdString("\n    ");
    LcdSetInvert(1);
    LcdString("Zeile5");
    LcdSetInvert(0);
    delay(500);
    LcdString("\n   Zeile6");
    delay(500);
    LcdString("\n  Zeile7");

    if (RxBufRp != RxBufWp)
      goto MAINLOOP;

    //------------
    delay(1000);
    LcdSetScroll(0);
    LcdClear();

    LcdString("Hello World\n Zeile2\n  Zeile3\n");
    delay(500);
    LcdString("   Zeile4\n");
    delay(500);
    LcdString("    ");
    LcdSetInvert(1);
    LcdString("Zeile5");
    LcdSetInvert(0);
    LcdString("\n");
    delay(500);
    LcdString("   Zeile6\n");
    delay(500);
    LcdString("  Zeile7\n");


#if 0
    // The main loop
    while(1)
    {
        delay(500);
        // Toggle the output pin
        GPIOB->ODR ^= LED_PIN;
    }
#endif


MAINLOOP:
#endif

#define NO_SPL
#ifdef NO_SPL
    #define CLK_DIVR	(*(volatile uint8_t *)0x50c6)
    #define CLK_PCKENR1	(*(volatile uint8_t *)0x50c7)

    #define UART1_SR	(*(volatile uint8_t *)0x5230)
    #define UART1_DR	(*(volatile uint8_t *)0x5231)
    #define UART1_BRR1	(*(volatile uint8_t *)0x5232)
    #define UART1_BRR2	(*(volatile uint8_t *)0x5233)
    #define UART1_CR2	(*(volatile uint8_t *)0x5235)
    #define UART1_CR3	(*(volatile uint8_t *)0x5236)

    #define UART_CR2_TEN (1 << 3)
    #define UART_CR3_STOP2 (1 << 5)
    #define UART_CR3_STOP1 (1 << 4)
    #define UART_SR_TXE (1 << 7)
#endif
  // core header file from our library project:
  #include "stm8s.h"
  #include "uart.h"


void UartInit(uint8_t baud)
{
#ifdef NO_SPL
    CLK_DIVR = 0x00; // Set the frequency to 16 MHz
	CLK_PCKENR1 = 0xFF; // Enable peripherals

	UART1_CR2 = UART_CR2_TEN; // Allow TX and RX
	UART1_CR3 &= ~(UART_CR3_STOP1 | UART_CR3_STOP2); // 1 stop bit

	/*
	   Baud rate calculation:

	   div = 16 000 000 / br;

	   div ->  hex 0xWXYZ
	   BRR2 = 0xWZ
	   BRR1 = 0xXY
	*/

	switch (baud)
	{
    case BAUD_9600:
       	UART1_BRR2 = 0x03; UART1_BRR1 = 0x68; // 9600 baud
        break;
    case BAUD_38400:
       	UART1_BRR2 = 0x01; UART1_BRR1 = 0x1A; // 38400 baud
        break;
    case BAUD_57600:
       	UART1_BRR2 = 0x06; UART1_BRR1 = 0x11; // 57600 baud
        break;
    default:
   	    UART1_BRR2 = 0x0b; UART1_BRR1 = 0x08; // 115200 baud
        break;
	}



	// UART1_BRR2 = 0x03; UART1_BRR1 = 0x68; // 9600 baud


    UART1->CR2 |= (uint8_t)UART1_CR2_REN;


    //enableUART1Int(UART1_IT_RXNE_OR, ENABLE);
    UART1->CR2 |= 0x20; // ReceivInterruptEnable

    /* enable interrupts */
    enableInterrupts();

     /* UART1 Enable */
    UART1->CR1 &= (uint8_t)(~UART1_CR1_UARTD);


#else // NO_SPL
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);

  UART1_DeInit();
  /* UART1 configuration ------------------------------------------------------*/
  /* UART1 configured as follow:
        - BaudRate = 115200 baud
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Receive and transmit enabled
        - UART1 Clock disabled
  */
  UART1_Init((uint32_t)115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO,
              UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
#endif



}

void UartPutc(char c)
{
#ifdef NO_SPL
  while(!(UART1_SR & UART_SR_TXE))
    ;
  UART1_DR = c;
#else
  UART1_SendData8(c);
  /* Loop until the end of transmission */
  while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
#endif
}

void UartPuts(char *s)
{
  while (*s)
  {
    UartPutc(*s++);
  }
}


static char NibbleToASCII(unsigned char n)
{
    if (n<10)
       return ('0'+n);
    return ('A'+n-10);
}



void UartSendHexByte(unsigned char b)
{
    UartPutc(NibbleToASCII(b>>4));
    UartPutc(NibbleToASCII(b&0xF));
}





#ifndef UART_H_INCLUDED
#define UART_H_INCLUDED


#define BAUD_9600   1
#define BAUD_38400  2
#define BAUD_57600  3
#define BAUD_115200 4

extern void UartInit(uint8_t baud);
extern void UartPutc(char c);
extern void UartPuts(char *s);


extern void UartSendHexByte(char b);




#endif // UART_H_INCLUDED

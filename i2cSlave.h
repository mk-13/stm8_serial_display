#ifndef I2CSLAVE_H_INCLUDED
#define I2CSLAVE_H_INCLUDED


#define RX_BUF_SIZE 128

extern __IO uint8_t Buffer_Rx[RX_BUF_SIZE];
extern __IO uint8_t RxBufWp, RxBufRp;



extern void i2cSlaveInit(uint16_t addr);

#endif // I2CSLAVE_H_INCLUDED

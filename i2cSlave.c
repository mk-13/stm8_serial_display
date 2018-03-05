// core header file from our library project:
#include "stm8s.h"
#include "i2cSlave.h"

__IO uint8_t Buffer_Rx[RX_BUF_SIZE];
__IO uint8_t RxBufWp = 0, RxBufRp = 0;

#ifdef USE_I2C

void i2cSlaveInit(uint16_t addr)
{
    I2C_DeInit();
    I2C_Init(100000, addr, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, 16);
    /* Enable Error Interrupt*/
    I2C_ITConfig((I2C_IT_TypeDef)(I2C_IT_ERR | I2C_IT_EVT | I2C_IT_BUF), ENABLE);
    enableInterrupts();
}
#endif



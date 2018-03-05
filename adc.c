#include "stm8s.h"
#include "adc.h"

void ADCInit(void)
{
    ADC1->CR1 = 1; //ADC_CR1_ADON, fADC=fMaster/2, single conversion;       //  Turn ADC on, note a second set is required to start the conversion.

//    ADC1->CSR = pin;      //  ADC channel, no interrupts

    ADC1->CR3 = 0; // ADC_CR3_DBUF = 0;
    ADC1->CR2 = 8; // ADC_CR2_ALIGN, no ext trig, no scan mode
    // ADC_CSR_EOCIE = 1;      //  Enable the interrupt after conversion completed.
}

#if 0
// from http://www.electroons.com/blog/stm8-tutorials-3-adc-interfacing/
unsigned int readADC1(unsigned int channel)
{
     unsigned int val=0;
     //using ADC in single conversion mode
     ADC1->CSR |= ((0x0F)&channel); // select channel
     ADC1->CR2 |= (1<<3); // Right Aligned Data
     ADC1->CR1 |= (1<<0); // ADC ON
     ADC1->CR1 |= (1<<0); // ADC Start Conversion
     while(((ADC1->CSR)&(1<<7))== 0); // Wait till EOC
     val |= (unsigned int)ADC1->DRL;
     val |= (unsigned int)ADC1->DRH<<8;
     ADC1->CR1 &= ~(1<<0); // ADC Stop Conversion
     val &= 0x03ff;
     return (val);
}
#endif

uint16_t analogRead(uint8_t pin)
{
    uint16_t v;
    ADC1->CSR = pin;      //  ADC channel, no interrupts
    ADC1->CR1 = 1; //ADC_CR1_ADON trigger conversion
    ADC1->CR1 = 1; //ADC_CR1_ADON trigger conversion

    // poll for conversion complete

   while ((ADC1->CSR&0x80)== 0)
      ;

   ADC1->CR1 = 0;

   v = ADC1->DRL;
   v |= (ADC1->DRH << 8);

   return v;
}

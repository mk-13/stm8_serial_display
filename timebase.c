// core header file from our library project:
#include "stm8s.h"
#include "timebase.h"

/**
  * @brief  Configure TIM4 to generate an update interrupt each 1ms
  * @param  None
  * @retval None
  */
void TimebaseInit(void)
{
  /* TIM4 configuration:
   - TIM4CLK is set to 16 MHz, the TIM4 Prescaler is equal to 128 so the TIM1 counter
   clock used is 16 MHz / 128 = 125 000 Hz
  - With 125 000 Hz we can generate time base:
      max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
      min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
  - In this example we need to generate a time base equal to 1 ms
   so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 */

  /* Time base configuration */
  //TIM4_TimeBaseInit(TIM4_PRESCALER_128, 124);
  /* Set the Prescaler value */
  TIM4->PSCR = TIM4_PRESCALER_128;
  /* Set the Autoreload value */
  TIM4->ARR = 124;

  /* Clear TIM4 update flag */
  //TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  TIM4->SR1 = (uint8_t)(~(TIM4_FLAG_UPDATE));

  /* Enable update interrupt */
  //TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
  TIM4->IER |= (uint8_t)TIM4_IT_UPDATE;

  /* enable interrupts */
  enableInterrupts();

  /* Enable TIM4 */
  //TIM4_Cmd(ENABLE);
  TIM4->CR1 |= TIM4_CR1_CEN;
}


uint16_t  waitTime;
uint16_t  timerTime;
uint16_t  monitorTime;



void delay(int16_t ms)
{
    waitTime = ms;
    while (waitTime != 0)
        ;
}





void startTimer(uint16_t ms)
{
    timerTime = ms;
}


uint16_t checkTimer(void)
{
    return timerTime;
}


void startMonitorTimer(uint16_t ms)
{
    monitorTime = ms;
}


uint16_t checkMonitorTimer(void)
{
    return monitorTime;
}

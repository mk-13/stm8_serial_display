#ifndef TIMEBASE_H_INCLUDED
#define TIMEBASE_H_INCLUDED

extern void TimebaseInit(void);
extern void delay(int16_t ms);

extern void startTimer(uint16_t ms);
extern uint16_t checkTimer(void);


extern void startMonitorTimer(uint16_t ms);
extern uint16_t checkMonitorTimer(void);


#endif // TIMEBASE_H_INCLUDED

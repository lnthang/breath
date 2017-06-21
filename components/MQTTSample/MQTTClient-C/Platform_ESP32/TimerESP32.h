#ifndef __TIMER_ESP32_H__
#define __TIMER_ESP32_H__

typedef struct Timer
{
  int endTime;
} Timer;

void TimerInit(Timer*);
char TimerIsExpired(Timer*);
void TimerCountdownMS(Timer*, unsigned int);
void TimerCountdown(Timer*, unsigned int);
int TimerLeftMS(Timer*);

#endif

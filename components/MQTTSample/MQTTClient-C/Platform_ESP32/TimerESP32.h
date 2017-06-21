#ifndef __TIMER_ESP32_H__
#define __TIMER_ESP32_H__

#include "freertos/FreeRTOS.h"

typedef struct Timer
{
  portTickType endTime;
} Timer;

void TimerInit(Timer*);
char TimerIsExpired(Timer*);
void TimerCountdownMS(Timer*, unsigned int);
void TimerCountdown(Timer*, unsigned int);
int TimerLeftMS(Timer*);

#endif

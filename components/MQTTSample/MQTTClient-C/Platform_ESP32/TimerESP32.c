#include "TimerESP32.h"

#include "freertos/task.h"

void TimerInit(Timer *timer)
{
  timer->endTime = 0;
}

char TimerIsExpired(Timer *timer)
{
  return ((timer->endTime - xTaskGetTickCount()) <= 0);
}

void TimerCountdownMS(Timer *timer, unsigned int timeoutInMSec)
{
  timer->endTime = xTaskGetTickCount() + (timeoutInMSec / portTICK_RATE_MS);
}

void TimerCountdown(Timer *timer, unsigned int timeoutInSec)
{
  TimerCountdownMS(timer, timeoutInSec * 1000);
}

int TimerLeftMS(Timer *timer)
{
  int timeLeft = timer->endTime - xTaskGetTickCount();

  if (timeLeft < 0)
  {
    timeLeft = 0;
  }

  return (timeLeft / portTICK_RATE_MS);
}

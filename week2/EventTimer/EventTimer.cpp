#include "EventTimer.h"

EventTimer::EventTimer(void)
{
  duration = 0;
  startTime = 0;
  isRunning = false;
}

void EventTimer::set(uint32_t dur)
{
  duration = dur;
  startTime = millis();
  isRunning = true;
}

bool EventTimer::checkExpired(void)
{
   if(millis() >= (startTime + duration))
    return true;
   else{
    isRunning = false;
    return false;
   }
}

void EventTimer::cancel(void)
{
  duration = 0;
  startTime = 0;
  isRunning = false;
}

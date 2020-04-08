#ifndef _EVENTTIMER_H
#define _EVENTTIMER_H

#include "Arduino.h"

class EventTimer
{
  private:
    uint32_t duration, startTime;
    bool isRunning;

   public:
    EventTimer(void); 
    void set(uint32_t dur); 
    bool checkExpired(void);
    void cancel(void);
};

#endif

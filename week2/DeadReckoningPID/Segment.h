#ifndef _SEGMENT_H
#define _SEGMENT_H

#include "Arduino.h"

class Segment
{
  private:
    int16_t rightSpeed, leftSpeed;
    int32_t encTicks;
    
  public:
    Segment(void);
    void setSegmentVals(int16_t rSpeed, int16_t lSpeed, int32_t distance); 
    int16_t Segment::getRightSpeed(void);
    int16_t Segment::getLeftSpeed(void);
    int32_t Segment::getEncTicks(void);
};

#endif

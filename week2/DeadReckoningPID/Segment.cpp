#include "Segment.h"

Segment::Segment(void)
{
  rightSpeed = 0;
  leftSpeed = 0;
  encTicks = 0;
}

void Segment::setSegmentVals(int16_t rSpeed, int16_t lSpeed, int32_t distance)
{
  rightSpeed = rSpeed;
  leftSpeed = lSpeed;
  encTicks = 50.5*distance; //convert from cm to encoder ticks based on ticks/cm val from previous section
}

int16_t Segment::getRightSpeed(void)
{
  return rightSpeed;
}

int16_t Segment::getLeftSpeed(void)
{
  return leftSpeed;
}

int32_t Segment::getEncTicks(void)
{
  return encTicks;
}

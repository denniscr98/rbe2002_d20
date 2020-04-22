#ifndef _DRIVEPID_H
#define _DRIVEPID_H
#include <Zumo32U4Motors.h>
#include "Arduino.h"

class DrivePID
{
  private:
    Zumo32U4Motors motors;
    volatile uint8_t readyToPID;   //a flag that is set when the PID timer overflows
    volatile int16_t countsLeft;
    volatile int16_t countsRight;
    int32_t deltaTicksLeft;
    int32_t deltaTicksRight;
    uint16_t wallDist;
    float Kp;
    float KpDist;
    float Ki;
    float Kd;
    int32_t encTicksToDrive;
    float targetLeft;
    float targetRight;

  public:
    DrivePID(float KP, float KI, float KD, float KpDist, float targetSpeeds, uint16_t distFromWall);
    void initChassis(void);
    bool checkTicksExpired();
    void doPidDriving(uint16_t ultraReading);
    void setReadyToPID(volatile uint8_t isReady);
    void setCountsLeft(volatile int16_t leftCounts);
    void adjustTargets(uint16_t ultraReading, int16_t speedLeft, int16_t speedRight);
    void setCountsRight(volatile int16_t rightCounts);
};

#endif

#ifndef _FILTER_H
#define _FILTER_H
#include <Wire.h>
#include <Zumo32U4.h>

class filter
{
  private:
  LSM303 compass;
  L3G gyro;
  Zumo32U4ButtonA buttonA;
  
  float estimatedAngle = 0;
  float observedAngle = 0;
  float gyroBias = 0;
  double gyroSens = 8.75;
  double timeStep = 0.01;
  double k = 0.9;
  float accXoffset = 0;
  float e = 0.005;

  public:
  filter(void);
  void Init(void);
  bool CalcAngle(float& observedAngle, float& est, float& bias);
};

#endif

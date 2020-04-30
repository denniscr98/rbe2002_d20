/* This example reads the raw values from the L3GD20H gyro and
LSM303D accelerometer and magnetometer on the Zumo 32U4, and
prints those raw values to the serial monitor.

The accelerometer readings can be converted to units of g using
the conversion factors specified in the "Sensor characteristics"
table in the LSM303 datasheet.  The default full-scale (FS)
setting is +/- 2 g, so the conversion factor is 0.61 mg/LSB
(least-significant bit).  A raw reading of 16384 would correspond
to 1 g.

The gyro readings can be converted to degrees per second (dps)
using the "Mechanical characteristics" table in the L3GD20H
datasheet.  The default sensitivity is 8.75 mdps/LSB
(least-significant bit).  A raw reading of 10285 would correspond
to 90 dps.

The magnetometer readings are more difficult to interpret and
will usually require calibration. */

#include "filter.h"

filter myFilter;


float observedAngle = 0;
float estimatedAngle = 0;
float gyroBias = 0;


void setup()
{
 myFilter.Init();
}

void loop()
{
  if(myFilter.CalcAngle(observedAngle, estimatedAngle, gyroBias)){
    Serial.print(observedAngle);
    Serial.print("\t");
    Serial.print(estimatedAngle);
    Serial.print("\t");
    Serial.println(gyroBias);

  }
}

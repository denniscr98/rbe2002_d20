#include "filter.h"


filter::filter(){}

void filter::Init(void)
{
   Wire.begin();

  if (!compass.init())
  {
    // Failed to detect the compass.
    ledRed(1);
    while(1)
    {
      Serial.println(F("Failed to detect the compass."));
      delay(100);
    }
  }

  compass.enableDefault();

  if (!gyro.init())
  {
    // Failed to detect the gyro.
    ledRed(1);
    while(1)
    {
      Serial.println(F("Failed to detect gyro."));
      delay(100);
    }
  }

  gyro.enableDefault();

  compass.writeReg(LSM303::CTRL1, 0x67); //set accelerometer sampl freq to 100Hz
  
//  while(!Serial){} //wait for the Serial Monitor
//  uint8_t ctrl1 = compass.readReg(LSM303::CTRL1);
//  Serial.print("CTRL1 is: ");
//  Serial.println(ctrl1, HEX);
//
//  uint8_t gyroctrl1 = gyro.readReg(L3G::CTRL1);
//  Serial.print("Gyro CTRL1 is: ");
//  Serial.println(gyroctrl1, HEX);
  gyro.writeReg(L3G::CTRL1, 0xBF); //set gyro sampl freq to 400Hz w/ 110Hz cut-off
  gyro.writeReg(L3G::CTRL4, 0x0);
//  gyroctrl1 = gyro.readReg(L3G::CTRL1);
//  Serial.print("updated Gyro CTRL1 is: ");
//  Serial.println(gyroctrl1, HEX);
}

bool filter::CalcAngle(float& obs, float& est, float& bias)
{
  if(compass.readReg(LSM303::STATUS_A) & 0x8){ //bitwise comparison to 00001000
    compass.readAcc(); //read accelerometer to clear status bit
    gyro.read();

    if(buttonA.getSingleDebouncedPress())
    {
      float tempSum = 0;
      for(int i = 0; i < 200; i++){
        tempSum =+ compass.a.x;
      }
      accXoffset = tempSum/200;
    }

    float rotSpeed = 0.0174533 * ((gyro.g.y * gyroSens) / 1000); 

    observedAngle = atan2(-(compass.a.x - accXoffset), compass.a.z);
    
    estimatedAngle = estimatedAngle + (rotSpeed - gyroBias)*timeStep;
    
    gyroBias = gyroBias + e*(estimatedAngle - observedAngle);
    
    estimatedAngle = k*estimatedAngle + (1-k)*observedAngle;

    est = estimatedAngle;
    obs = observedAngle;
    bias = gyroBias;
    
    return true;
  }else
    return false;
}

#include <Zumo32U4Encoders.h>
#include <Zumo32U4Motors.h>
#include "UltrasonicSensor.h"
#include <Wire.h>
#include <Zumo32U4.h>
#include "button.h"       //include your button class from last week
#include "DrivePID.h"
#define NUM_SENSORS 5

Zumo32U4Motors motors;
Zumo32U4Encoders encoders; //(we're not acutally using this in this code, but we will soon)
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;

const uint8_t ultraSensorPin = 14;
const uint16_t distFromWall = 475;
unsigned long buttonCount = 0; //declare global vars
bool startDriving = false;
uint16_t lineSensorValues[NUM_SENSORS];

//declare the robot states here
enum DRIVING_STATE {IDLE_DRIV, DRIVING};
DRIVING_STATE state = IDLE_DRIV;

UltrasonicSensor myUltrasonicSensor(ultraSensorPin);
DrivePID chassisDrive(10, 2, 0, 0.05, 20, 475);
Button buttonC(17, 10); //button A is pin 14 on the Zumo

void setup() {
  Serial.begin(115200);
  //while(!Serial) {} //you must open the Serial Monitor to get past this step!
  Serial.println("Hi!");

  lineSensors.initFiveSensors();
  myUltrasonicSensor.initUltrasonic();
  buttonC.Init(); //don't forget to call Init()!
  chassisDrive.initChassis();
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if(!startDriving) //only read for button presses when not driving
    buttonPress();
  else{
    startDriving = isOnWhite(); //only check timer while driving
  }

  switch(state){
    case IDLE_DRIV:
      if(startDriving)
        state = DRIVING;
      else{
        state = IDLE_DRIV;
      }
    break;

    case DRIVING:
      if(startDriving){
        chassisDrive.doPidDriving(myUltrasonicSensor.getPulseLTC());
      }else{ //if changed to not driving set motors to 0 
        motors.setSpeeds(-400,400); //do 360 rotation
        delay(430);
        motors.setSpeeds(0,0);
        state = IDLE_DRIV;
      }
    break;   
  }
}

//handle the button press
void buttonPress()
{
   if(buttonC.checkButtonPress()){
      startDriving = true;
      delay(1000);
      Serial.print("Button pressed"); //debugging
   }
}

bool isOnWhite(void)
{
  lineSensors.read(lineSensorValues, true);
  
  if(lineSensorValues[0] < 500)
    return true;
  else if(lineSensorValues[0] > 1500)
    return false;
}

/*
 * ISR for timing. Basically, raise a flag on overflow. Timer4 is set up to run with a pre-scaler 
 * of 1024 and TOP is set to 249. Clock is 16 MHz, so interval is dT = (1024 * 250) / 16 MHz = 16 ms.
 */
ISR(TIMER4_OVF_vect)
{
  //Capture a "snapshot" of the encoder counts for later processing
  chassisDrive.setCountsLeft(encoders.getCountsLeft());
  chassisDrive.setCountsRight(encoders.getCountsRight());

  chassisDrive.setReadyToPID(1);
}

/*
 * ISR for input capture on pin 13. We can precisely capture the value of TIMER3
 * by setting TCCR3B to capture either a rising or falling edge. This ISR
 * then reads the captured value (stored in ICR3) and copies it to the appropriate
 * variable.
 */
ISR(TIMER3_CAPT_vect)
{
  if(myUltrasonicSensor.getPulseState() == 1) //we're waiting for a rising edge (correlate states to their corresponding enums)
  {
    myUltrasonicSensor.setPulseStart(ICR3); //copy the input capture register (timer count)
    TCCR3B &= 0xBF;    //now set to capture falling edge on pin 13
    myUltrasonicSensor.setPulseState(2);
  }

  else if(myUltrasonicSensor.getPulseState() == 2) //waiting for the falling edge
  {
    myUltrasonicSensor.setPulseEnd(ICR3);
    myUltrasonicSensor.setPulseState(3);; //raise a flag to indicate that we have data
  }
}

#include "DrivePID.h"


DrivePID::DrivePID(float KP, float KI, float KD,  float KPdist, float targetSpeeds, uint16_t distFromWall){
  readyToPID = 0;
  countsLeft = 0;
  countsRight = 0;
  deltaTicksLeft = 0;
  deltaTicksRight = 0;
  Kp = KP;
  Ki = KI;
  Kd = KD;
  KpDist = KPdist;
  targetLeft = targetSpeeds;
  targetRight = targetSpeeds;
  wallDist = distFromWall;
}

void DrivePID::initChassis(void)
{
  noInterrupts(); //disable interupts while we mess with the Timer4 registers
  //sets up timer 4
  TCCR4A = 0x00; //disable some functionality -- no need to worry about this
  TCCR4B = 0x0C; //sets the prescaler -- look in the handout for values
  TCCR4C = 0x04; //toggles pin 6 at one-half the timer frequency
  TCCR4D = 0x00; //normal mode

  OCR4C = 107;   //TOP goes in OCR4C
  TIMSK4 = 0x04; //enable overflow interrupt
  
  interrupts(); //re-enable interrupts
}

void DrivePID::doPidDriving(uint16_t ultraReading)
{
  if(readyToPID) //timer flag set
  {
    //clear the timer fla
    readyToPID = 0;

    //for tracking previous counts
    static int16_t prevLeft = 0;
    static int16_t prevRight = 0;

    //error sum
    static int16_t sumLeft = 0;
    static int16_t sumRight = 0;

    /*
     * Do PID stuffs here. Note that we turn off interupts while we read countsLeft/Right
     * so that it won't get accidentally updated (in the ISR) while we're reading it.
     */
    noInterrupts();
    int16_t speedLeft = countsLeft - prevLeft;
    int16_t speedRight = countsRight - prevRight;

    prevLeft = countsLeft;
    
    prevRight = countsRight;

    interrupts();

    adjustTargets(ultraReading, speedLeft, speedRight);
     
    int16_t errorLeft = targetLeft - speedLeft;
    sumLeft += errorLeft;
    
    float effortLeft = Kp * errorLeft + Ki * sumLeft;

    int16_t errorRight = targetRight - speedRight;
    sumRight += errorRight;
    
    float effortRight = Kp * errorRight + Ki * sumRight;

    Serial.print(ultraReading); //Debugging
    Serial.print('\t');
    Serial.print(targetLeft);
    Serial.print('\t');
    Serial.print(effortLeft);
    Serial.print('\t');
    Serial.print(targetRight);
    Serial.print('\t');
    Serial.println(effortRight);
    
    motors.setSpeeds(effortLeft, effortRight); //up to you to add the right motor
  }

}

void DrivePID::adjustTargets(uint16_t ultraReading, int16_t speedLeft, int16_t speedRight)
{ 
  bool turningLeft = false;
  bool turningRight = false;
  
  if(ultraReading > 1000 && ultraReading < 1900) //Hard-code corner cases when ultrasonic readings are unreliable
    turningLeft = true;
  else if (ultraReading > 1900 && ultraReading < 2500)
    turningRight = true;
  
  int16_t errorDist = wallDist - ultraReading;

  targetLeft = KpDist * errorDist + targetRight;

  if(turningLeft)
    targetLeft = 30;
  else if (turningRight)
    targetLeft = 5;
  else
    targetLeft = targetLeft; //else do regular PID
}

void DrivePID::setCountsLeft(volatile int16_t leftCounts)
{
  countsLeft = leftCounts;
}

void DrivePID::setCountsRight(volatile int16_t rightCounts)
{
  countsRight = rightCounts;
}

void DrivePID::setReadyToPID(volatile uint8_t isReady)
{
  readyToPID = isReady;
}

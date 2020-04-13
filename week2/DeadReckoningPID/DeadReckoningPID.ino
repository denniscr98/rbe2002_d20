#include <Zumo32U4Motors.h>
#include <Zumo32U4Encoders.h>

#include "button.h"       //include your button class from last week
#include "Segment.h"
#include "params.h"
#include "serial_comm.h"

Button buttonA(14, 10); //button A is pin 14 on the Zumo
//EventTimer timer;   //assumes you named your class EventTimer
volatile uint8_t readyToPID = 0;   //a flag that is set when the PID timer overflows

//use the Pololu libraries for motors and encoders
Zumo32U4Motors motors;
Zumo32U4Encoders encoders; //(we're not acutally using this in this code, but we will soon)
volatile int16_t countsLeft = 0;
volatile int16_t countsRight = 0;

int32_t deltaTicksLeft = 0;
int32_t deltaTicksRight = 0;

//declare the robot states here
//...
enum DRIVING_STATE {IDLE_DRIV, DRIVING};
DRIVING_STATE state = IDLE_DRIV;

unsigned long buttonCount = 0; //declare global vars
const int numberOfSegments = 3;
Segment segments[numberOfSegments];
unsigned int segmentNum = 0;
bool startDriving = false;

void setup()
{
  Serial.begin(115200);
  Serial.println("Hello.");

  buttonA.Init(); //don't forget to call Init()!

  segments[0].setSegmentVals(50, 50, 60);
  segments[1].setSegmentVals(-20, 20, 9);
  segments[2].setSegmentVals(50, 50, 40);



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

void loop() 
{  
  //put in a simple event-driven structure
  //if(SomeEvent()) HandleSomeEvent()
  //...
  if(!startDriving) //only read for button presses when not driving
    buttonPress();
  else{
    ticksMet(); //only check timer while driving
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

      if(startDriving)
        doPidDriving();
      else{ //if changed to not driving set motors to 0 
        motors.setSpeeds(0,0);
        state = IDLE_DRIV;
      }
    break;   
  }
 
}

//add you handlers here:

//handle the button press
void buttonPress()
{
   if(buttonA.checkButtonPress()){
      startDriving = true;
      Serial.print("Button pressed, current segment number: "); //debugging
      Serial.println(segmentNum);
   }
}

//handle the timer expiring
void ticksMet()
{
  if(checkTicksExpired()){
    segmentNum++; //inc seg num
         
      if(segmentNum >= numberOfSegments){ //check for end of array
        startDriving = false; //reset bool
        segmentNum = 0; //reset current segment
      }
  }
   
}

bool checkTicksExpired()
{
  if((countsLeft >= (segments[segmentNum].getEncTicks() + deltaTicksLeft)) || //calc delta of ticks between encoders
    (countsRight >= (segments[segmentNum].getEncTicks()+ deltaTicksRight))) {
      deltaTicksRight =+ countsRight; //update running sum of temp enc ticks
      deltaTicksLeft =+ countsLeft;

      return true;

    }else
      return false;
}

void doPidDriving(){
  if(readyToPID) //timer flag set
  {
    //clear the timer flag
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

    targetLeft = segments[segmentNum].getLeftSpeed();
    targetRight = segments[segmentNum].getRightSpeed();
     
    int16_t errorLeft = targetLeft - speedLeft;
    sumLeft += errorLeft;
    
    float effortLeft = Kp * errorLeft + Ki * sumLeft;

    int16_t errorRight = targetRight - speedRight;
    sumRight += errorRight;
    
    float effortRight = Kp * errorRight + Ki * sumRight;
    
    motors.setSpeeds(effortLeft, effortRight); //up to you to add the right motor
  }

}

/*
 * ISR for timing. Basically, raise a flag on overflow. Timer4 is set up to run with a pre-scaler 
 * of 1024 and TOP is set to 249. Clock is 16 MHz, so interval is dT = (1024 * 250) / 16 MHz = 16 ms.
 */
ISR(TIMER4_OVF_vect)
{
  //Capture a "snapshot" of the encoder counts for later processing
  countsLeft = encoders.getCountsLeft();
  countsRight = encoders.getCountsRight();

  readyToPID = 1;
}

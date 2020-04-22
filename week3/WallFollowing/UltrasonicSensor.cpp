#include "UltrasonicSensor.h"

UltrasonicSensor::UltrasonicSensor(uint8_t ultPin)
{
  trigPin = ultPin;
}

void UltrasonicSensor::initUltrasonic(void)
{
  noInterrupts(); //disable interupts while we mess with the control registers
  
  //sets timer 3 to normal mode (16-bit, fast counter)
  TCCR3A = 0; 
  
  interrupts(); //re-enable interrupts

  //note that the Arduino machinery has already set the prescaler elsewhere
  //so we'll print out the value of the register to figure out what it is
//  Serial.print("TCCR3B = ");
//  Serial.println(TCCR3B, HEX);

  pinMode(trigPin, OUTPUT);
  pinMode(13, INPUT); //explicitly make 13 an input, since it defaults to OUTPUT in Arduino World (LED)

  lastPing = millis();
}

uint16_t UltrasonicSensor::getPulseLTC(void)
{
    //schedule pings roughly every PING_INTERVAL milliseconds
  if(millis() - lastPing > PING_INTERVAL)
  {
    lastPing = millis();
    commandPing(trigPin); //command a ping
  }
  
  if(pulseState == PLS_CAPTURED) //we got an echo
  {
    //update the state to IDLE
    pulseState = PLS_IDLE;

    /*
     * Calculate the length of the pulse (in timer counts!). Note that we turn off
     * interrupts for a VERY short period so that there is no risk of the ISR changing
     * pulseEnd or pulseStart. As noted in class, the way the state machine works, this
     * isn't a problem, but best practice is to ensure that no side effects can occur.
     */
    noInterrupts();
    uint16_t pulseLengthTimerCounts = pulseEnd - pulseStart;
    interrupts();

    return pulseLengthTimerCounts;
  }
}

/*
 * Commands the MaxBotix to take a reading
 */
void UltrasonicSensor::commandPing(int trigPin)
{
  cli(); //disable interrupts

  TIFR3 = 0x20; //clear any interrupt flag that might be there

  TIMSK3 |= 0x20; //enable the input capture interrupt
  TCCR3B |= 0xC0; //set to capture the rising edge on pin 13; enable noise cancel

  sei(); //re-enable interrupts

  //update the state and command a ping
  pulseState = PLS_WAITING_LOW;
  
  digitalWrite(trigPin, HIGH); //command a ping by bringing TRIG HIGH
  delayMicroseconds(10);      //we'll allow a delay here for convenience; it's only 10 us
  digitalWrite(trigPin, LOW);  //must bring the TRIG pin back LOW to get it to send a ping
}

uint8_t UltrasonicSensor::getPulseState(void)
{
  return pulseState;
}

void UltrasonicSensor::setPulseState(uint8_t num)
{
  pulseState = num;
}

void UltrasonicSensor::setPulseStart(uint16_t plsStart)
{
  pulseStart = plsStart;
}

void UltrasonicSensor::setPulseEnd(uint16_t plsEnd)
{
  pulseEnd = plsEnd;
}

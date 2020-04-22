#ifndef _ULTRASONICSENSOR_H
#define _ULTRASONICSENSOR_H

#include "Arduino.h"

class UltrasonicSensor
{
  private:
    volatile uint16_t pulseStart = 0;
    volatile uint16_t pulseEnd = 0;
    
    //define the states for the echo capture
    enum PULSE_STATE {PLS_IDLE, PLS_WAITING_LOW, PLS_WAITING_HIGH, PLS_CAPTURED};
    
    //and initialize to IDLE
    volatile PULSE_STATE pulseState = PLS_IDLE;
    
    //this may be any appropriate pin, connect the pin to Trig on the sensor
    uint8_t trigPin;
    
    //for scheduling pings
    uint32_t lastPing = 0;
    uint32_t PING_INTERVAL = 250; //ms

  public:
    UltrasonicSensor(uint8_t ultPin);
    void initUltrasonic(void);
    void commandPing(int ultPin);
    void setPulseState(uint8_t num);
    void setPulseStart(uint16_t plsStart);
    void setPulseEnd(uint16_t plsEnd);
    uint8_t getPulseState(void);
    uint16_t getPulseLTC(void);
};

#endif

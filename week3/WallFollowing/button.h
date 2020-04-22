#ifndef _BUTTON_H
#define _BUTTON_H

#include "Arduino.h"

class Button
{
  private:
    enum BUTTON_STATE {BUTTON_STABLE, BUTTON_UNSTABLE};
    BUTTON_STATE state = BUTTON_STABLE;

    uint8_t buttonPin = 14; //uint8_t: "unsigned integer 8-bit ... type" => 255
    
    //assume active LOW
    uint8_t buttonPosition = HIGH; //most recent stable position
    uint8_t tempButtonPos = HIGH; //temporary position, bouncing not complete
    
    uint32_t lastBounceTime = 0; //millis() returns an unsigned long (32-bit unsigned)
    uint32_t debouncePeriod = 10; //in ms
  
  public:
    Button(uint8_t pin, uint32_t db = 10); //default to 10ms debounce
    void Init(bool usePullup = true);
    bool checkButtonPress(void);
};

#endif

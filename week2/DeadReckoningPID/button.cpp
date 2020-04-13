#include "button.h"

Button::Button(uint8_t pin, uint32_t db = 10)
{
  buttonPin = pin;
  debouncePeriod = db;
}

void Button::Init(bool usePullup = true)
{
  if(usePullup) 
    pinMode(buttonPin, INPUT_PULLUP);
  else 
    pinMode(buttonPin, INPUT);
}


bool Button::checkButtonPress(void)
{
  buttonPosition = digitalRead(buttonPin);
 // tempButtonPos = buttonPosition;

  switch(state){

    case BUTTON_STABLE:
      if(buttonPosition != tempButtonPos){
        lastBounceTime = millis();
        tempButtonPos = buttonPosition;
        state = BUTTON_UNSTABLE;
        return false;   
      }     
      break;
      
   case BUTTON_UNSTABLE:
      if(buttonPosition != tempButtonPos){
        lastBounceTime = millis();
        tempButtonPos = buttonPosition;
        state = BUTTON_UNSTABLE;
        return false;  
    
      }else if(millis() > (lastBounceTime + debouncePeriod) && buttonPosition == HIGH){
        state = BUTTON_STABLE;
        return true;
      }
      
      break;
   
  }

   return false;
}

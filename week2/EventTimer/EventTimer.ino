#include "EventTimer.h"

EventTimer timer;

void setup() {
  Serial.begin(115200);
  
  timer.set(1000);
}

void loop() {
  if(timer.checkExpired())
  {
    Serial.println(millis());
    timer.set(1000);
  }

}

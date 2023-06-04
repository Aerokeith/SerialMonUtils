#include <Arduino.h>
#include <elapsedMillis.h>
#include "SerialMonLog.h"

serialMonLogClass smLog;
elapsedMillis time;

void setup() {
  Serial.begin(115200);
  time = 0;
  delay(2000);
  smLog.setTimeStamp(&time);
  smLog.logLevel = 1;
  LOGMSG(1, "This should print: %u", 5);
  LOGMSG(2, "This shouldn't print: %u", 10);
}

void loop() {
}


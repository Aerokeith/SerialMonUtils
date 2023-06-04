/* SerialMonLog
    SerialMonLog.h and SerialMonLog.cpp implement a serialMonLogClass that provides Serial Monitor output macros and functions
    that support printing of timestamped log messages based on the criticality of the message relative to the current global 
    criticality level. The LOGMSG macro allows complex messages to be printed using the format specifier capabilities of the
    standard sprintf() function. As currently written (but easily modified), the LOGMSG macro requires the prior definition 
    of a serialMonLogClass object named "smLog". 
    The class data member logLevel (0 - 255) may be set to control which log messages are printed. For example, if 
    logLevel = 2, only log messages with criticality 0, 1, and 2 will be printed. 
    A boolean data member named "enable" may be set to globally enable or disable printing of all log messages regardless of
    criticality level. 
*/
#include <Arduino.h>
#include <elapsedMillis.h>
#include "SerialMonLog.h"


/* serialMonLogClass::printLog()
    Prints the log message string that has been previously assembled in serialMonLogClass::buf[] by a call to the
    LOGMSG macro. The message is prepended with a timestamp (in seconds), if setTimeStamp() has been previously called to define the 
    timestamp variable.
  Parameters: None
  Returns: None
*/
void serialMonLogClass::printLog() {
  float ts;   // temp timestamp value

  if (timeStampP != NULL) {           // if a timestamp timer variable has been defined
    ts = (float) *timeStampP / 1000;  // convert elapedMillis timer value into seconds
    sprintf(tsBuf, "[%5.3f] ", ts);   // assemble the timestamp string
    Serial.print(tsBuf);              // print the timestamp
  }
  Serial.println(msgBuf);             // print the log message
}


/* serialMonLogClass::setTimeStamp()
    Stores a pointer to an elapsedMillis timer, to be used as a timestamp by printLog()
  Parameters: 
    elapsedMillis *timeStampP: pointer to timestamp timer
  Returns: None
*/
void serialMonLogClass::setTimeStamp(elapsedMillis *timeStampP) {
  this->timeStampP = timeStampP;
}

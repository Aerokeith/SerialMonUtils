#include <Arduino.h>
#include <elapsedMillis.h>

#ifndef _SERIALMONLOG_TYPES       // prevent multiple redefinition of types in this header
#define _SERIALMONLOG_TYPES

/* LOGMSG() [Variadic Macro]
    Optionally prints a timestamped log message, based on the criticality level of the message relative to the current system 
    logging level determined by a referenced variable. Printing is also conditional upon a referenced variable that can
    globally enable/disable all log messages. This macro assumes the existence of a serialMonLogClass object named "smLog",
    which should be defined globally for access by all functions that use this macro
  Parameters:
    uint8_t msgLevel: message criticality level (0 = most critical)
    ...: variadic argument consisting of an sprintf format string followed by a variable number of variables
  Returns: None
  Example: LOGMSG(2, "The value of foo is %u", foo);    // Note that the '\n' may be omitted
*/
#define LOGMSG(msgLevel, ...) if ((smLog.enable) && (msgLevel <= smLog.logLevel)) \
                                    { sprintf(smLog.msgBuf, __VA_ARGS__); smLog.printLog(); }

const uint8_t maxMsgLen = 100;      // max number of characters in a log message string, including the terminating '\0'
const uint8_t maxTimestampLen = 10; // max number of chars in a timestamp string (float seconds), including '\0

class serialMonLogClass {
  elapsedMillis *timeStampP;              // pointer to an elapsedMillis timer to be used for log message timestamps
  char tsBuf[maxTimestampLen];            // temporary buffer to assemble message timestamp
public:
  char msgBuf[maxMsgLen];                 // temporary buffer to assemble log messages (must be public for access by macro)
  uint8_t logLevel;                       // current logging level (0 = most critical)
  bool enable;                            // enables/disables all log messages, regardless of criticality level
  serialMonLogClass() { logLevel = 0; enable = false; }
  void printLog();
  void setTimeStamp(elapsedMillis *timeStampP);
};

#endif    // _SERIALMONLOG_TYPES
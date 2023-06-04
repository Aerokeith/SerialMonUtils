/* Main.cpp
    Example program to demonstrate the capabilities of the SerialMonUtils library, specifically the C++ classes
    serialMonLogClass, serialMonInputClass, and serialMonCmdClass. 
*/
#include <Arduino.h>
#include <elapsedMillis.h>      // system library that implements 1-ms resolution timers
#include "SerialMonLog.h"       // header file for library containing log message functions
#include "SerialMonCmd.h"       // header file for library to implement command menu functions
#include "Menu.h"              // header file for example user menu functions


serialMonLogClass smLog;        // object used to print log messages to serial monitor (see SerialMonLog.h) [Don't change name!]
serialMonCmdClass smCmd;        // object used to implement serial monitor command menus (SerialMonCommands.h)

  // Variables and constants used to implement this example program
elapsedMillis sysTimer;         // ms-resolution free-running "system timer" used to generate log message timestamps
elapsedMillis loopTimer;        // timer used to control rate of main loop execution
elapsedMillis logTimer;         // timer used to generate periodic log messages for example
const uint32_t loopPeriod = 10;     // 10-ms period for main loop
const uint32_t logMsgPeriod = 1000; // 1-second period for log messages
uint16_t logMsgNum;             // used to count log periodic messages
bool serialCmdEnable = true;    // variable that can be used to enable/disable command menus

void setup() {
  Serial.begin(115200);         // set baud rate for serial monitor
  while (!Serial);              // wait for serial monitor to initialize
  sysTimer = 0;                 // initialize the free-running system timer to 0
  smLog.setTimeStamp(&sysTimer);    // specify the system timer is to be used to generate log message timestamps
  smLog.logLevel = 1;           // set the log message criticality level to 1 (print messages with criticality of 0 - 1)
  smLog.enable = true;          // enable log message printing
  LOGMSG(1, "This should print: %u", 5);      // example log message, criticality level 1
  LOGMSG(2, "This shouldn't print: %u", 10);  // example log message, criticality level 2 (less critical)
  smCmd.initMenu(menuMain);     // specify root command menu (SerialUI.cpp)
  logMsgNum = 0;                // intialize log message counter used in main loop
  loopTimer = 0;                // initialize main loop timer
}

  // Very simple main loop to illustrate usage of log and menu functions
void loop() {
  if (loopTimer >= loopPeriod) {            // if it's time to execute the main loop functions (every 10ms)
    loopTimer = 0;                          // reinitialize the loop timer
    smCmd.processCommands(serialCmdEnable); // execute the current command menu (if enabled and active)
    smLog.enable = !smCmd.cmdMode;          // temporarily disable log messages when a command menu is active

    // Perform all other "normal" program functions here. 
    // For more guidance, see https://electricfiredesign.com/2021/03/18/simple-multi-tasking-for-arduino/

  }
  if (logTimer >= logMsgPeriod) {           // if it's time to generate a periodic log message (example)
    logTimer = 0;                           // reinitialize the log message timer
    LOGMSG(1, "log message %u", logMsgNum++); // print the example log message
  }
}


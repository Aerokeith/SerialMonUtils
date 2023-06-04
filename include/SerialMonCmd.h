#include <Arduino.h>
#include "SerialMonInput.h"

#ifndef _SERIALMONCOMMANDS_TYPES      // prevent multiple redefinition of types in this header
#define _SERIALMONCOMMANDS_TYPES

const char cmdModeChar = escChar;     // use ESC char as initial trigger for command mode
  // enum indicating the three different types of call to a user menu function
enum execTypeEnum {PROMPT,  // just print the cue/prompt for the user menu
                  COMMAND,  // parse and execute the menu-specific commands from the command line
                  ESCAPE};  // initiate transition to "next level up" menu

class serialMonCmdClass {
  void (*initMenuFuncP)(execTypeEnum execType);   // pointer to the "root" user menu function
  void (*menuFuncP)(execTypeEnum execType);       // pointer to the current user menu function
public:
  serialMonCmdClass() { cmdMode = false; menuFuncP = NULL; }  // class object constructor
  bool cmdMode;                             // indicates that menu command mode is active
  serialMonInputClass input;                // object used to read serial monitor input and assemble command line
  void processCommands(bool enable);
  void initMenu(void (*fP)(execTypeEnum));
  void nextMenu(void (*fP)(execTypeEnum));
  void menuPrompt(const char *prompt, const char *cue);
  void exit();
};

#endif  // _SERIALMONCOMMANDS_TYPES
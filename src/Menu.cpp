/* Menu.cpp
    Example implementation of user menu functions. The example includes a top-level (main) menu and one sub-menu
    that is accessed via a single-character command from the main menu. 
*/
#include <Arduino.h>
#include "SerialMonCmd.h"
#include "SerialMonLog.h"
#include "Menu.h"

extern serialMonCmdClass smCmd;       // object defined in main.cpp
extern serialMonLogClass smLog;       // object defined in main.cpp
char msgBuf[80];                      // temp buffer for assembling example output strings


/* menuMain()
    Example of a top-level user menu function. Called by serialMonCmdClass::processCommands() which provides a single 
    parameter of type execTypeEnum. This parameter specifies one of three execution scenarios for mainMenu():
      PROMPT - Just print a menu-specific prompt, using the serialMonCmdClass::menuPrompt() function
      COMMAND - Parse the command line that was assembled by processCommands() using the parsing functions provided by the 
                  serialMonInput class, and then execute the desired config/control actions. The actions could include
                  activating a lower-level command menu, using serialMonCmdClass:nextMenu().
      ESCAPE - Take appropriate actions to exit the current menu, such as to "pop up" a level in a multi-level menu structure.
                This is triggered when the user presses the <ESC> key when the command line is empty. In this example,
                <ESC> is not used to exit the main menu. Instead, an "exit" ('x') command is defined to do this. 
  Parameters: 
    execTypeEnum execType: see above
  Returns: None
*/
void menuMain(execTypeEnum execType) {
  char cmdChar;       // command character parsed from the command line buffer
  bool error;         // indicates error condition detected by parameter parsing functions
  float fParam[2];    // two floating-point parameters for the example 'f' command
  int iParam;         // one integer parameter for the example 'i' command

  switch (execType) {
    case PROMPT:        // Just print the menu prompt and return
      smCmd.menuPrompt("Main", "Commands: f <float> <float>, i <int>, t, x");   // example prompt/cue messages for this menu
    break;
        // Parse and execute a command line. Note that a loop could be used to parse multiple commands per command line
    case COMMAND:       
      cmdChar = smCmd.input.getCmdChar();   // find the first command character in the line 
      switch (cmdChar) {    // take actions based on the command character
        case 'f':   // example 'f' command that accepts two float parameters
          fParam[0] = smCmd.input.getFloatParam(&error);  // get the first float value and check for error
          if (error) {
            Serial.println("Invalid or missing float parameter");
            return;   // don't do anything else if error
          }
          fParam[1] = smCmd.input.getFloatParam(&error);  // get second float param
          if (error) {
            Serial.println("Invalid or missing float parameter");
            return;
          }           // both parameters read with no errors
            // assemble and print string to indicate what command is being executed
          sprintf(msgBuf, "Executing: f (%3.2f, %3.2f)", fParam[0], fParam[1]); // replace this with program-specific actions
          Serial.println(msgBuf);
        break;
        case 'i':   // example 'i' command that accepts one int parameter
          iParam = smCmd.input.getIntParam(&error);   // get int param and check for error
          if (error) {
            Serial.println("Invalid or missing integer parameter");
            return;
          }
          sprintf(msgBuf, "Executing: i (%i)", iParam);   // replace with program-specific action
          Serial.println(msgBuf);
        break;
        case 't':   // example 't' command that activates a lower-level menu 
          smCmd.nextMenu(menuLevel1);   // transition to menuLevel1
        break;
        case 'x':   // 'x' command to exit main menu and terminate menu command mode
          smCmd.exit();
        break;
        case '\0':  // if <RETURN> is pressed with empty line, discard line and do nothing
        break;
        default:  // any other cmdChar than those handled above
          Serial.print("Unknown command: ");
          Serial.println(cmdChar);
        break;
      }
    break;
    case ESCAPE:    // don't do anything for ESCAPE from main menu; Use the 'x' command to exit
    break;
    default:
    break;
  }
}


/* menuLevel1()
    Example of a lower-level user menu function. Takes action based on the parameter, similar to menuMain) above. 
    In this simple example, this menu implements no commands, and can be exited with <ESC> 
  Parameters: 
    execTypeEnum execType: see menuMain()
  Returns: None
*/
void menuLevel1(execTypeEnum execType) {
  char cmdChar;

  switch (execType) {
    case PROMPT:
      smCmd.menuPrompt("Test", "This is the test menu");
    break;
    case COMMAND:
      cmdChar = smCmd.input.getCmdChar();
      switch (cmdChar) {
        case '\0':
        break;
        default:
          Serial.print("Unknown command: ");
          Serial.println(cmdChar);
        break;
      }
    break;
    case ESCAPE:
      smCmd.nextMenu(menuMain);   // go back up to menuMain() if <ESC> is pressed with empty line
    break;
    default:
    break;
  }
}

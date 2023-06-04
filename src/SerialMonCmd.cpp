/* SerialMonCmd
    SerialMonCmd.h and SerialMonCmd.cpp implement the serialMonCmdClass, which implements functions that support the 
    creation of a multi-level command line menu utilizing the character I/O capabilities of the Serial Monitor. A command
    menu created using this class can be used to configure or control the operation of any program. A significant
    advantage over other methods is that all of the functions are non-blocking, meaning that they do not loop while
    waiting for user input or create any significant delay in normal program execution. The key requirement is that 
    the function processCommands() should be called from the program main loop at least 5 times per second. 
    The processCommands() function uses the capabilities of the serialMonInput class to read characters from the Serial
    Monitor and assemble them into command lines. It then calls a user-defined "menu" function to parse the command
    line and perform the desired configuration/control actions. Initiation and traversal of multi-level user menus is
    accomplished by using the initMenu() and nextMenu() functions, both of which take arguments that are pointers to user
    menu functions. Each user menu function must be defined as
      void myMenuFunc(execTypeEnum execType);
    The processCommands() function will call the current user menu function using one of three possible execTypeEnum values:
      PROMPT - The user menu function should take actions appropriate to prompt the user to enter a command line for a 
                  specific menu level, such as printing a prompt using the provided menuPrompt() function
      COMMAND - The menu function should parse the available command line using the parsing functions provided by the 
                  serialMonInput class, and then execute the desired config/control actions
      ESCAPE - The menu function should take appropriate actions to exit the current menu, such as to "pop up" a level in 
                  a multi-level menu structure. This is triggered when the user presses the <ESC> key when the command
                  line is empty. 
*/
#include <Arduino.h>
#include "SerialMonInput.h"
#include "SerialMonCmd.h"


/* serialMonCmdClass::processCommands()
  Parameters: 
    bool enable: if true, all command processing is inhibited and the function returns immediately
  Returns: None
*/
void serialMonCmdClass::processCommands(bool enable) {
  if (!enable)
    return;
  if (!cmdMode) {
    if (Serial.available()) {
      if (Serial.read() == cmdModeChar) {
        if (initMenuFuncP == NULL) {
          Serial.printf("\nRoot command menu has not been set!\n");
          return;
        }
        cmdMode = true;
        menuFuncP = initMenuFuncP;
        (*menuFuncP)(PROMPT);
      }
      else return;  // not in command mode. Received char, but not the one to trigger command mode
    }
    else return;  // not in command mode. No character received
  }
  else {    // already in command mode
    if (input.getCmdLine()) {     // terminated command line has been received
      if (menuFuncP == NULL) {
        Serial.printf("\nCommand menu has not been initialized!\n");
        return;
      }
      if (input.escape) {         // if escape char received to pop up a menu level
        Serial.println();         // start new line to prepare for new prompt
        (*menuFuncP)(ESCAPE);     // call current menu to determine "next level up" menu
        (*menuFuncP)(PROMPT);     // print the prompt for the new menu
      }
      else {                      // command line received, no escape char
        (*menuFuncP)(COMMAND);    // call the menu to execute the command
        input.clearLine();        // clear the command line
        if (*menuFuncP != NULL)   // if command didn't result in exit from command mode
          (*menuFuncP)(PROMPT);   // call the menu to print the menu prompt
      }
    }
  }
}


/* serialMonCmdClass::menuPrompt()
    Prints two strings as a prompt for the user to enter a comand line for a specific menu level. The "cue" string is
    printed first (on a separate line), and can be used to list the commands that are available for this menu. The 
    "prompt" string is printed as "<prompt>-> ", without a CR/NL, and can be used to indicate the menu name or any
    type of context information. 
  Parameters: 
    const char *prompt: pointer to a string (may be empty) containing a menu-specific prompt string
    const char *cue: pointer to a string (may be empty) containing a menu-specific cue string
  Returns: None
*/
void serialMonCmdClass::menuPrompt(const char *prompt, const char *cue) {
  if (strlen(cue) > 0) 
    Serial.println(cue);
  Serial.print('<');
  Serial.print(prompt);
  Serial.print(">-> ");
}


/* serialMonCmdClass::initMenu()
    Used to specify the user menu function that should be called when user menu command mode (cmdMode) is initially
    activated. When command mode is activated, the specified function will be called with a parameter value 
    (execTypeEnum) of PROMPT. 
  Parameters: 
    void (*fp)(execTypeEnum): pointer to a user menu function that accepts an execTypeEnum parameter
  Returns: None
*/
void serialMonCmdClass::initMenu(void (*fP)(execTypeEnum)) {
  initMenuFuncP = fP;
}


/* serialMonCmdClass::nextMenu()
    Causes an immediate transition to a specified user menu function (other than the current one). Typically 
    used to transition to a sub-menu (in a multi-level menu tree) as part of executing a menu command. Also
    used to "pop up" a menu level when handling a menu ESCAPE condition. The specified function is called with 
    a parameter value (execTypeEnum) of PROMPT. 
  Parameters: 
    void (*fp)(execTypeEnum): pointer to a user menu function that accepts an execTypeEnum parameter
  Returns: None
*/
void serialMonCmdClass::nextMenu(void (*fP)(execTypeEnum)) {
  menuFuncP = fP;
}


/* serialMonCmdClass::exit()
    Causes an immediate exit from menu command mode.
  Parameters: None
  Returns: None
*/
void serialMonCmdClass::exit() {
  Serial.println("Exiting command mode");
  cmdMode = false;
  menuFuncP = NULL;
}


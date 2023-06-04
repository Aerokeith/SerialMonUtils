/* SerialMonInput
    SerialMonInput.h and SerialMonInput.cpp implement the serialMonInputClass, which provides Serial Monitor input
    (and some output) functions that support the serialMonCmdClass (see SerialMonCmd.cpp) and user-defined command
    menus that utilize serialMonCmdClass. All of the functions are non-blocking, emaing that they never loop while
    waiting for user input. 
    The functions most likely to be used directly in the implementation of a user-defined command menu (UI) are:
      serialMonInputClass::getCmdChar() - fetch the next single-character command from a previously assembled command line.
      serialMonInputClass::getFloatParam() - fetch the next floating point parameter fromn the command line
      serialMonInputClass::getIntParam() - fetch the next integer parameter from the command line
*/

#include <Arduino.h>
#include "SerialMonInput.h"

/* serialMonInputClass::getCmdLine()
    Reads a single character from the serial monitor input (if available) and appends it to the end of the command line
    buffer (buf). Prior to the first call, assumes that bufP has been previously initialized to point to the head of 
    the buffer. If a newline ('\n') character is received, the function return value indicates that a complete command 
    line has been assembled and is ready for processing. Note: Pressing the <Return> key results in the two-character 
    sequence <CR><NL> = "\r\n". 
    If an <ESC> character is received, the response depends on whether or not the command line is currently empty. 
    If non-empty, the entire contents of the command buffer are deleted from the buffer and erased from the serial 
    monitor output. If empty, the return value indicates an end-of-line condition and also sets the boolean class
    variable "escape" to indicate that special processing is required. 
  Parameters: None
  Returns: 
    bool: True when a complete line has been received and is ready to be processed. The boolean class variable "escape"
          indicates that special processing is required. False if no character is available to read, or if any character
          other than '\n' is received.
*/
bool serialMonInputClass::getCmdLine() {
  bool eol;   // flag to indicate end-of-line detected
  char c;

  eol = false;                // no eol detected yet
  if (Serial.available()) {   // If anything is available to read
    c = Serial.read();        // read one char
    switch (c) {
      case '\r':              // don't do anything if <CR> or <TAB> is read
      case '\t':
      break;
      case '\b':              // if <BACKSPACE>, delete/erase the previous character
        eraseChar();
      break;
      case '\n':              // if <NEWLINE> received  
        eol = true;           // this indicates end of line
        escape = false;       // no special processing required
        bufP = buf;           // reset bufP to the head of the buffer for the next command line
        Serial.println();     // move output cursor to start of next line
      break;
      case escChar:           // if <ESC> char received
        if (strlen(buf) > 0)  // if the buffer isn't empty
          clearLine();        // delete/erase entire line, and contine assembling more chars
        else {                // <ESC> was pressed when buffer is empty
          eol = true;         // indicate end-of-line condition
          escape = true;      // with special processing
        }
      break;
      default:                // any other character was received
        *bufP = c;            // append it to the buffer
        bufP++;               // advance the pointer
        *bufP = '\0';         // add null terminator
        Serial.print(c);      // echo the char to the serial monitor output
      break;
    }
  }
  return eol;                 // return value as determined above
}


/* serialMonInputClass::getCmdChar()
    Returns the next non-blank character in the command line buffer (buf), assuming that bufP has previously been
    set (by getCmdLine) to point to the first character in the buffer. Leaves bufP pointing to the next
    character in the buffer.
  Parameters: None
  Returns: 
    char: next non-blank character in the command line buffer past the current position of bufP. A null character
      '\0' is returned if there are no more characters to read. 
*/
char serialMonInputClass::getCmdChar() {
  char retVal;
  skipBlanks();         // find next non-blank char
  retVal = *bufP;       // get the char (which could be '\0')
  if (*bufP != '\0') {  // if not at end of string
    bufP++;             // advance pointer to next char
  }
  return retVal;        // return the character found
}


/* serialMonInputClass::getFloatParam()
    Finds and converts the next floating point value (string) in the command line buffer. Updates the bufP to the character
      just past the float string. 
    Parameters: 
      bool *error: referenced boolean set to true if no valid floating point value was found in buffer
    Returns: 
      float: Converted substring
*/
float serialMonInputClass::getFloatParam(bool *error) {
  char *endPtr;
  float retVal;

  if (!scanToNum()) {   // scan in the buffer to find a valid floating point string
    *error = true;      // no float value found
    return (0);         // return 0
  }
  retVal = strtof(bufP, &endPtr);   // convert the string found, and get pointer to next char past the float
  bufP = endPtr;                    // move bufP past the float
  *error = false;     // no error
  return (retVal);    // return converted value
}


/* serialMonInputClass::getIntParam()
    Finds and converts the next integer value (string) in the command line buffer. Updates the bufP to the character
      just past the integer string. 
    Parameters: 
      bool *error: referenced boolean set to true if no valid floating point value was found in buffer
    Returns: 
      int: Converted substring
*/
int serialMonInputClass::getIntParam(bool *error) {
  char *endPtr;
  float retVal;

  if ((!scanToNum()) || (*bufP == '.')){   // scan in the buffer to find a valid integer string
    *error = true;      // no integer value found
    return (0);         // return 0
  }
  retVal = (int) strtol(bufP, &endPtr, 10);   // convert the string found, and get pointer to next char past the float
  bufP = endPtr;      // move bufP past the float
  *error = false;     // no error
  return (retVal);    // return converted value
}


/* serialMonInputClass::eraseChar()
    Deletes the character most recently added to the command line buffer (buf). Assumes that, if the buffer 
    isn't empty, bufP points to the next index in the string (which contains a '\0' string terminator). 
    This function also erases the character, which was previously printed (echoed) to the serial monitor 
    output. 
  Parameters: None
  Returns: None
*/
void serialMonInputClass::eraseChar() {
  if (bufP > buf) {         // if the command buffer isn't empty
    bufP--;                 // move pointer to the last non-null char in the buffer
    *bufP = '\0';           // replace the character with the null string terminator
    Serial.print("\b \b");  // <backspace><space><backspace> to erase character on output
  }
}


/* serialMonInputClass::clearLine()
    Deletes the entire contents of the command line buffer (buf) and erases all characters that have been
    previously echoed to the output
  Parameters: None
  Returns: None
*/
void serialMonInputClass::clearLine() {
  uint8_t n = strlen(buf);          // get the length of the command line buffer (not including terminator)
  for (uint8_t i = 0; i < n; i++)   // for each char
    eraseChar();                    // delete from buffer and erase previously-echoed output
}


/* serialMonInputClass::skipBlanks()
    Advances bufP from its current position in the buffer to the position of the next non-blank character
    or a null string terminator ('\0')
  Parameters: None
  Returns: None
*/
void serialMonInputClass::skipBlanks() {
  while (isblank(*bufP) && (*bufP != '\0')) {
    bufP++;
  }
}


/* scriptLineBufferClass::scanToNum()
    Moves the line buffer bufP until something that looks like the start of a floating point value is found. Some additional
      validation is performed to ensure a successful conversion of the string to a float value.  
    Parameters: None
    Returns:
      bool: True if a valid-looking float value is found
*/
bool serialMonInputClass::scanToNum() {
    // scan in buffer until a something looks like the start of a float (or end of line) is found
  while (!isdigit(*bufP) && (*bufP != '.') && (*bufP != '-') && (*bufP != '\0'))
    bufP++;
  if (*bufP == '\0')   // if end of line was encountered without finding anything that looks like a float
    return (false);
  if (!isdigit(*bufP)) {   // must have found '.' or '-'
    if (!isdigit(*(bufP+1)))
      return (false);   // next char after '.' or '-' should have been a digit
    else 
      return (true);    // everything looks OK
  }
  else {
    return (true);    // valid digit was found
  }
}



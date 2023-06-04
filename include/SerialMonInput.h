#include <Arduino.h>

#ifndef _SERIALMONINPUT_TYPES     // prevent multiple redefinition of types in this header
#define _SERIALMONINPUT_TYPES

const uint8_t maxInputLen = 80;   // maximum characters in a single command line (including terminating '\0')
const char escChar = '\x1B';      // ASCII ESC character, used for multiple purposes

class serialMonInputClass {
  char buf[maxInputLen];      // buffer to hold a single command line
  char *bufP;                 // pointer to buf, used both for adding and reading characters
  void skipBlanks();          
  bool scanToNum();
public:
  serialMonInputClass() { bufP = buf; *bufP = '\0'; }   // class object constructor; initialize buffer and bufP
  bool escape;                // indicates a command line containing only an ESC character
  bool getCmdLine();
  float getFloatParam(bool *error); 
  int getIntParam(bool *error); 
  void eraseChar();
  void clearLine();
  char getCmdChar();
};



#endif  // _SERIALMONINPUT_TYPES
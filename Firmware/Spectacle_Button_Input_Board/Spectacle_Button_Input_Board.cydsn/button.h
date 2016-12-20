#ifndef __servo_h_
#define __servo_h_
#include <project.h>  
#include <stdbool.h>

#pragma pack(1)
struct button
{
  uint8 channel;
  uint8 mode;
  uint8 button;
  uint8 pressed;
  uint8 released;
  uint8 latched;
  int32 buttonTimer;
};
#pragma pack()

enum {PRESS, RELEASE, PRESS_RELEASE, CONTINUOUS, LATCHING};

#endif
  

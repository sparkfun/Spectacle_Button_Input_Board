/****************************************************************************
button.h
Behavior struct definition file
Mike Hord @ SparkFun Electronics
24 Jan 2017
https://github.com/sparkfun/Spectacle_Button_Input_Board

Defines the struct which will be created to store each behavior received
from the director board.

Development environment specifics:
Developed in PSoC Creator 4.0

This code is beerware; if you see me (or any other SparkFun employee) at the
local, and you've found our code helpful, please buy us a round!
****************************************************************************/

#ifndef __servo_h_
#define __servo_h_
#include <project.h>  

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
  

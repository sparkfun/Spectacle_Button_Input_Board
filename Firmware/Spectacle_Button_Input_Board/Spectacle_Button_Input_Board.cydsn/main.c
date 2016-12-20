#include <project.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "programming.h"
#include "spectacle.h"
#include "button.h"

// systemTimer is incremented in the tickISR, which occurs once a millisecond.
//  It's the timebase for the entire firmware, upon which all other timing
//  is based.
volatile int32 systemTimer = 0;
CY_ISR_PROTO(tickISR);

// servo is a struct which tracks the desired behavior of a single channel. A
// single motor may have more than one servo struct associated with it, but
// only one servo struct should associate with a channel coming into the board.
// behaviors is the list of behaviors that the director board has passed into
// the servo board.
struct button *behaviors;

// behaviorListLen is the variable which tracks the number of behaviors that
// the director board has passed into the servo board. During normal operation,
// we will iterate over the object 'behaviors' 'behaviorListLen' times every 10
// milliseconds.
int behaviorListLen = 0;

// mailboxes is where our channel data comes in. A spectacle system can have
// up to 64 channels of behaviors, each of which is an int16.
int16 mailboxes[128];

volatile uint8 *I2C_Mem;

uint8 (*btnRead[9])(void) = {B_0_Read, B_1_Read, B_2_Read, B_3_Read,
                             B_4_Read, B_5_Read, B_6_Read, B_7_Read,
                             B_8_Read};

#define I2C_BUFFER_SIZE 256
#define I2C_BUFFER_RW_BOUNDARY 256

int main()
{
  CyGlobalIntEnable; /* Enable global interrupts. */
  
  EZI2C_Start();
  EZI2C_EzI2CSetBuffer1(I2C_BUFFER_SIZE, I2C_BUFFER_RW_BOUNDARY, (uint8*)mailboxes);
  
  UART_Start();
  
  CyIntSetSysVector((SysTick_IRQn + 16), tickISR);
  SysTick_Config(48000);

  behaviors = malloc(64*sizeof(struct button));
  I2C_Mem = (uint8*)mailboxes;
  
  int32 _100HzTick = 0;
  int32 _2HzTick = 0;
  int i = 0;
  
  UART_UartPutString("Hello world");
  LED_Write(1);

  for(;;)
  {
    // One hundred times per second,
    if ((systemTimer - 10) > _100HzTick)
    {
      _100HzTick = systemTimer;
      if (I2C_Mem[PROG_ENABLE_REG] == 1)
      {
        program();
      }
      if (I2C_Mem[CONFIGURED_REG] == 1)
      {
        EZI2C_EzI2CSetAddress1(I2C_Mem[I2C_ADDR_REG]);
        I2C_Mem[CONFIGURED_REG] = 0;
        I2C_OUT_EN_Write(1);
      }
      

      // behaviors loop: now that we've collected our data from the
      // accelerometer and parsed it into usable chunks, we have to determine
      // what to do with that information.
      for (i = 0; i < behaviorListLen; i++)
      {
        switch (behaviors[i].mode)
        {
          case PRESS:
            if (behaviors[i].pressed == 1)
            {
              if (systemTimer > behaviors[i].buttonTimer + 100)
              {
                mailboxes[behaviors[i].channel] = 0;
                if (btnRead[behaviors[i].button]() != 0)
                {
                  behaviors[i].pressed = 0;
                }
              }
            }
            else
            {
              if (btnRead[behaviors[i].button]() == 0)
              {
                behaviors[i].pressed = 1;
                behaviors[i].buttonTimer = systemTimer;
                mailboxes[behaviors[i].channel] = 1000;
              }
            }
            break;

          case RELEASE:
            if (btnRead[behaviors[i].button]() == 0)
            {
              behaviors[i].pressed = 1;
            }
            else
            {
              if (behaviors[i].pressed == 1)
              {
                behaviors[i].pressed = 0;
                behaviors[i].released = 1;
                behaviors[i].buttonTimer = systemTimer;
                mailboxes[behaviors[i].channel] = 1000;
              }
              else
              {
                if (behaviors[i].released == 1)
                {
                  if (behaviors[i].buttonTimer + 100 < systemTimer)
                  {
                    behaviors[i].released = 0;
                    mailboxes[behaviors[i].channel] = 0;
                  }
                }
              }
            }

            break;

          case PRESS_RELEASE:
            if ((btnRead[behaviors[i].button]() == 0) &&
                (behaviors[i].pressed == 0)           &&
                (behaviors[i].released == 0)        )
            {
              behaviors[i].pressed = 1;
              behaviors[i].buttonTimer = systemTimer;
              mailboxes[behaviors[i].channel] = 1000;
            }
            else if ((btnRead[behaviors[i].button]() == 0) &&
                (behaviors[i].pressed == 1)                &&
                (behaviors[i].released == 0)               &&
                (behaviors[i].buttonTimer + 100 < systemTimer))
            {
              mailboxes[behaviors[i].channel] = 0;
            }
            else if ((btnRead[behaviors[i].button]() == 1) &&
                (behaviors[i].pressed == 1)           &&
                (behaviors[i].released == 0)        )
            {
              behaviors[i].pressed = 0;
              behaviors[i].released = 1;
              behaviors[i].buttonTimer = systemTimer;
              mailboxes[behaviors[i].channel] = 1000;
            }
            else if ((btnRead[behaviors[i].button]() == 1) &&
                (behaviors[i].pressed == 0)                &&
                (behaviors[i].released == 1)               &&
                (behaviors[i].buttonTimer + 100 < systemTimer))
            {
              mailboxes[behaviors[i].channel] = 0;
              behaviors[i].released = 0;
            }
            break;

          case CONTINUOUS:
            if (behaviors[i].pressed == 1)
            {
              if (systemTimer > behaviors[i].buttonTimer + 50)
              {
                if (btnRead[behaviors[i].button]() == 1)
                {
                  mailboxes[behaviors[i].channel] = 0;
                  behaviors[i].pressed = 0;
                }
                else
                {
                  behaviors[i].buttonTimer = systemTimer;
                }
              }
            }
            else
            {
              if (btnRead[behaviors[i].button]() == 0)
              {
                behaviors[i].pressed = 1;
                behaviors[i].buttonTimer = systemTimer;
                mailboxes[behaviors[i].channel] = 1000;
              }
            }
            break;

          case LATCHING:
            if ((btnRead[behaviors[i].button]() == 0) &&
                (behaviors[i].latched == 0)           &&
                (behaviors[i].pressed == 0)           &&
                (behaviors[i].released == 0)        )
            {
              behaviors[i].latched = 1;
              behaviors[i].pressed = 1;
              mailboxes[behaviors[i].channel] = 1000;
            }
            else if ((btnRead[behaviors[i].button]() == 1) &&
                     (behaviors[i].latched == 1)           &&
                     (behaviors[i].pressed == 1)           &&
                     (behaviors[i].released == 0)         )
            {
              behaviors[i].pressed = 0;
              behaviors[i].released = 1;
            }
            else if ((btnRead[behaviors[i].button]() == 0) &&
                     (behaviors[i].latched == 1)           &&
                     (behaviors[i].pressed == 0)           &&
                     (behaviors[i].released == 1)         )
            {
              behaviors[i].released = 0;
              behaviors[i].latched = 0;
              behaviors[i].pressed = 1;
              mailboxes[behaviors[i].channel] = 0;
            }
            else if ((btnRead[behaviors[i].button]() == 1) &&
                     (behaviors[i].latched == 0)           &&
                     (behaviors[i].pressed == 1)           &&
                     (behaviors[i].released == 0)         )
            {
              behaviors[i].pressed = 0;
            }
            break;
        }
      }
    } // End of 100Hz loop

    // One Hz blinking LED heartbeat. Will probably be removed from production
    //  code.
    if ((systemTimer - 500) > _2HzTick)
    {
      _2HzTick = systemTimer;
      if (LED_Read() != 0)
      {
        LED_Write(0);
      }
      else
      {
        LED_Write(1);
      }
    }
  }
}

CY_ISR(tickISR)
{
  systemTimer++;
}



/* [] END OF FILE */

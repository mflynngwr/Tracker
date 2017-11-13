
#include <string.h>
#include "Tracker_if.h"

void Tracker_Platform_Init (void);
void task1(tEventID event);
void task2(tEventID event);
void testEventQ(void);
void testTimers (void);

static char *dbgmsg1 = "Debug message 1 : ";
static char *dbgmsg2 = "Debug message 2 : ";
static char *dbgmsg3 = "Unexpected Event : ";
   
int main (void)
{
   tEventID event, evtGroup;
   
   Tracker_Platform_Init ();
   Tracker_IF_Init ();
   
   startTimer (Debug1, 1000, true);
   startTimer (Debug2, 2500, true);
   
   while (1)
   {
      waitEvent(10);
      event = dequeueEvent();
      evtGroup = event & EventGroupMask;
          
      switch (evtGroup)
      {
         case Debug:
            task1 (event);
            break;
         
         case MotorOp:
         case EncoderOp:
            motorEvent (event);
            break;
         
         case FrontButton:
         case ModeButton:
         case GPS:
         case WiFi:
            task2 (event);
            break;
         
         case EmptyEvent:
            break;
      }
   }
}

void task1 (tEventID event)
{
   switch (event)
   {
      case Debug1:
         DEBUG_PRINT(dbgmsg1);
         break;
      case Debug2:
         DEBUG_PRINT(dbgmsg2);
         break;
   }
}

void task2 (tEventID event)
{
   DEBUG_PRINT(dbgmsg3);
}
#if 0
void testEventQ (void)
{
   queueEvent (FrontButtonUp);
   queueEvent (ModeButtonUp);
   queueEvent (GPS);
   queueEvent (WiFi);
   queueEvent (Encoder);
   dequeueEvent ();
   dequeueEvent ();
   queueEvent (FrontButtonUp);
   queueEvent (ModeButtonUp);
   dequeueEvent ();
   dequeueEvent ();
   dequeueEvent ();
   dequeueEvent ();
   dequeueEvent ();
}

void testTimers (void)
{
   startTimer (FrontButton, 500, NoRepeat);
   startTimer (ModeButton, 600, NoRepeat);
   startTimer (GPS, 400, NoRepeat);
   startTimer (WiFi, 700, NoRepeat);
   startTimer (Encoder, 500, NoRepeat);
   stopTimer  (GPS);
   stopTimer  (FrontButton);
   stopTimer  (WiFi);
   startTimer (FrontButton, 500, Repeat);
   stopTimer  (FrontButton);
}
#endif


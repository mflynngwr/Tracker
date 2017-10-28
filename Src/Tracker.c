
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

   startPWM ();
   
//   testEventQ();
//   testTimers();
   
   startTimer (DebugEvent1, 256, Repeat);
   startTimer (DebugEvent2, 768, Repeat);
   
   while (1)
   {      
      waitEvent(100);
      event = dequeueEvent();
      evtGroup = event & EventGroupMask;
          
      switch (evtGroup)
      {
         case Debug:
            task1 (event);
            break;
         
         case FrontButton:
         case ModeButton:
         case Encoder:
         case Zigbee:
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
      case DebugEvent1:
         DEBUG_PRINT(dbgmsg1);
         break;
      case DebugEvent2:
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
   queueEvent (Zigbee);
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
   startTimer (Zigbee, 400, NoRepeat);
   startTimer (WiFi, 700, NoRepeat);
   startTimer (Encoder, 500, NoRepeat);
   stopTimer (Zigbee);
   stopTimer (FrontButton);
   stopTimer (WiFi);
   startTimer (FrontButton, 500, Repeat);
   stopTimer (FrontButton);
}
#endif


/*
 ******************************************************************************
 * @file    Tracker_if.c
 * @brief   Tracker Platform Independent Applicaton Interface Functions
 ******************************************************************************
 *
 * COPYRIGHT(c) 2017 Apeiros, LLC
 *
 ******************************************************************************
*/

#include "Tracker_if.h"

static tEvents events;
static tTimers timers;

static void addTimerIdx (int addidx);

//*****************************************************************************
//*
//* Tracker_IF_Init
//* Initialize the Tracker interface data structures
//*
//*****************************************************************************

void Tracker_IF_Init (void)
{
   // Initialize the Event Queue
   events.head = 0;
   events.cnt  = 0;
   events.lost = 0;
   events.lock = NoLock;

   // Initialize the Timer List
   timers.cnt    =  0;
   timers.active = -1;
   timers.inactive  =  0;

   for (int i=0; i<TIMER_LIST_SIZE; i++)
   {
      timers.next[i] = i+1;
      timers.event[i] = EmptyEvent;
   }
      timers.next[TIMER_LIST_SIZE-1] = -1;
}

//*****************************************************************************
//*
//* waitEvent
//* ---
//*
//*****************************************************************************

void waitEvent (timeMS timeWait)
{
   if (timeWait <= MINIMUM_EVENT_WAIT_TIME)
   {
      timeWait = MINIMUM_EVENT_WAIT_TIME;
   }

   timeWait += getTime();
   do
   {
      // Check active timers
      checkTimer ();
   } while ((!events.cnt) && (getTime() >= timeWait));
}

//*****************************************************************************
//*
//* queueEvent
//* Add an event to the queue for processing in the main loop
//*
//* Expectation is adding of events to the queue will be primarily done
//*   within an interrupt handler. If interupts are nested, there is a 
//*   chance that an event will be lost. The "lost" counter is maintained
//*   to detect these situations and can be checked during debug.
//*
//*****************************************************************************

tEventStatus queueEvent (tEventID event)
{
   tEventStats rtnStatus = Error;

   if (!events.lock && (events.cnt < EVENT_QUEUE_SIZE))
   {
      events.lock = Lock;

      events.event[(events.head + events.cnt) % EVENT_QUEUE_SIZE] = event;
      events.cnt++;

      events.lock = NoLock;
      rtnStatus = Success;
   }
   else
   {
      events.lost++;
   }
   return rtnStatus;
}

//*****************************************************************************
//*
//* dequeueEvent
//* ---
//*
//*****************************************************************************

tEventID dequeueEvent (void)
{
   tEventID rtnEvent = EmptyEvent;

   if (events.cnt)
   {
      while (events.lock) {};

      events.lock = Lock;

      rtnEvent = events.event[events.head];
      events.head = (events.head+1) % EVENT_QUEUE_SIZE;
      events.cnt--;

      events.lock = NoLock;
   }
   return rtnEvent;
}

//*****************************************************************************
//*
//* checkTimer
//* ---
//*
//*****************************************************************************

void checkTimer (void)
{
   int chkidx = timers.active;
   timeMS curTime = getTime();

   if ((timers.cnt > 0) && (curTime >= timers.time[chkidx]))
   {
      // Queue the timer event
      queueEvent(timers.event[chkidx]);

      // Remove the timer from the active list
      timers.active = timers.next[chkidx];
      timers.cnt--;

      // If timer is to repeat, add it back into the active timer list
      if (timers.delay[chkidx] != 0)
      {
#define NO_DRIFT
#ifdef  NO_DRIFT

         // Update timer for current event
         do
         {  // Update timer relative to last timer value
            timers.time[chkidx] += timers.delay[chkidx];
         } while (timers.time[chkidx] <= curTime);
#else
         // Update timer relative to current time
         timers.time[chkidx] =  timers.delay[chkidx] + currTime;
#endif
         // Add updated timer to the active timer list
         addTimerIdx (chkidx);
      }
      else
      {
         // Return current timer to the inactive timer list
         timers.next[chkidx] = timers.inactive;
         timers.inactive = chkidx;
      }
   }
}

//*****************************************************************************
//*
//* startTimer
//* ---
//*
//*****************************************************************************

bool startTimer (tEventID event, timeMS delay, bool repeat)
{
   int addidx;
   timeMS eventTime = delay + getTime();
   tEventStatus rtnStatus = Error;
   
   // Remove any duplicate timer currently in the active list
   stopTimer (event);

   // Confirm timer list is not full
   if (timers.cnt < TIMER_LIST_SIZE)
   {
      // Get timer node from the inactive list
      addidx = timers.inactive;
      timers.inactive = timers.next[addidx];

      // Initialize timer values
      timers.event[addidx] = event;
      timers.delay[addidx] = (repeat) ? delay : 0;
      timers.time [addidx] = eventTime;

      // Add timer to active timer list
      addTimerIdx (addidx);
      rtnStatus = Success;
   }
   return rtnStatus;
}

//*****************************************************************************
//*
//* stopTimer
//* ---
//*
//*****************************************************************************

void stopTimer (tEventID event)
{
   int delidx, idx = timers.active;

   // Confirm timer list is not empty
   if (timers.cnt > 0)
   {
      // If timer found at beginning of the active timer list
      if (timers.event[idx] == event)
      {
         // Remove timer from begining of the active timer list
         delidx = timers.active;
         timers.active = timers.next[idx];
         timers.cnt--;

         // Return current timer to the inactive timer list
         timers.next[delidx] = timers.inactive;
         timers.inactive = delidx;
      }
      else
      {
         while (timers.next[idx] != -1)
         {
            // Check if timer is found at the next active timer entry
            if (timers.event[timers.next[idx]] == event)
            {
               // Timer match found, remove from the active timer list
               delidx = timers.next[idx];
               timers.next[idx] = timers.next[delidx];
               timers.cnt--;

               // Return current timer to the inactive timer list
               timers.next[delidx] = timers.inactive;
               timers.inactive = delidx;
               break;
            }
            idx = timers.next[idx];
         }
      }
   }
}

//*****************************************************************************
//*
//* addTimerIdx
//* ---
//*
//*****************************************************************************

static void addTimerIdx (int addidx)
{
   int idx = timers.active;
   timeMS eventTime = timers.time[addidx];

   // Check if active timer list is empty
   if (timers.cnt == 0)
   {
      // New timer is first one in the active timer list
      timers.active = addidx;
      timers.next[addidx] = -1;
   }
   // Check if new timer has the earliest time in the list
   else if (eventTime < timers.time[idx])
   {
      // Add new timer to begining of the active timer list
      timers.next[addidx] = timers.active;
      timers.active = addidx;
   }
   // Add new timer into the active timer list
   else
   {
      while (1)
      {
         // Check for end of active timer list
         // OR if new timer to be inserted at current list position
         if ((timers.next[idx] == -1)
         || (eventTime <= timers.time[timers.next[idx]]))
         {
            // Add new timer at current position
            timers.next[addidx] = timers.next[idx];
            timers.next[idx] = addidx;
            break;
         }
         // Move to next position in the active timer list
         idx = timers.next[idx];
      }
   }
   timers.cnt++;
}

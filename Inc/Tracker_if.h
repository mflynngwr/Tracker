/*
 ******************************************************************************
 * @file    Tracker_if.h
 * @brief   Tracker Applicaton Interface Functions
 ******************************************************************************
 *
 * COPYRIGHT(c) 2017 Apeiros, LLC
 *
 ******************************************************************************
*/
#ifndef TRACKER_IF_H
#define TRACKER_IF_H

#include <stdbool.h>

/******************************************************************************
 * Debug print output controled by #define NO_DEBUG_PRINT macro
 * When NO_DEBUG_PRINT define, all debug printing is diabled
 *****************************************************************************/

//#define NO_DEBUG_PRINT
#ifdef NO_DEBUG_PRINT
#define DEBUG_PRINT(msg) (void)0U
#else
#define DEBUG_PRINT(msg) debugPrint(msg)
#endif

/******************************************************************************
 *
 * Tracker Interface macros
 *
 *****************************************************************************/

#define MAX_TIME_VAL_MS 0x7FFFFFFFU
#define PWM_PERIOD_PRESCALE  36000-1     /* Yields 2KHz clock to PWM period */
#define PWM_PERIOD 200                   /* 100 ms PWM period               */
#define PAN_PWM_INIT  0                  /*  */
#define TILT_PWM_INIT 0                  /*  */
#define UTILITY_TIMER_PRESCALE 36000     /* Yields 2KHz clock to timer      */
#define UTILITY_TIMER_PERIOD   100       /* 50 ms timer                     */


/******************************************************************************
 *
 * Tracker Events Definitions
 *
 *****************************************************************************/
 
typedef unsigned int timeMS;

typedef enum
{
	Error   = 0,
	Success = 1,
	NoLock  = 0,
	Lock    = 1,
	QueueNoPending = 0,
	QueuePending   = 1,
	TimersFull,
} tEventStats;

typedef unsigned int tEventStatus;

typedef enum 
{
	EmptyEvent     = 0x1000000,
	Debug          = 0x1010000,
	DebugEvent1    = 0x1010001,
	DebugEvent2    = 0x1010002,
	FrontButton    = 0x1020000,
	FrontButtonUp  = 0x1020001,
	FrontButtonDn  = 0x1020002,
	ModeButton     = 0x1030000,
	ModeButtonUp   = 0x1030001,
	ModeButtonDn   = 0x1030002,
	Encoder        = 0x1040000,
	EncoderUpdate  = 0x1040001,
	Zigbee         = 0x1050000,
	ZigbeeInput    = 0x1050001,
	WiFi           = 0x1060000,
	WiFiInput      = 0x1060001,
	Repeat         = 0x8000000,
	NoRepeat       = 0x0000000,
	EventGroupMask = 0xFFF0000
} tEventIDs;

typedef unsigned int tEventID;
	
// Event Queue size needs to be power of 2
#define EVENT_QUEUE_SIZE 10U
#define MINIMUM_EVENT_WAIT_TIME 10

typedef struct
{
   unsigned int head;
   unsigned int cnt;
   unsigned int lost;
   tEventStatus lock;
   tEventID event[EVENT_QUEUE_SIZE];
} tEvents;

#define TIMER_LIST_SIZE 6U

typedef struct
{
	int cnt;
	int active;
	int inactive;
	int next [TIMER_LIST_SIZE];
	timeMS time [TIMER_LIST_SIZE];
	timeMS delay[TIMER_LIST_SIZE];
	tEventID event[TIMER_LIST_SIZE];
} tTimers;


/******************************************************************************
 *
 * Tracker interface function declarations (platform independent
 *
 *****************************************************************************/

void Tracker_IF_Init (void);

tEventStatus queueEvent (tEventID event);
tEventID dequeueEvent (void);

void stopTimer  (tEventID event);
bool startTimer (tEventID event, timeMS delay, bool repeat);

void debugPrint (char* msg);
void delay_ms   (timeMS timems);
void waitEvent  (timeMS time_ms);
void checkTimer (void);
timeMS getTime  (void);
void startPWM   (void);

#endif

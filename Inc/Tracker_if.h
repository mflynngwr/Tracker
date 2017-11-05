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

#define TRACKER_SYS_CLOCK  72000000
#define MAX_TIME_VAL_MS 0x7FFFFFFFU

#define PWM_PERIOD_DEFAULT     10          /* Default PWM Period (cycles/sec) */
#define PWM_DUTYCYCLE_DEFAULT   0          /* Default PWM Duty Cycle          */
#define PWM_PERIOD_MIN          1          /* Motor PWM Minimum Period        */
#define PWM_PERIOD_MAX        100          /* Motor PWM Maximum Period        */
#define PWM_DUTYCYCLE_MIN       0          /* Motor Duty Cycle Minimum        */
#define PWM_DUTYCYCLE_MAX     100          /* Motor Duty Cycle Maximum        */

#define PAN_IDX 0
#define TILT_IDX 1
 

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
	EmptyEvent      = 0x1000000,
    TimeOut         = 0x1000001,
	Debug           = 0x1010000,
	Debug1          = 0x1010001,
	Debug2          = 0x1010002,
	FrontButton     = 0x1020000,
	FrontButtonUp   = 0x1020001,
	FrontButtonDn   = 0x1020002,
	ModeButton      = 0x1030000,
	ModeButtonUp    = 0x1030001,
	ModeButtonDn    = 0x1030002,
	MotorOp         = 0x1040000,
	MotorOpFinish   = 0x1040001,
	MotorOpTest     = 0x10400FF,
	TiltOpFinish    = 0x1040003,
	EncoderOp       = 0x1040000,
	GPS             = 0x1060000,
	GPSUpdate       = 0x1060001,
	WiFi            = 0x1080000,
	WiFiInput       = 0x1080001,
	EventGroupMask  = 0xFFF0000
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
 * Tracker Motor Control declarations
 *
 *****************************************************************************/

 typedef enum 
{
	NoDir,
	CW,
	CCW,
	Brake
} tMotorDirs;

typedef enum 
{
	MotorEnabled,
	MotorDisabled,
	MotorOpPending,
	NoMotorOpPending,
} tMotorStates;

typedef unsigned int tMotorDir;
typedef unsigned int tMotorState;

typedef struct
{
   int         curPeriod;
   int         curDutyCycle[2];
   tMotorDir   curDir[2];
   tMotorState stateMotor;
} tMotorControl;

/******************************************************************************
 *
 * Tracker interface function declarations
 *
 *****************************************************************************/

void Tracker_IF_Init (void);
void Tracker_MotorCtl_Init (void);

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

void enableMotorOp (void);
void disableMotorOp (void);
void setMotorPeriod (int period);
void setPanDir (tMotorDir dir, int dutycylcle);
void setTiltDir (tMotorDir dir, int dutycylcle);
void motorEvent (tEventID event);
void initMotorControl (void);

void pfPWMInit (void);
void pfPWMEnable (void);
void pfPWMDisable (void);
void pfSetPeriod (int period);
void pfSetDir (int idx, tMotorDir dir, int dutyCycle);

#endif


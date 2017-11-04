/*
 ******************************************************************************
 * @file    Tracker_MotorCtl.c
 * @brief   Tracker Motor Control platform independent applicaton interface.
 ******************************************************************************
 *
 * COPYRIGHT(c) 2017 Apeiros, LLC
 *
 ******************************************************************************
*/

#include "Tracker_if.h"

static void setPWMDir (int idx, tMotorDir dir, int dutyCycle); 
static void finishMotorOp (void);
static void testMotorOp  (void);

static tMotorControl trackerMCtl;

#define TEST_MOTOR_OP
#ifdef TEST_MOTOR_OP
static int finishMotorCnt = 0;
static int testMotorCnt = 0;
#endif

//*****************************************************************************
//*
//* initMotorControl
//* ---
//*
//*****************************************************************************
   
void initMotorControl (void)
{
   trackerMCtl.curDir[0]   = NoDir;
   trackerMCtl.curDir[1]   = NoDir;
   trackerMCtl.curPeriod   = PWM_PERIOD_DEFAULT;
   
   trackerMCtl.curDutyCycle[0]  = PWM_DUTYCYCLE_DEFAULT;
   trackerMCtl.curDutyCycle[1]  = PWM_DUTYCYCLE_DEFAULT;
   
   platformPWMInit ();  
}

//*****************************************************************************
//*
//* motorEvent
//* ---
//*
//*****************************************************************************
   
void motorEvent (tEventID event)
{
   switch (event)
   {
      case MotorOpFinish :
         finishMotorOp ();
         break;
      
#ifdef TEST_MOTOR_OP      
      case MotorOpTest :
         testMotorOp ();
         break;
#endif
      
      default :
         break;
   }
}

//*****************************************************************************
//*
//* enableMotorOp
//* ---
//*
//*****************************************************************************
   
void enableMotorOp (int period)
{
   if (period <= PWM_PERIOD_MIN)
   {
      period = PWM_PERIOD_MIN;
   }
   else if (period > PWM_PERIOD_MAX)
   {
      period = PWM_PERIOD_MAX;
   }
   
   trackerMCtl.curPeriod = period;
   trackerMCtl.stateMotor = platformPWMEnable (period);
}

//*****************************************************************************
//*
//* disableMotorOp
//* ---
//*
//*****************************************************************************
   
void disableMotorOp ()
{
   if (trackerMCtl.stateMotor != MotorDisabled)
   {
      initMotorControl ();
   }
   
   trackerMCtl.stateMotor = MotorDisabled;
}

//*****************************************************************************
//*
//* setPanDir
//* ---
//*
//*****************************************************************************
   
void setPanDir (tMotorDir dir, int dutyCycle)
{
   setPWMDir (0, dir, dutyCycle);
}

//*****************************************************************************
//*
//* setTiltDir
//* ---
//*
//*****************************************************************************
   
void setTiltDir (tMotorDir dir, int dutyCycle)
{
   setPWMDir (1, dir, dutyCycle);
}

//*****************************************************************************
//*
//* setPWMDir
//* ---
//*
//*****************************************************************************
   
void setPWMDir (int idx, tMotorDir dir, int dutyCycle)
{
   if (dutyCycle <= PWM_DUTYCYCLE_MIN)
   {
      dutyCycle = PWM_DUTYCYCLE_MIN;
   }
   else if (dutyCycle > PWM_DUTYCYCLE_MAX)
   {
      dutyCycle = PWM_DUTYCYCLE_MAX;
   }

   trackerMCtl.stateMotor = platformSetDir (idx, dir, dutyCycle);
   
   trackerMCtl.curDir[idx]  = dir;
   trackerMCtl.curDutyCycle[idx]  = dutyCycle;
}

//*****************************************************************************
//*
//* finishMotorOp
//* ---
//*
//*****************************************************************************
   
void finishMotorOp (void)
{
   trackerMCtl.stateMotor = MotorEnabled;   
   
   if (trackerMCtl.nextDir[0] != NoDir)
   {
      setPanDir (trackerMCtl.nextDir[0], trackerMCtl.nextDutyCycle[0]);
   }
   
   if (trackerMCtl.nextDir[1] != NoDir)
   {
      setTiltDir (trackerMCtl.nextDir[1], trackerMCtl.nextDutyCycle[1]);
   }
   
#ifdef TEST_MOTOR_OP
   finishMotorCnt++;
#endif
}

#ifdef TEST_MOTOR_OP
//*****************************************************************************
//*
//* testMotorOp
//* ---
//*
//*****************************************************************************
   
void testMotorOp (void)
{
   static int dcPan = 0;
   static int dcTilt = 0;
   static int mtrPeriod = 0;
   
   switch (testMotorCnt)
   {
      case 0:
         enableMotorOp (mtrPeriod);
         mtrPeriod = (mtrPeriod + 10) % 100;
         setPanDir  (NoDir, 50);
         setTiltDir (Brake, 50);
         break;
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
         setPanDir  (CW, dcPan);
         setTiltDir (CCW, dcTilt);
         dcPan  = (dcPan + 10) % 100;
         dcTilt = (dcTilt + 90) % 100;
         break;
      default:
         break;
   }
   
   //startTimer (MotorOpTest, 60000, false);
   
   testMotorCnt = (testMotorCnt + 1) % 11;
}
#endif

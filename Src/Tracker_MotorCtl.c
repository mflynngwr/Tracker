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
static void testMotorOp (void); 
static void finishMotorOp (void);

static tMotorControl trackerMCtl;

#define TEST_MOTOR_OP
#ifdef TEST_MOTOR_OP
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
   pfPWMInit ();  
   
   disableMotorOp ();
   
   pfSetPeriod (PWM_PERIOD_DEFAULT);
   
   setPanDir (NoDir, PWM_DUTYCYCLE_DEFAULT);
   setTiltDir (NoDir, PWM_DUTYCYCLE_DEFAULT);
   setMotorPeriod (PWM_PERIOD_DEFAULT);
      
#ifdef TEST_MOTOR_OP      
   startTimer (MotorOpTest, 5000, true);
#endif
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

void enableMotorOp (void)
{
   pfPWMEnable();
   trackerMCtl.stateMotor = MotorEnabled;
}

//*****************************************************************************
//*
//* disableMotorOp
//* ---
//*
//*****************************************************************************
   
void disableMotorOp (void)
{
   pfPWMDisable();
   trackerMCtl.stateMotor = MotorDisabled;
}

//*****************************************************************************
//*
//* setMotorPeriod
//* ---
//*
//*****************************************************************************
   
void setMotorPeriod (int period)
{
   if (period <= PWM_PERIOD_MIN)
   {
      period = PWM_PERIOD_MIN;
   }
   else if (period > PWM_PERIOD_MAX)
   {
      period = PWM_PERIOD_MAX;
   }
   
   // Update PWM period
   trackerMCtl.curPeriod = period;
   pfSetPeriod (trackerMCtl.curPeriod);
}

//*****************************************************************************
//*
//* setPanDir
//* ---
//*
//*****************************************************************************
   
void setPanDir (tMotorDir dir, int dutyCycle)
{
   setPWMDir (PAN_IDX, dir, dutyCycle);
}

//*****************************************************************************
//*
//* setTiltDir
//* ---
//*
//*****************************************************************************
   
void setTiltDir (tMotorDir dir, int dutyCycle)
{
   setPWMDir (TILT_IDX, dir, dutyCycle);
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

   pfSetDir (idx, dir, dutyCycle);
   
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
   static int dcPan = 10;
   static int dcTilt = 50;
   static int dirPan = NoDir;
   static int dirTilt = NoDir;
   static int mtrPeriod = 1;
   
   switch (testMotorCnt)
   {
      case 0:
         setMotorPeriod (mtrPeriod);
         mtrPeriod = (mtrPeriod + 1) % 100;
         setPanDir  (NoDir, 50);
         setTiltDir (Brake, 50);
         dcPan  = (dcPan + 91) % 100;
         dcTilt = (dcTilt + 11) % 100;
         break;
      
      case 1:
         dirPan = CW;
         dirTilt = CCW;
         break;
      
      case 6:
         dirPan = CCW;
         dirTilt = CW;
         break;
      
      case 10:
         dirPan = CW;
         dirTilt = CCW;
         break;
      
      default:
         break;
   }
   
   setPanDir  (dirPan, dcPan);
   setTiltDir (dirTilt, dcTilt);
   dcPan  = (dcPan + 10) % 100;
   dcTilt = (dcTilt + 90) % 100;
   
   testMotorCnt = (testMotorCnt + 1) % 11;
}
#endif

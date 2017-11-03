/*
 ******************************************************************************
 * @file    stm32f3xx_motorctl.c
 * @brief   Tracker Motor Control platform dependent applicaton interface.
 ******************************************************************************
 *
 * COPYRIGHT(c) 2017 Apeiros, LLC
 *
 ******************************************************************************
*/
#include "main.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_if.h"

#define PWM_PERIOD_DIVISOR   200
#define PWM_PRESCALE_MAX   36000

static int curPreScaleCnt = PWM_PRESCALE_MAX;
static int curPeriodCnt   = PWM_PERIOD_DIVISOR;

static int curDC[2] = {PWM_TIMER_DC_INIT, PWM_TIMER_DC_INIT};
static tMotorDir curDir[2] = {NoDir, NoDir};

static unsigned int pwmDIR[2]  = {PAN_DIR_Pin,  TILT_DIR_Pin};
static unsigned int pwmDIRn[2] = {PAN_DIRn_Pin, TILT_DIRn_Pin};

static GPIO_TypeDef *pwmDIRPort[2]  = {PAN_DIR_GPIO_Port,  TILT_DIR_GPIO_Port};
static GPIO_TypeDef *pwmDIRnPort[2] = {PAN_DIRn_GPIO_Port, TILT_DIRn_GPIO_Port};

static int cntCallback = 0;

//*****************************************************************************
//*
//* platformPWMInit
//* ---
//*
//*****************************************************************************

void platformPWMInit (void)
{
   // Configure Motor interface to Power Save Mode
   HAL_GPIO_WritePin (MOTOR_PWRSAVE_GPIO_Port, MOTOR_PWRSAVE_Pin, GPIO_PIN_RESET);
   
   // Configure Pan and Tilt to NoDir state   
   HAL_GPIO_WritePin (PAN_DIR_GPIO_Port,   PAN_DIR_Pin,   GPIO_PIN_SET);
   HAL_GPIO_WritePin (PAN_DIRn_GPIO_Port,  PAN_DIRn_Pin,  GPIO_PIN_SET);
   HAL_GPIO_WritePin (TILT_DIR_GPIO_Port,  TILT_DIR_Pin,  GPIO_PIN_SET);
   HAL_GPIO_WritePin (TILT_DIRn_GPIO_Port, TILT_DIRn_Pin, GPIO_PIN_SET);
   curDir[0] = NoDir;
   curDir[1] = NoDir;
   
   // Clear any pending interrupts 
   htim2.Instance->SR   = ~TIM_FLAG_UPDATE;
   htim2.Instance->CR1 |= TIM_CR1_UDIS; 
   
   // Initialize Pan and Tilt duty cycle to default 
   htim2.Instance->ARR  = PWM_PERIOD_DIVISOR-1;
   htim2.Instance->PSC  = PWM_PRESCALE_MAX - 1;
   htim2.Instance->CCR1 = PWM_PERIOD_DIVISOR;
   htim2.Instance->CCR2 = PWM_PERIOD_DIVISOR;
   curPreScaleCnt = PWM_PRESCALE_MAX;
   curPeriodCnt   = PWM_PERIOD_DIVISOR;
   curDC[0] = PWM_PERIOD_DIVISOR;
   curDC[1] = PWM_PERIOD_DIVISOR;
   
   // Start the PWM Timer
   HAL_TIM_OC_Start (&htim2, TIM_CHANNEL_1);
   HAL_TIM_OC_Start (&htim2, TIM_CHANNEL_2);
   HAL_TIM_Base_Start_IT (&htim2);
   
   return;
}

//*****************************************************************************
//*
//* platformPWMEnable
//* ---
//*
//*****************************************************************************

tMotorState platformPWMEnable (int period)
{
   // Set Test Trigger
   HAL_GPIO_WritePin (FRONT_LED_GPIO_Port, FRONT_LED_Pin, GPIO_PIN_SET);

   // Configure Motor interface to Power Save Mode Off
   HAL_GPIO_WritePin (MOTOR_PWRSAVE_GPIO_Port, MOTOR_PWRSAVE_Pin, GPIO_PIN_SET);
      
   curPreScaleCnt = (int) ((float) (TRACKER_SYS_CLOCK / PWM_PERIOD_DIVISOR) / (float) period);
   curPeriodCnt   = PWM_PERIOD_DIVISOR;
   
   if (curPreScaleCnt > PWM_PRESCALE_MAX)
   {
      curPreScaleCnt = PWM_PRESCALE_MAX;
      curPeriodCnt = (int) ((float) (TRACKER_SYS_CLOCK / PWM_PRESCALE_MAX) / (float) period);
   }   

   htim2.Instance->ARR  = curPeriodCnt-1;
   htim2.Instance->PSC  = curPreScaleCnt-1;   
   htim2.Instance->CR1 &= ~TIM_CR1_UDIS;
   
   // Set Test Trigger
   HAL_GPIO_WritePin (FRONT_LED_GPIO_Port, FRONT_LED_Pin, GPIO_PIN_RESET);
   
   return MotorOpPending;
}

//*****************************************************************************
//*
//* platformSetPanDir
//* ---
//*
//*****************************************************************************

tMotorState platformSetPanDir (tMotorDir dir, int dutyCycle)
{
   curDC[0] = (int) ((float)(curPeriodCnt * dutyCycle) / 100.0F);
   
   htim2.Instance->CCR1 = (curDC[0] <= 1) ? 0 : curDC[0] - 1;  
   htim2.Instance->CR1 &= ~TIM_CR1_UDIS;
 
   return MotorOpPending;
}

//*****************************************************************************
//*
//* platformSetTiltDir
//* ---
//*
//*****************************************************************************

tMotorState platformSetDir (int idx, tMotorDir dir, int dutyCycle)
{
   bool needUpdate = false;
   int  newDC;
   
   // Set Test Trigger
   HAL_GPIO_WritePin (FRONT_LED_GPIO_Port, FRONT_LED_Pin, GPIO_PIN_SET);
   
   switch (dir)
   {
      case NoDir :
         dutyCycle =-1;
         break;
      case CW :
         if (curDir[idx] == CCW)
         {
            // Set PWM DIR, set update flag to true
            HAL_GPIO_WritePin (pwmDIRPort[idx], pwmDIR[idx], GPIO_PIN_SET);
            needUpdate = true;
         }
         else
         {
            // Reset DIRn
            HAL_GPIO_WritePin (pwmDIRnPort[idx], pwmDIRn[idx], GPIO_PIN_RESET);
         }
         curDir[idx] = CW;
         break;
         
      case CCW :
         if (curDir[idx] == CW)
         {
            // Set DIRn, set update flag to true
            HAL_GPIO_WritePin (pwmDIRnPort[idx], pwmDIRn[idx], GPIO_PIN_SET);
            needUpdate = true;
         }
         else
         {
            // Reset DIR
            HAL_GPIO_WritePin (pwmDIRPort[idx], pwmDIR[idx], GPIO_PIN_RESET);
         }
         curDir[idx] = CCW;
         break;
         
      case Brake :
         dutyCycle = 100;
         // Set DIR, set DIRn
         HAL_GPIO_WritePin (pwmDIRPort[idx], pwmDIR[idx], GPIO_PIN_SET);
         HAL_GPIO_WritePin (pwmDIRnPort[idx], pwmDIRn[idx], GPIO_PIN_SET);
         curDir[idx] = Brake;
         break;
      default :
         break;
   }
   // Avoid update interupt when updating timer compare register
   htim2.Instance->CR1 |= TIM_CR1_UDIS;
   
   // Calculate duty cycle
   dutyCycle = 100 - dutyCycle;
   newDC = (int) ((float)(curPeriodCnt * dutyCycle) / 100.0F);
   curDC[idx] = newDC;
   
   // Update timer duty cycle
   if (idx == 0)
   {
      htim2.Instance->CCR1 = (newDC <= 1) ? 0 : newDC - 1;
   }
   else
   {
      htim2.Instance->CCR2 = (newDC <= 1) ? 0 : newDC - 1;
   }
   
   // Enable update event if needed
   if (needUpdate)
   {
      htim2.Instance->CR1 &= ~TIM_CR1_UDIS;
   }
   
   // Set Test Trigger
   HAL_GPIO_WritePin (FRONT_LED_GPIO_Port, FRONT_LED_Pin, GPIO_PIN_RESET);
 
   return MotorOpPending;
}

//*****************************************************************************
//*
//* HAL_TIM_PeriodElapsedCallback
//* ---
//*
//*****************************************************************************

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
   cntCallback++;
   
   if (curDir[0] == CW)
   {
      // Set Pan DIRn Low
      HAL_GPIO_WritePin (PAN_DIRn_GPIO_Port, PAN_DIRn_Pin, GPIO_PIN_RESET);
   }
   if (curDir[0] == CCW)
   {
      // Set Pan DIR Low
      HAL_GPIO_WritePin (PAN_DIR_GPIO_Port, PAN_DIR_Pin, GPIO_PIN_RESET);
   }   
   if (curDir[1] == CW)
   {
      // Set Tilt DIRn Low
      HAL_GPIO_WritePin (TILT_DIRn_GPIO_Port, TILT_DIRn_Pin, GPIO_PIN_RESET);
   }
   if (curDir[1] == CCW)
   {
      // Set Tilt DIR Low
      HAL_GPIO_WritePin (TILT_DIR_GPIO_Port, TILT_DIR_Pin, GPIO_PIN_RESET);
   }
   
   //queueEvent (MotorOpFinish);
   
   // Disable further timer update interrupt
   htim2.Instance->CR1 |= TIM_CR1_UDIS;
}

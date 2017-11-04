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
   // Confirm value of period is within the valid range
   period = (period <= PWM_PERIOD_MIN) ? PWM_PERIOD_MIN : 
            (period <= PWM_PERIOD_MAX) ? period : PWM_PERIOD_MAX;
   
   // Configure Motor interface to Power Save Mode Off
   HAL_GPIO_WritePin (MOTOR_PWRSAVE_GPIO_Port, MOTOR_PWRSAVE_Pin, GPIO_PIN_SET);
   
   // Calculate PWM timer clock divisor and overflow count values
   curPreScaleCnt = (int) ((float) (TRACKER_SYS_CLOCK / PWM_PERIOD_DIVISOR) / (float) period);
   curPeriodCnt   = PWM_PERIOD_DIVISOR;
   
   // If calculated clock divisor is larger than maximum
   // force the divisor to max value and calculate the overflow count
   if (curPreScaleCnt > PWM_PRESCALE_MAX)
   {
      curPreScaleCnt = PWM_PRESCALE_MAX;
      curPeriodCnt = (int) ((float) (TRACKER_SYS_CLOCK / PWM_PRESCALE_MAX) / (float) period);
   }
   
   // Disable update interupt when updating the duty cycle
   htim2.Instance->CR1 |= TIM_CR1_UDIS;

   htim2.Instance->ARR  = curPeriodCnt-1;
   htim2.Instance->PSC  = curPreScaleCnt-1;   
   htim2.Instance->CR1 &= ~TIM_CR1_UDIS; 
   
   // Enable update event 
   htim2.Instance->CR1 &= ~TIM_CR1_UDIS;
   
   return MotorOpPending;
}

//*****************************************************************************
//*
//* platformSetDir
//*
//* Platform specific Set Motor Direction (SetDir) Function
//*
//* Affect the specified motor direction and update the pwm duty cycle.
//* If no change in direction, only suty cycle will be updated.
//* Index is motor selection id, Pan = 0, Tilt = 1
//*
//*****************************************************************************

tMotorState platformSetDir (int idx, tMotorDir dir, int dutyCycle)
{
   tMotorState rtnState = NoMotorOpPending;
   
   switch (dir)
   {
      case NoDir :
         dutyCycle = 0;
         break;
      case CW :
         if (curDir[idx] == CCW)
         {
            // Set PWM DIR signal, direction change deferred to update interrupt
            HAL_GPIO_WritePin (pwmDIRPort[idx], pwmDIR[idx], GPIO_PIN_SET);
            rtnState = MotorOpPending;
         }
         else
         {
            // Reset PWM DIRn signal
            HAL_GPIO_WritePin (pwmDIRnPort[idx], pwmDIRn[idx], GPIO_PIN_RESET);
         }
         curDir[idx] = CW;
         break;
         
      case CCW :
         if (curDir[idx] == CW)
         {
            // Set PWM DIRn, direction change deferred to update interrupt
            HAL_GPIO_WritePin (pwmDIRnPort[idx], pwmDIRn[idx], GPIO_PIN_SET);
            rtnState = MotorOpPending;
         }
         else
         {
            // Reset PWM DIR signal
            HAL_GPIO_WritePin (pwmDIRPort[idx], pwmDIR[idx], GPIO_PIN_RESET);
         }
         curDir[idx] = CCW;
         break;
         
      case Brake :
         dutyCycle = 100;
      
         // Set PWM DIR and PWM DIRn signals
         HAL_GPIO_WritePin (pwmDIRPort[idx], pwmDIR[idx], GPIO_PIN_SET);
         HAL_GPIO_WritePin (pwmDIRnPort[idx], pwmDIRn[idx], GPIO_PIN_SET);
      
         curDir[idx] = Brake;
         break;
      default :
         break;
   }
   // Disable update interupt when updating the duty cycle
   htim2.Instance->CR1 |= TIM_CR1_UDIS;
   
   // Calculate the inactive percentage of duty cycle
   dutyCycle = 100 - dutyCycle;
   dutyCycle = (int) ((float)(curPeriodCnt * dutyCycle) / 100.0F);
   
   // Update timer duty cycle
   if (idx == 0)
   {
      htim2.Instance->CCR1 = dutyCycle;
   }
   else
   {
      htim2.Instance->CCR2 = dutyCycle;
   }
   
   // Enable update event 
   htim2.Instance->CR1 &= ~TIM_CR1_UDIS;
   
   return rtnState;
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
   else if (curDir[0] == CCW)
   {
      // Set Pan DIR Low
      HAL_GPIO_WritePin (PAN_DIR_GPIO_Port, PAN_DIR_Pin, GPIO_PIN_RESET);
   }   
   if (curDir[1] == CW)
   {
      // Set Tilt DIRn Low
      HAL_GPIO_WritePin (TILT_DIRn_GPIO_Port, TILT_DIRn_Pin, GPIO_PIN_RESET);
   }
   else if (curDir[1] == CCW)
   {
      // Set Tilt DIR Low
      HAL_GPIO_WritePin (TILT_DIR_GPIO_Port, TILT_DIR_Pin, GPIO_PIN_RESET);
   }
   
   //queueEvent (MotorOpFinish);
   
   // Disable further timer update interrupts
   htim2.Instance->CR1 |= TIM_CR1_UDIS;
}

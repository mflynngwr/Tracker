/*
 ******************************************************************************
 * @file    stm32f3xx_if.c
 * @brief   Tracker Platform Specific Applicaton Interface Functions
 ******************************************************************************
 *
 * COPYRIGHT(c) 2017 Apeiros, LLC
 *
 ******************************************************************************
*/

#include "stm32f3xx_hal.h"
#include "stm32f3xx_if.h"
#include "Tracker_if.h"
#include <string.h>
#include <stdio.h>
   
static char stringbuff[100];
static timeMS lastDebugTime=0;

//*****************************************************************************
//* 
//* debugPrint
//* ---
//*
//*****************************************************************************

void debugPrint (char* msg)
{
   timeMS curTime = getTime();
   sprintf(stringbuff, "%s (%8.8X) [%d] \n\r", msg, getTime(), curTime-lastDebugTime);
	HAL_UART_Transmit(&huart2, (uint8_t*)stringbuff, strlen(stringbuff), 0xFFFF);
   lastDebugTime = curTime;
}

//*****************************************************************************
//* 
//* delay_ms
//* ---
//*
//*****************************************************************************

void delay_ms(timeMS time_ms)
{
	HAL_Delay (time_ms);
}

//*****************************************************************************
//* 
//* getTime
//* ---
//*
//*****************************************************************************

timeMS getTime (void)
{
   return HAL_GetTick();
}

//*****************************************************************************
//* 
//* debugPrint
//* ---
//*
//*****************************************************************************

void startPWM (void)
{
	HAL_TIM_Base_Start_IT(&htim2);
}



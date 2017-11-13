
#include "stm32f3xx_hal.h"
#include "stm32f3xx.h"
#include "stm32f3xx_it.h"

extern SPI_HandleTypeDef hspi2;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim6;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

static unsigned int tmpInt = 0;

void EXTI0_IRQHandler(void)
{
   EXTI->PR = TILT_ENCODER_A_Pin;
   queueEvent (EncTiltAOp);
}

void EXTI1_IRQHandler(void)
{
   EXTI->PR = TILT_ENCODER_B_Pin;
   queueEvent (EncTiltBOp);
}

void EXTI2_TSC_IRQHandler(void)
{
   EXTI->PR = PAN_ENCODER_B_Pin;
   queueEvent (EncPanBOp);
}

void EXTI4_IRQHandler(void)
{
   EXTI->PR = PAN_ENCODER_A_Pin;
   queueEvent (EncPanAOp);
}

void EXTI9_5_IRQHandler(void)
{
   // Check Power Button operation
   if (EXTI->PR & POWER_BTN_Pin)
   {
      // Clear the interrupt
      EXTI->PR = POWER_BTN_Pin;
      
      // Check if button up or down
      tmpInt = POWER_BTN_GPIO_Port->IDR & POWER_BTN_Pin;
      if (tmpInt)
      {
         queueEvent(PowerBtnDown);
      }
      else
      {
         queueEvent(PowerBtnUp);
      }
   }
   
   // Check Front Button operation
   if (EXTI->PR | FRONT_BTN_Pin)
   {
      // Clear the interrupt
      EXTI->PR = FRONT_BTN_Pin;
      
      // Check if button up or down
      tmpInt = FRONT_BTN_GPIO_Port->IDR & FRONT_BTN_Pin;
      if (tmpInt)
      {
         queueEvent(FrontBtnDown);
      }
      else
      {
         queueEvent(FrontBtnUp);
      }
   }
}

void TIM2_IRQHandler(void)
{
   // Clear the Update interrupt
   htim2.Instance->SR   = ~TIM_FLAG_UPDATE;
   // Process the Tracker PWM event
   HAL_TIM_PeriodElapsedCallback (&htim2);
   
   //HAL_TIM_IRQHandler(&htim2);
}

void SPI2_IRQHandler(void)
{
  HAL_SPI_IRQHandler(&hspi2);
}

void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart1);
}

void USART2_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart2);
}

void EXTI15_10_IRQHandler(void)
{
   // Check Red Pine SPI Input Ready interrupt      
   if(EXTI->PR & RS_SPI_IRQ_Pin)
   {
      // Clear SPI input ready interrupt
      EXTI->PR = RS_SPI_IRQ_Pin;
     
      // Queue SPI input read task
      queueEvent (GPSUpdate);
   }

   // Check Test Input pin interrupt      
   if(EXTI->PR & TEST_PT_IN_Pin)
   {
      // Clear Test Input Pin interrupt
      EXTI->PR = TEST_PT_IN_Pin;
      queueEvent (TestOpTPIn);
   }
   
   // Check Nucleo button operation interrupt (NUCLEO board only)  
   if(EXTI->PR & NUCLEO_BTN_Pin)
   {
      // Clear Nucleo button operation interrupt
      EXTI->PR = NUCLEO_BTN_Pin;
     
      // Check for button down operation
      tmpInt = NUCLEO_BTN_GPIO_Port->IDR & NUCLEO_BTN_Pin;
      if (!tmpInt)  // Nucleo button down
      {
         queueEvent (MotorOpTest);
      }
   }  
}

void TIM6_DAC_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim6);
}


/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */
#ifndef HAL_MODULE_ENABLED
   #define HAL_MODULE_ENABLED
#endif
#define HAL_SPI_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED

#define PWM_PERIOD_CNT    200                // 200 counts per period
#define PWM_PRESCALE_MAX  36000              // Establish max for calculations
#define PWM_PSCL_INIT     PWM_PRESCALE_MAX-1 // Yields 2KHz PWM period clock
#define PWM_INIT          PWM_PERIOD_CNT-1   // 100 ms PWM period
#define PWM_DC_INIT       0                  //
#define UTILITY_PRESCALE  36000-1            // Yields 2KHz clock to timer
#define UTILITY_PERIOD    100-1              // 50 ms timer
/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define NUCLEO_BTN_Pin GPIO_PIN_13
#define NUCLEO_BTN_GPIO_Port GPIOC
#define NUCLEO_BTN_EXTI_IRQn EXTI15_10_IRQn
#define PAN_PWM_Pin GPIO_PIN_0
#define PAN_PWM_GPIO_Port GPIOA
#define TILT_PWM_Pin GPIO_PIN_1
#define TILT_PWM_GPIO_Port GPIOA
#define CONSOLE_TX_Pin GPIO_PIN_2
#define CONSOLE_TX_GPIO_Port GPIOA
#define CONSOLE_RX_Pin GPIO_PIN_3
#define CONSOLE_RX_GPIO_Port GPIOA
#define PAN_ENCODER_A_Pin GPIO_PIN_4
#define PAN_ENCODER_A_GPIO_Port GPIOA
#define PAN_ENCODER_A_EXTI_IRQn EXTI4_IRQn
#define POWER_BTN_Pin GPIO_PIN_5
#define POWER_BTN_GPIO_Port GPIOA
#define POWER_BTN_EXTI_IRQn EXTI9_5_IRQn
#define TILT_DIR_Pin GPIO_PIN_6
#define TILT_DIR_GPIO_Port GPIOA
#define TILT_DIRn_Pin GPIO_PIN_7
#define TILT_DIRn_GPIO_Port GPIOA
#define TILT_ENCODER_A_Pin GPIO_PIN_0
#define TILT_ENCODER_A_GPIO_Port GPIOB
#define TILT_ENCODER_A_EXTI_IRQn EXTI0_IRQn
#define TILT_ENCODER_B_Pin GPIO_PIN_1
#define TILT_ENCODER_B_GPIO_Port GPIOB
#define TILT_ENCODER_B_EXTI_IRQn EXTI1_IRQn
#define PAN_ENCODER_B_Pin GPIO_PIN_2
#define PAN_ENCODER_B_GPIO_Port GPIOB
#define PAN_ENCODER_B_EXTI_IRQn EXTI2_TSC_IRQn
#define MODE_LED_Pin GPIO_PIN_10
#define MODE_LED_GPIO_Port GPIOB
#define RS_RESETn_Pin GPIO_PIN_11
#define RS_RESETn_GPIO_Port GPIOB
#define RS_SPI_NSS_Pin GPIO_PIN_12
#define RS_SPI_NSS_GPIO_Port GPIOB
#define RS_SPI_SCK_Pin GPIO_PIN_13
#define RS_SPI_SCK_GPIO_Port GPIOB
#define RS_SPI_MISO_Pin GPIO_PIN_14
#define RS_SPI_MISO_GPIO_Port GPIOB
#define RS_SPI_MOSI_Pin GPIO_PIN_15
#define RS_SPI_MOSI_GPIO_Port GPIOB
#define FRONT_BTN_Pin GPIO_PIN_8
#define FRONT_BTN_GPIO_Port GPIOA
#define FRONT_BTN_EXTI_IRQn EXTI9_5_IRQn
#define FRONT_LED_Pin GPIO_PIN_9
#define FRONT_LED_GPIO_Port GPIOA
#define RS_SPI_IRQ_Pin GPIO_PIN_10
#define RS_SPI_IRQ_GPIO_Port GPIOA
#define RS_SPI_IRQ_EXTI_IRQn EXTI15_10_IRQn
#define PAN_DIR_Pin GPIO_PIN_11
#define PAN_DIR_GPIO_Port GPIOA
#define PAN_DIRn_Pin GPIO_PIN_12
#define PAN_DIRn_GPIO_Port GPIOA
#define DEBUG_TMS_Pin GPIO_PIN_13
#define DEBUG_TMS_GPIO_Port GPIOA
#define DEBUG_CLK_Pin GPIO_PIN_14
#define DEBUG_CLK_GPIO_Port GPIOA
#define TEST_PT_IN_Pin GPIO_PIN_15
#define TEST_PT_IN_GPIO_Port GPIOA
#define TEST_PT_IN_EXTI_IRQn EXTI15_10_IRQn
#define DEGUB_SWO_Pin GPIO_PIN_3
#define DEGUB_SWO_GPIO_Port GPIOB
#define TEST_PT_OUT_Pin GPIO_PIN_4
#define TEST_PT_OUT_GPIO_Port GPIOB
#define MOTOR_PWRSAVE_Pin GPIO_PIN_5
#define MOTOR_PWRSAVE_GPIO_Port GPIOB
#define RS_TX_Pin GPIO_PIN_6
#define RS_TX_GPIO_Port GPIOB
#define RS_RX_Pin GPIO_PIN_7
#define RS_RX_GPIO_Port GPIOB
#define POWER_MGR_SCL_Pin GPIO_PIN_8
#define POWER_MGR_SCL_GPIO_Port GPIOB
#define POWER_MGR_SDA_Pin GPIO_PIN_9
#define POWER_MGR_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
